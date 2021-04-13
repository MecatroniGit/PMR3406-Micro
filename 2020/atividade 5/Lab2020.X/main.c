/*
 * File:   main.c
 * Author: Jun Okamoto Jr.
 *
 * Created on April 18, 2020, 12:58 PM
 */


// PIC16F886 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = EC        // Oscillator Selection bits (EC: I/O function on RA6/OSC2/CLKOUT pin, CLKIN on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include "always.h"
#include "delay.h"
#include "io.h"
#include "lcd.h"
#include "adc.h"
#include "debug.h"

__EEPROM_DATA(0, 1, -1, 0, -1, 0, 0, 1);
__EEPROM_DATA(1, 0, 0, -1, 0, -1, 1, 0);

// Variáveis Globais aqui se houverem
int contagem_pulsos;
int contagem_pulsos_anterior;
int contagem_interrupcoes_tmr0;
short estado_atual;
short estado_anterior;
signed char direcao;
signed char matrizTransicaoEstados[4][4];

// Função para tratamento de interrupções
void interrupt isr(void) {
  char message[9];
  
  // Tratamento da interrupção do Timer 0
  if (T0IE && T0IF) {
      // Executar a cada 500ms
      if (contagem_interrupcoes_tmr0++ == 50) {
//          float pulsos_segundo = (contagem_pulsos - contagem_pulsos_anterior) / 0.5;
          
          sprintf(message, "%f", (contagem_pulsos - contagem_pulsos_anterior) / 0.5);
          lcd_goto(64);
          lcd_puts(message);
          
          contagem_pulsos_anterior = contagem_pulsos;
          contagem_interrupcoes_tmr0 = 0;
      }
      
      TMR0 = 255 - 195; //interrupções a cada 10ms
      T0IF = 0;         //reseta a interrupt flag
  }
  
  // Tratamento da interrupção do Port B
  if (RBIE && RBIF) {
    
    char portB = PORTB; // leitura do port B limpa interrupção
    
    io_sw_read(portB);      // Necessário para usar a chave
    debug_led_toggle(1);    // exemplo de uso do LED de debug
        
    estado_atual = (portB & 0b00011000) >> 3;
    direcao = matrizTransicaoEstados[estado_anterior][estado_atual];
    estado_anterior = estado_atual;

    contagem_pulsos += direcao;

    sprintf(message, "%d", contagem_pulsos);
    lcd_goto(0);
    lcd_puts(message);
    
    RBIF = 0; // limpa o flag de interrupção para poder atender nova
  } 
  
  // Tratamento de outras interrupções aqui
}

// Inicialização do Timer 0
void t0_init(void) {
    T0CS = 0;   //usar timer0
    PSA = 0;    //prescaler usa timer0
    //define prescaler em 256:
    OPTION_REGbits.PS = 0b111;
    TMR0 = 255 - 195; //interrupções a 10ms
    T0IE = 1;   //habilita interrupções do timer0
}

// Iniciaçização das interrupções do PortB aqui
void portB_init(void) {
    TRISB3 = 1; //RB3 como entrada
    TRISB4 = 1; //RB4 como entrada
    ANS11 = 0;  //RB3 como digital
    ANS9 = 0;   //RB4 como digital
    IOCB3 = 1;  //habilita IoC do RB3
    IOCB4 = 1;  //habilita IoC do RB4
    nRBPU = 1;  //desabilita weak pull ups
    RBIE = 1;   // habilita interrupções do portB
}

// Inicialização da matriz de estados
void matriz_estados_init(void){
  unsigned char contador = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      matrizTransicaoEstados[i][j] = (signed char)EEPROM_READ(contador);
      contador ++;
    }
  }
}

// Programa Principal
void main(void) {
  
  //variáveis locais
  unsigned int loopCount = 0;   // contador para loop
  char message[9];     // "string" para mensagens
  
  //inicialização variáveis globais
  contagem_pulsos = 0;
  contagem_pulsos_anterior = 0;
  contagem_interrupcoes_tmr0 = 0;
  estado_atual = 0;
  estado_anterior = 0;
  direcao = 1;
  
  // Inicializações
  t0_init();             // inicializa Timer 0
  portB_init();          // inicializa portB
  matriz_estados_init(); //inicializa matriz de transição de estados
  io_init();             // inicializa chave, LED e Buzzer
  lcd_init();            // inicializa LCD
  debug_init();          // inicializa LEDs para debug
  ei();                  // macro do XC8, equivale a GIE = 1, habilita interrupções

  // Configurações iniciais
  io_beep(100);        // dá um beep
  
  lcd_cursor(0);       // desliga o cursor do LCD  
  lcd_goto(0);
  lcd_puts("T6G1-DIGR"); // mostra mensagem inicial
  lcd_clear();
  
  // Loop principal (infinito))
  while(1) {
    
    if (io_sw_pressed()) // exemplo de uso de detecção de chave pressionada
      io_led_toggle();   // exemplo de uso: chaveia o LED caso chave pressionada
    
    delay_ms(1);         // exemplo de espera de 1 ms
  }
}
