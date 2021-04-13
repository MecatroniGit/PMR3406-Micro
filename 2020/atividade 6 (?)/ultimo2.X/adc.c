/*
 * File:   adc.c
 * Author: Daniel Imai e Gustavo Rubo
 *
 * Created on 13 de Maio de 2020, 00:02
 */

#include <xc.h>
#include "adc.h"
#include <stdio.h>
#include"delay.h"
#include "always.h"

void adc_init_0(void){
    TRISA0 = 1; //Seta como uma entrada , nao sei se é só trisa
    ANS0 = 1; //Seta como analógica
    ADCS1 = 1; //ADCS1 e ADCS0 definem o divisor de Fosc
    ADCS0 = 0;
    VCFG1 = 0;//Seta VRef- como 0V
    VCFG0 = 0;//Seta VRef+ como 5V
    ADCON0bits.CHS=0; //Seta o canal AN0 como desejado
    ADFM = 1; //Justificando a direita (verificar se é melhor)
    ADON = 1; //ligar o conversor
    
    delay_us(5);//delay de 5 ns para inicialização
    
}

unsigned int adc_read_0(void){
    GO = 1; //Habilita o ADC para realizar a conexão
    while (!ADIF); //Loop de espera do cálculo, na ausencia da flag de interrupção do mesmo
    int distancia= ADRESH*256+ADRESL; // Salva o valor encontrado da conversão
    ADRESH=0;
    ADRESL=0;
    ADIF=0; // reseta o flag para uma conversão seguinte
    
    return distancia;
    
    }
