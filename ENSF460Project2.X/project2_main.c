/*
 * File:   assignment4_main.c
 * Author: Uchenna Osemeka
 *
 * Created on: November 2nd, 2025
 */

// FBS
#pragma config BWRP = OFF               // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC oscillator (FRC))
#pragma config IESO = OFF               // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-Speed Start-up disabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = ON            // CLKO Enable Configuration bit (CLKO output disabled; pin functions as port I/O)
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8 MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary oscillator configured for high-power operation)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor Selection (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (WDT prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected; windowed WDT disabled)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default location for SCL1/SDA1 pins)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (MCLR pin enabled; RA5 input pin disabled)

// FICD
#pragma config ICS = PGx2               // ICD Pin Placement Select bits (PGC2/PGD2 are used for programming and debugging the device)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses LPRC as reference clock)
#pragma config RTCOSC = SOSC            // RTCC Reference Clock Select bit (RTCC uses SOSC as reference clock)
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = ON             // Deep Sleep Watchdog Timer Enable bit (DSWDT enabled)

// #pragma config statements should precede project file includes.

#include <xc.h>
#include <p24F16KA101.h>
#include "clkChange.h"
#include "UART2.h"
#include "ios.h"
#include "ADC.h"

uint8_t clkfck= 0; //unused
//Event Flags
uint16_t volatile programOn= 0;
uint16_t volatile LED1Mode= 0;
uint16_t volatile LED2Mode= 0;
uint16_t volatile PB2OnMode= 0;
uint16_t volatile PB3Mode= 0;
uint16_t volatile PB2OffMode= 0;




//Status Flags
uint16_t volatile prsLfilter= 0;  //used for filtering long vs short presses
uint16_t volatile TMR3Flag= 0;
uint16_t volatile changeState= 0;


//Global Variables
uint16_t volatile ADCvalue= 0; // 16 bit register used to hold ADC converted digital output ADC1BUF0
uint16_t volatile overState= 0; // state of output
uint16_t volatile dutyOn= 0;
uint16_t volatile dutyOff= 0;

/**
 * You might find it useful to add your own #defines to improve readability here
 */

int main(void) {
    
    /** This is usually where you would add run-once code
     * e.g., peripheral initialization. For the first labs
     * you might be fine just having it here. For more complex
     * projects, you might consider having one or more initialize() functions
     */
    __builtin_enable_interrupts();   //enable interrupts
    //AD1PCFG = 0xFFFF; /* keep this line as it sets I/O pins that can also be analog to be digital 
    ADCInit();
    newClk(500);   //set Fosc to 500
    IOinit();    //initialize I/O 
    InitUART2();   //initialize UART
  
    while(1) {//keep device on
        Idle();  //stay in Idle
        IOcheck(); //perform correct logic
    }
    return 0;
}
// Timer 1 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void){
    //This interrupt is used for Pulse Width Modulation 
    IFS0bits.T1IF = 0;
    T1CONbits.TON= 0; //start timer
}

// Timer 2 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    /*This interrupt implements debounce on our push buttons. It ensures we read
     * a stable value, then sets the correct flag for our Iocheck()
     */
    //Don't forget to clear the timer 2 interrupt flag!
    IFS0bits.T2IF = 0;
    T2CONbits.TON = 0;   //turn off timer after filter
    if(programOn== 1){
        if(PORTBbits.RB7== 0){
            //on PB1 press while on, filter to decide
            T3CONbits.TCKPS = 2; // set prescaler to 1:64
            TMR3= 0;  
            PR3= 11718;    //set for 3 seconds give or take
            TMR3Flag= 0;        
            T3CONbits.TON = 1;
            prsLfilter= 1;   //flag for press Lenght filter
            
        }
        else if(PORTBbits.RB4== 0){
            //on PB2 press while on, start to blink
            if(PB2OnMode== 1){
                //if already blinking 
                PB2OnMode= 0;
            }
            else if(PB2OnMode== 0){
                PB2OnMode= 1;
                //add blink
            }
        }
        else if(PORTAbits.RA4== 0){
            if(PB3Mode== 1){
                //if already transmitting end transmision
                PB3Mode= 0;
            }
            else if(PB3Mode== 0){
                PB3Mode= 1;
                //start transmitting
            }
        }
    }
    else if(PORTBbits.RB4== 0 && PB2OffMode== 1){
        //TURN PB2Off mode off regardless of other modes
        PB2OffMode= 0;
    }
    
    else if(programOn== 0){
        if(PORTBbits.RB7== 0){
            //on PB1 press while off turn LED1 on
            programOn= 1;
            LED1Mode= 1; //on PB1 press start normal light control-LED1
        }
        else if(PORTBbits.RB4== 0){
            PB2OffMode= 1;
        }
    }
    
    
       
    
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    //TMR3 used for force blinking every 0.5s
    IFS0bits.T3IF = 0;
    overState ^= 1;
    T3CONbits.TON = 1;   //keep timer on
    TMR3Flag= 1;   
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void){
    IFS1bits.CNIF = 0;  //clear CN interrupt flag
    //possible edge case of multiple cn interrupts erasing prsLfilter 
    if(prsLfilter== 1){
        if (TMR3Flag== 1){
        /*If timer has finished and we are filtering, this is a long press*/
             //on PB1 long press, change LED
            //programOn= 1;
            if(LED1Mode==1){
                LED2Mode= 1;
                LED1Mode= 0;
            }
            else if(LED2Mode== 1){
                LED1Mode= 1;
                LED2Mode= 0;  
            }
        
        TMR3Flag= 0;
        }
        else if(TMR3Flag== 0){
         /*If timer has not finished and we are filtering, this is a short press*/
            //go to Off Mode
            programOn= 0;
            LED1Mode= 0;
            LED2Mode= 0;
            PB2OnMode= 0;
            PB3Mode= 0;
            PB2OffMode= 0;
            //All these combine to Off mode
        }
        prsLfilter= 0; //clear filter flag
    }
    
    //Don't forget to clear the CN interrupt flag!
    TMR2 = 0;  
    T2CONbits.TON = 1;   //use tmr2 for debounce filter
        
}

//ADC interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void){
    ADCvalue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
    IFS0bits.AD1IF = 0; // Clear the ADC1 Interrupt Flag
}

