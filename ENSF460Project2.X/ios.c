/*
 * File:   ios.c
 * Author: Uchenna Osemeka
 *
 * Created on: 21st October, 2025
 */

#include <xc.h>
#include <p24F16KA101.h>
#include "ios.h"
#include "ADC.h"
#include "UART2.h"
#include "displayUART.h"



void IOinit(){ //initialize input/output
    //T3CON config
    T3CONbits.TCKPS = 1; // set prescaler to 1:8
    T3CONbits.TCS = 0; // use internal clock
    T3CONbits.TSIDL = 0; //operate in idle mode
    IPC2bits.T3IP = 3; //7 is highest and 1 is lowest pri.
    IFS0bits.T3IF = 0; //clear interrupt flag
    IEC0bits.T3IE = 1; //enable timer interrupt
    
    
    //T2CON config
    T2CONbits.T32 = 0; // operate timer 2 as 16 bit timer
    T2CONbits.TCKPS = 1; // set prescaler to 1:8
    T2CONbits.TCS = 0; // use internal clock
    T2CONbits.TSIDL = 0; //operate in idle mode
    IPC1bits.T2IP = 2; //7 is highest and 1 is lowest pri.
    IFS0bits.T2IF = 0; //clear interrupt flag
    IEC0bits.T2IE = 1; //enable timer interrupt
    PR2 = 1800; // set to a debounce that's just right
    
    //T1CON config
    T1CONbits.TCKPS = 1; // set prescaler to 1:8
    T1CONbits.TSYNC = 0; // do not sync to external clock
    // T1CONbits.TGATE = 0;
    T1CONbits.TSIDL = 0; //operate in idle mode
    IPC0bits.T1IP = 4; //set priority 
    IFS0bits.T1IF = 0; //clear interrupt flag
    IEC0bits.T1IE = 1; //enable timer interrupt
    
    
    /* Let's set up some I/O */
    /*Here we set directions, initial values, and enable internal
     pull-ups
     */
    TRISBbits.TRISB9 = 0;  
    LATBbits.LATB9 = 0;
    TRISAbits.TRISA6 = 0;
    LATAbits.LATA6 = 0;
    
    TRISAbits.TRISA4 = 1;
    CNPU1bits.CN0PUE = 1;
    CNEN1bits.CN0IE = 1;
    
    TRISBbits.TRISB4 = 1;
    CNPU1bits.CN1PUE = 1;
    CNEN1bits.CN1IE = 1;
    
    TRISBbits.TRISB7 = 1;
    CNPU2bits.CN23PUE = 1;
    CNEN2bits.CN23IE = 1;
    
  
    
    IPC4bits.CNIP = 6; //set priority for CN interrupts
    IFS1bits.CNIF = 0; //clear CN flag
    
    IEC1bits.CNIE = 1; //enable CN interrupts
    
    TMR3= 0;
    PR3=  12500;  //0.5 sec timer
    T3CONbits.TON= 1; //start timer
}

void IOcheck(){
 /*On return from CN and then Timer 2 Interrupts, an event flag is raised. 
  * IOcheck, the next instruction after Idle() in main, is called. IOcheck uses
  * if else logic to call the appropriate action defined in timerDelay
  */
    if(programOn== 1){
        if(LED1Mode== 1){
            ADCtoLED1(); 
        }
        else if(LED2Mode== 1){
            ADCtoLED2();
        }
    }
    else if(programOn== 0){
        ADCEnd();
        if(PB2OffMode== 1){
            PB2OffBlink();
        }
    }    
}

void PB2OffBlink(){
    while(PB2OffMode== 1){
        LATBbits.LATB9^= 1;
        TMR3= 0;
        PR3=  12500;  //0.5sec timer
        T3CONbits.TON= 1; //start timer
        Idle();
    }
    if(PB2OffMode== 0){
            LATBbits.LATB9= 0;
        }
}