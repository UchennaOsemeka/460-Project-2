/* 
 * File:   ADC.h
 * Author: Uchenna Osemeka
 *
 * Created on November 22, 2025, 5:44 PM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif
//Event Flags
extern volatile uint16_t programOn;
extern volatile uint16_t LED1Mode;
extern volatile uint16_t LED2Mode;
extern volatile uint16_t PB2OnMode;
extern volatile uint16_t PB3Mode;

//Global Variables 
extern volatile uint16_t ADCvalue; // 16 bit register used to hold ADC converted digital output ADC1BUF0
extern volatile uint16_t overState; // force leave
extern volatile uint16_t dutyOn; // time on for PWM
extern volatile uint16_t dutyOff; // time off for PWM

void ADCInit();
void do_ADC();
void ADCtoLED1();
void ADCtoLED2();
void ONMode();
void ADCEnd();
void delay_sample();



#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

