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
#include "adc.h"

// Variáveis Globais aqui se houverem
static int contador=0;
unsigned int distancia;

// Função para tratamento de interrupções
void interrupt isr(void) {
  
  // Tratamento da interrupção do Timer 0
  if (T0IE && T0IF) {
      contador++;
      
    // Interrupção do Timer 0 aqui
      TMR0 = 255-98;
      T0IF=0;
  }
  
  // Tratamento da interrupção do Port B
  if (RBIE && RBIF) {
    
    char portB = PORTB; // leitura do port B limpa interrupção
    
    io_sw_read(portB);      // Necessário para usar a chave
    debug_led_toggle(1);    // exemplo de uso do LED de debug
    
    RBIF = 0; // limpa o flag de interrupção para poder atender nova
  } 
  
  // Tratamento de outras interrupções aqui
}

// Inicialização do Timer 0
void t0_init(void) {
  // Inicialização do Timer 0 aqui
    T0CS=0;
    //T0CS=1;
    PSA=0;
    OPTION_REGbits.PS=0b111;
    TMR0=255-98;
    T0IE=1;
}

// Programa Principal
void main(void) {
  
  //variáveis locais
  //unsigned int loopCount = 0;   // contador para loop
  char message[9];     // "string" para mensagens
  
  // Inicializações
  adc_init_0();
  t0_init();           // inicializa Timer 0
  io_init();           // inicializa chave, LED e Buzzer
  lcd_init();          // inicializa LCD
  debug_init();        // inicializa LEDs para debug
  ei();                // macro do XC8, equivale a GIE = 1, habilita interrupções

  // Configurações iniciais
  io_beep(100);        // dá um beep
  
  lcd_cursor(0);       // desliga o cursor do LCD  
  lcd_puts("T6G1-DG"); // mostra mensagem inicial
  
  // Loop principal (infinito))
  while(1) {
    //int dist;
    if (io_sw_pressed()) // exemplo de uso de detecção de chave pressionada
      io_led_toggle();   // exemplo de uso: chaveia o LED caso chave pressionada
    
    if(contador>39){  // impressão da distancia lida a cada 200ms, ou 40 interrupções de 5ms do TMR0
      //delay_ms(100);
      distancia = adc_read_0();
      //delay_ms(100);
      contador = 0;
      if(distancia>850){
          distancia = 0;
          sprintf(message,"%04d",distancia); // exemplo de como carregar um string formatado
          lcd_goto(64);        // exemplo: posiciona o cursor do LCD na 1a. posição da 2a. linha
          lcd_puts(message);   // exemplo de como colocar string no display LCD
      }
                
        //dist= adc_read_0();
        //contador = 0; 
        //distancia = adc_read_0();
        //lcd_clear();
      else{
          distancia = 300 - 0.3529*distancia;
          sprintf(message,"%04d mm",distancia); // exemplo de como carregar um string formatado
          lcd_goto(64);        // exemplo: posiciona o cursor do LCD na 1a. posição da 2a. linha
          lcd_puts(message);   // exemplo de como colocar string no display LCD
      }
    }
    
    delay_ms(1);         // exemplo de espera de 1 ms
           
  }
}
