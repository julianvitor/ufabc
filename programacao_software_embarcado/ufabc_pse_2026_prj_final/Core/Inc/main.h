/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * rev João Ranhel - 07/2026.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>  // porque vou usar rand()
#include <string.h>  // para criar uma string de msgs

#include "cmsis_os2.h"
#include "funcoes_SPI_display.h"  // funções de display 7-segm e conversores
#include "stdint.h"               // porque uso: uint8_t, int16_t, uint32_t

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/*  -=-=-=- Algumas definicoes desse projeto (Prof. J Ranhel):  -=-=-=-
"rqcrn" (placa Server) requisita o envio do valor do CRONOMETRO
"rqadc" (placa Server) requisita o envio do valor de tensao mV, do ADC
"rqsrv" (placa Client) requisita que o 'Server' mostre seus valores
"rqoff" (placa Client) requisita desligamento do serviço pelo 'Server'
"msnos" (placa Client) informa que não está oferecendo serviço mais
"oper?" (minha) envia para requisitar PING
"oper!" (outra) no PING, responde que está ouvindo a minha placa
"aXXXX" (placa Client) envia 'a' mais os digitos do ADC em ASCII
"cXXXX" (placa Client) envia 'c' mais os digitos do cronometro em ASCII
~ A mensagem é montada do LSD BufOUT[0] p/ MSD BufOUT[4], c/ 5 caracteres.
~ Para receber um valor é necessário requisitá-lo.
~ O intervalo entre solicitacoes deve ser < 100 ms (para obter val decimo seg).
~ Para nao ter que escrever defini as macros seguintes:
BufOUT[0]='r'; BufOUT[1]='q'; BufOUT[2]='s'; BufOUT[3]='r'; BufOUT[4]='v'; */

#define REQCRN "rqcrn"  // define a string para pedir leitura cronometro
#define REQADC "rqadc"  // define a string para pedir leitura adc
#define REQSRV "rqsrv"  // define a string para solicitar servico
#define REQOFF "rqoff"  // define a string para descartar servico
#define MSGNSV "msnos"  // string com msg que não está atendendo serviços
#define PNGPRG "oper?"  // define a string msg: PING operando?
#define PNGRSP "oper!"  // define a string msg: PING ok, operando!

// clang-format off
#define FLAG_MASK_SERVICE   0b00000001 // Bit 0
#define FLAG_MASK_MSNOS     0b00000010 // Bit 1
#define FLAG_MASK_TESTE     0b00000100 // Bit 2
#define FLAG_MASK_PING      0b00001000 // Bit 3
#define FLAG_MASK_RQDADOS   0b00110000 // bits 4 e 5
#define FLAG_MASK_RESPPING  0b01000000 // Bit 6

// Extratores para os eventGroups. id é o handle do event group
#define GET_FLAG_SERVICE(id)  ((osEventFlagsGet(id) & FLAG_MASK_SERVICE) != 0) 
#define GET_FLAG_MSNOS(id)    ((osEventFlagsGet(id) & FLAG_MASK_MSNOS) != 0)
#define GET_FLAG_TESTE(id)    ((osEventFlagsGet(id) & FLAG_MASK_TESTE) != 0) 
#define GET_FLAG_PING(id)     ((osEventFlagsGet(id) & FLAG_MASK_PING) != 0) 
#define GET_FLAG_RQDADOS(id)  ((osEventFlagsGet(id) & FLAG_MASK_RQDADOS) >> 4) 
#define GET_FLAG_RESPPING(id) ((osEventFlagsGet(id) & FLAG_MASK_RESPPING) != 0) 
// clang-format off

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

// macro = função que copia uma string (n chars) para o BufOUT[] (n items)
// char *strncpy(char *dest, const char *src, size_t n)
// copies up to n characters from the string pointed to by *src to 'dest'.
// strlen() function calc the length of a string (returns a size_t ( uint )
#define STR_BUFF(str)                                                          \
  do {                                                                         \
    const char *src = str;                                                     \
    strncpy((char *)(BufOUT), src, sizeBuffs);                                 \
  } while (0)

#define SEND_MSG(msg_enum)                                                     \
  do {                                                                         \
    uint8_t msg = msg_enum;                                                    \
    osMessageQueuePut(q_uart_msgsHandle, &msg, 0, 0U);                         \
  } while (0)

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

// essas macros definem kts e delays comuns solicitadas no projeto
#define MD_CRONO 0            // cronômetro =0 incrementa, =1 decrementa
#define DT_MUX_DISP 6         // dt = ~7ms para multiplex display (142 vz/s)
#define DT_NEWREQ_CRN (90 - 1)  // DT = ~90ms p/ nova requisição de cronometro
#define DT_NEWREQ_ADC (240 - 1) // DT = ~240ms p/ nova requisição de adc
#define DT_ADC (500 - 1)        // def samples/seg (2s/s = 500ms delay)
#define DT_CRONO (100 - 1)      // dt = 99 ms (== 100 e ajusta crono, reset p/ 00)
#define DT_DISPLAY_MD1 (4000 - 1) // modo 1 muda display a cada 4000 ms
#define DT_DISPLAY_MD2 (2000 - 1) // modo 2 muda display a cada 2000 ms
#define DT_LEDS (200 - 1)         // intervalo tempo para piscar leds e buzzer
#define DT_ERROC (80 - 1)         // intervalo curto para indicar erro
#define DT_ERROL (2000 - 1)       // intervalo longo para indicar erro
#define DT_PING (250 - 1)
#define DT_PING_TIMEOUT (240 - 1)
#define DT_INICIALIZACAO (3000 - 1)
// para funcoes_display
#define NDIGSDISP 4      // quatro digitos nos displays
#define NUMSEGS 7        // ligar 7 segs (leds) em cada display
#define DIG_APAGADO 0x10 // kte valor p/ apagar um dígito no display
// para debouncing
#define DT_DEBOUNCING (150 - 1) // delay P/ debouncing - ~250 ms (no xx_it.c)
#define DT_EFEITO_INI (75 - 1)  // delay p/ efeito inicial nos LEDs ~75 ms

typedef enum {
  sndNADA,
  sndCRN,
  sndADC,
  sndREQCRN,
  sndREQADC,
  sndREQSRV,
  sndREQOFF,
  sndMSGNSV,
  sndREQPING, // oper?
  sndRESPING // oper!
} EvtEnviar; // valores enumerados dos sinais/msgs a serem ENVIADAS

typedef enum {
  STT_DISPLAY_TESTE, // Valor Inicial
  STT_DISPLAY_NOSSO,
  STT_DISPLAY_OUTRO_CRN,
  STT_DISPLAY_OUTRO_ADC
} SttDisplay;

// @definições para teste de uma função minha, vcs não vão utilizar...
// #define GPIOA_IDR *((uint32_t *)0x40010C08ul) // end. reg GPIOB Input Data
// #define GPIOB_ODR *((uint32_t *)0x40010C0Cul) // end. reg GPIOB Output Data
#define PA1 1u               // em que pino o PA1 está (qtos bits deslocar)
#define PA2 2u               // em que pino o PA2 está (qtos bits deslocar)
#define PA3 3u               // em que pino o PA3 está (qtos bits deslocar)
#define DT_CKECKEYS 10 - 1   // dt p/ verificar teclas/botões (10ms)
#define PA1CHECK 0xFFFFFFFFu // check estado default do botão PA1 (tudo '1')
#define PA2CHECK 0xFFFFFFFFu // check estado default do botão PA2 (tudo '1')
#define PA3CHECK 0xFFFFFFFFu // check estado default do botão PA3 (tudo '1')
// @até aqui, definições só minhas...

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
