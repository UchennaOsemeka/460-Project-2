/* 
 * File:   ADC.c
 * Author: Uchenna Osemeka
 *
 * Created on November 22, 2025, 5:44 PM
 */

#include <p24F16KA101.h>
#include <xc.h>
#include "UART2.h"
#include "ADC.h"
#include "displayUART.h"

void ADCInit(){
    AD1PCFG = 0xFFDF;  //configured for AN5 on pin ra3, pin 8
    TRISAbits.TRISA3= 1;   //input on A3-AN5
    
    /* ------------- ADC INITIALIZATION ------------------*/
     // Configure ADC by setting bits in AD1CON1 register
    AD1CON1bits.SSRC = 0b111;     // Auto-conversion
    AD1CON1bits.ADSIDL= 0; //Stop in Idle Mode bit
    AD1CON1bits.SSRC = 0b111;     // Auto-conversion
    AD1CON1bits.ASAM = 0;  // Automatic sampling
  
    // Configure ADC by setting bits in AD1CON2
    AD1CON2bits.VCFG = 0b000; // Selects AVDD, AVSS (supply voltage to PIC) as Vref
    
    // Configure ADC by setting bits in AD1CON3
    AD1CON3bits.ADRC = 0; // Use system clock
    AD1CON3bits.ADCS = 20;        // AD clock = (ADCS + 1) * Tcy
    AD1CON3bits.SAMC = 15;        // Auto-sample for 10 T_AD
     //Configure the ADC?s sample time by setting bits in AD1CON3
     // Ensure sample time is 1/10th of signal being sampled
     // Select and configure ADC input
    
    /* ------------- ADC SAMPLING AND CONVERSION ------------------*/
    AD1CHSbits.CH0SA= 0b0101; //listen on AN5
    
    /* ------------- ADC Interrupts------------------*/
    IPC3bits.AD1IP= 3; //set interrupt priority of 3
    IFS0bits.AD1IF = 0; // Clear the ADC1 Interrupt Flag
    IEC0bits.AD1IE= 1; //enable interrupt on conversion done
    AD1CON1bits.ADON = 1; // turn on ADC module
}
void do_ADC(){
    AD1CON1bits.SAMP=1; //Start Sampling, Conversion starts automatically
    Idle(); //interrupt will release from idle())
    /* Polling option
    while(AD1CON1bits.DONE==0)
    {}*/
    Disp2Hex(ADCvalue);
    ADCvalue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
    AD1CON1bits.SAMP=0; //End Sampling,
    /*
    TMR1= 0;
    PR1=  24999;  //1sec timer
    T1CONbits.TON= 1; //start timer
    Idle();
     */
    return;
}

void ONMode(){
    uint16_t firstTime= 1;
    while(LED1Mode== 1 || LED2Mode== 1){ //if flags have changed, exit 
        
        if(firstTime== 1){
            T1CONbits.TON= 1; //start timer
            firstTime= 0;
        }
        Idle();
        if(overState== 0){
            do_ADC();
            if(PB3Mode== 1){
                sendtoPython();
            }
            uint16_t maxDuty= 309;
            uint16_t maxADC= 1023;
            dutyOn= ((uint32_t)maxDuty * ADCvalue)/maxADC;
            if (dutyOn < 12){
                dutyOn= 12;
            }
            dutyOff= 310- dutyOn;
            while(overState == 0 && PB2OnMode== 1){//global variable, fires every second and prompts a recalculation
                    Idle();
            }
        }
    }
}


void ADCEnd(){
    AD1CON1bits.ADON = 0; // turn off ADC module
}

