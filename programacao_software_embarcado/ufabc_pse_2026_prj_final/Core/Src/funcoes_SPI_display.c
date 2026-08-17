/* ----------------------------------------------------------------------------
      UFABC - SuP e PSE - criado em 2024.08 / rev 2026.07
 Programa: funcoes_SPI_display.c -
 Autor:     Joao Ranhel
 Descricao: contem as funcoes para serializar dados para o shield multifuncao,
       fn conversora hexa->7seg, e conversoras de ASCII -> num e vice-versa e
       uma funcao que faz a multiplexacao temporal (varredura) dos displays
 Usa:  arquivo "funcoes_SPI_display.h" que contem os #defines e protótipos
       das funções descritas aqui
/  --------------------------------------------------------------------------*/
#include <funcoes_SPI_display.h>
#include "main.h"

#define DIG_APAGA 0x10                 // kte p/ apagar um dígito no display

/* ------ FUNCAO que converte um inteiro 8-bits em 7 segmentos --------
argmento:  valHEX(valor hexadecimal) + kte TIPO_DISPLAY="0" anodo comum
A ordem dos bits no registrador de deslocamento e':
dp g f e d c b a 0 0 0 0 0 0 0 0  (fazer OR no retorno p/ ligar 1 dos displays)
OBS: esta rotina nao liga o DP   (ele deve ser ligado no retorno)
 ----------------------------------------------------------------------------*/
uint16_t conv_7_seg(int8_t valHEX) {
  uint16_t sseg = 0xFF00;              // inicializa MSByte da palavra com 0xFF
  switch(valHEX)                       // valores default p/ ANODO comum
  {
    case 0x0: {sseg = 0xC000; break;}  // retorna val p/ 0
    case 0x1: {sseg = 0xF900; break;}  // retorna val p/ 1
    case 0x2: {sseg = 0xA400; break;}  // retorna val p/ 2
    case 0x3: {sseg = 0xB000; break;}  // retorna val p/ 3
    case 0x4: {sseg = 0x9900; break;}  // retorna val p/ 4
    case 0x5: {sseg = 0x9200; break;}  // retorna val p/ 5
    case 0x6: {sseg = 0x8200; break;}  // retorna val p/ 6
    case 0x7: {sseg = 0xF800; break;}  // retorna val p/ 7
    case 0x8: {sseg = 0x8000; break;}  // retorna val p/ 8
    case 0x9: {sseg = 0x9000; break;}  // retorna val p/ 9
    case 0xA: {sseg = 0x8800; break;} // retorna val p/ A
    case 0xB: {sseg = 0x8300; break;} // retorna val p/ B
    case 0xC: {sseg = 0xC600; break;} // retorna val p/ C
    case 0xD: {sseg = 0xA100; break;} // retorna val p/ D
    case 0xE: {sseg = 0x8600; break;} // retorna val p/ E
    case 0xF: {sseg = 0x8E00; break;} // retorna val p/ F
    case 0x10: {sseg = 0xFF00; break;} // default = tudo desligado
    default: {sseg = 0xBF00; break;} // ERRO retorna "-" (so' g ligado)
  }
  if (TIPO_DISPLAY == 0)             // 0 = ANODO COMUM sai como a tabela
    return sseg;
  else                               // CATODO COMUM, inverte bits (bitwise)
    return ~sseg;
}


// fn serializa dados de 'ser_data' p/ 74HC595 (shield multifuncao do Arduino)
// Baixa RCLK, envia 16 bits p/ SDATA e pulsa SCLK. Ao final, sobe RCLK
void serializar(uint16_t ser_data) {
  GPIOB->ODR = GPIOB->ODR & ~(0x1 << 6);     // zera pino GPIOB_6
  int stts = 15;                             // 1o. envia bit MSB 1o. = dp
  do {                                       // repete enquanto stts >= 0
    if ((ser_data >> stts) & 1) {            // ser_data desloc >> ssts ='1'
      GPIOB->ODR = GPIOB->ODR | (0x1 << 10); // liga GPIOB_10 (SDATA = 1)
    } else {
      GPIOB->ODR = GPIOB->ODR & ~(0x1 << 10);// desliga B10 (SDATA=0)
    }
    // ajustado o pino SDATA (acima), agora pulsa o clock SCLK
    GPIOB->ODR = GPIOB->ODR | (0x1 << 9);    // liga GPIOB_9
    GPIOB->ODR = GPIOB->ODR & ~(0x1 << 9);   // desliga B9
    -- stts;                                 // dec stts p/ o novo bit
  } while (stts >= 0);
  // depois de serializar os 16 bits, gera RCK p/ paralelizar os dados
  GPIOB->ODR = GPIOB->ODR & ~(0x1 << 10);    // desliga B10
  GPIOB->ODR = GPIOB->ODR | (0x1 << 6);      // liga pino GPIOB_6
}


// fn que converte um dos valores numericos em codigo ASCII
// faz critica do valor 'n' que tem que ser entre 0 e 9
uint8_t conv_num_ASC(int8_t n) {
  // testa se n esta' dentro da faixa numerica
  if ((n < 0) || (n > 9)) return 0x45; // sai cod ascii do 'E' (erro)
  else return (0x00 | (n + 0x30));     // uint8_t <= sai cod ascii
}


// fn que converte ASCII de um numero em valor decimal
// faz critica de cados, o caractere 'c' tem que estar entre 0x30 e 0x39
int8_t conv_ASC_num(uint8_t c) {
  // testa se esta' fora da faixa numerica
  if ((c < 0x30) || (c > 0x39)) return 0x45;  // cod ascii do 'E' (erro)
  else return (c - 0x30);              // int8_t <= sai valor numerico
}


// fn que faz a varredura e mostra no display os dados do vetor Dsp[]
// ajuste o vetor D[] e var pto e chame essa funcao
// chame essa funcao pelo menos 30 x 4displays = 120 vezes por segundo
void mostrar_no_display(int8_t D[], uint8_t pto) {
  // var de estado que controla a varredura (qual display é mostrado)
  static enum {DIG_MIL, DIG_CEN, DIG_DEZ, DIG_UNI} sttVARRE=DIG_UNI;
  uint16_t ser_data;                   // dado a ser serializado
  uint16_t val7seg;                    // dado com quais segmts ligam
  switch(sttVARRE)                     // escolher qual DIG vai varrer
  {
    case DIG_UNI: {                    // unidade (digito menos significante LSD)
      sttVARRE = DIG_DEZ;              // prox digito = DEZENA
      ser_data = 0x0008;               // display #1 (LSD = 0x0008 no shield)
      val7seg = conv_7_seg(D[0]);      // conv dsp1_hex para 7-seg
      if (pto & 0x1) val7seg &= 0x7FFF;    // liga o ponto decimal
      break;
    }
    case DIG_DEZ: {                    // dezena (2o. digito dir <- esq)
      sttVARRE = DIG_CEN;              // ajusta p/ prox digito = CENTENA
      ser_data = 0x00004;              // bit p/ ativar display #2
      if(D[3]>0 || D[2]>0 || D[1]>0) { // se D3 ou D2 > 0; ou D[1] != 0
        val7seg = conv_7_seg(D[1]);    // converte 7-seg
        if (pto & 0x2) val7seg &= 0x7FFF;  // liga o ponto decimal
      } else {
        val7seg = conv_7_seg(DIG_APAGA);   // se D3,D2,D1=0 apaga esse
      }
      break;
    }
    case DIG_CEN: {                    // centena (3o. digito dir <- esq)
      sttVARRE = DIG_MIL;              // ajusta p/ prox digito = MILHAR
      ser_data = 0x0002;               // bit p/ ativar display #3
      if(D[3]>0 || D[2]>0) {           // se D3>0 ou D2>0
        val7seg = conv_7_seg(D[2]);    // converte 7-seg
        if (pto & 0x4) val7seg &=0x7FFF;   // liga o ponto decimal
      } else {
        val7seg = conv_7_seg(DIG_APAGA);   // se D3 e D2=0 apaga esse
      }
      break;
    }
    case DIG_MIL: {                    // milhar (MSD em 4 digitos)
      sttVARRE = DIG_UNI;              // ajusta p/ prox digito = UNIDADE
      ser_data = 0x0001;               // bit p/ ativar display #4
      if(D[3]>0) {                     // se D3 != 0
        val7seg = conv_7_seg(D[3]);    // converte 7-seg
        if (pto & 0x8) val7seg &=0x7FFF;   // liga o ponto decimal
      } else {
        val7seg = conv_7_seg(DIG_APAGA);   // se D3=0 apagar esse
      }
      break;
    }
  }  // fim case
  ser_data |= val7seg;                 // OR com val7seg = dado a serializar
  serializar(ser_data);                // serializa dado p/74HC595 (shift reg)
}

// esta função RESETA os pinos ao inicializar a SPI COM '1'
void reset_pinos_emula_SPI (void)
{
  // garantir que pinos serial comecam com NÍVEL HIGH
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);   // SDATA=1
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);    // SCK=1
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);    // RCK=SS'=1
}
// --- final deste arquivo ---