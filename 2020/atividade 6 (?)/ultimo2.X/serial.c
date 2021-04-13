/*
 * File:   serial.c
 * Author: Jun Okamoto Jr.
 *
 * Created on May 11, 2020, 21:43
 */

#ifndef SERIAL_C
#define SERIAL_C

#include <xc.h>
#include <conio.h>
#include <stdio.h>
#include "always.h"
#include "delay.h"

void serial_init(void) {
  // escreva a fun��o de inicializa��o aqui
}

unsigned char chkchr (void) {
  // escreva sua fun��o aqui
}

// Retira um caractere do buffer de Rx
// n�o sai enquanto no chegar caractere no buffer de Rx.
unsigned char getch(void) {
  while(!RCIF); 
  unsigned char c = 255;
  while (c == 255) c = chkchr();
  return c;
}

// Escreve um caractere no canal serial
void putch(unsigned char c) {
  // escreva sua fun��o aqui
}

// Envia um string pelo canal serial
// se o string contiver <CR>, adiciona <LF>
// se o string contiver <LF>, adiciona <CR>
void putst(register const char *str) {
  while((*str)!=0) {
    putch(*str);
    if (*str==13) putch(10);
    if (*str==10) putch(13);
    str++;
  }
}

#endif
