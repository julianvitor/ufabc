/* --- arquivo header para serial do display 4x7seg e conversor bcdx7seg ----
Arquivo: funcoes_SPI_display.h
Created on: 2024.08  -  rev 2026.07
Author: JRanhel
-----------------------------------------------------------------------------*/

#ifndef FUNCOES_SPI_DISPLAY_H_
#define FUNCOES_SPI_DISPLAY_H_

#include "main.h"

// as constantes usadas nos arquivos com as funcoes.c
#define TIPO_DISPLAY 0                 // tipo de display 0 = Anodo comum

// fn converte um valor hexa (valHEX = uint8_t) em 7-seg (retorna um uint16_t).
// Argumentos: valHEX + constante TIPO_DISPLAY (definida no main.h)
uint16_t conv_7_seg(int8_t valHEX);

// fn serializa dados p/ 74HC595 (shield multifuction do Arduino)
// Baixa RCLK, envia 16 bits p/ SDATA e pulsa SCLK. Ao final, sobe RCLK
void serializar(uint16_t ser_data);    // prot fn serializa dados p/ 74HC595

// fn que converte um dos valores numericos em codigo ASCII
// faz critica do valor 'n' que tem que ser entre 0 e 9
uint8_t conv_num_ASC(int8_t n);

// fn que converte ASCII de um numero em valor decimal
// faz critica de cados, o caractere 'c' tem que estar entre 0x30 e 0x39
int8_t conv_ASC_num(uint8_t c);

// fn que faz a varredura e mostra no display os dados do vetor Dsp[]
void mostrar_no_display(int8_t D[], uint8_t pto);

// fn que reseta pinos PB6, PB9, PB10 para começarem em nível '1'
void reset_pinos_emula_SPI (void);

#endif  /* FUNCOES_DISPLAY_H_ */