/* 
 * File:   ios.h
 * Author: Uchenna Osemeka
 * Created on October 30, 2025, 2:24 PM
 */

#ifndef IOS_H
#define	IOS_H

#ifdef	__cplusplus
extern "C" {
#endif

//Event Flags
extern volatile uint16_t programOn;
extern volatile uint16_t LED1Mode;
extern volatile uint16_t LED2Mode;
extern volatile uint16_t PB2OnMode;
extern volatile uint16_t PB3Mode;
extern volatile uint16_t PB2OffMode;

//Global Variables 
extern volatile uint16_t ADCvalue; // 16 bit register used to hold ADC converted digital output ADC1BUF0
extern volatile uint16_t overState; // force leave

void IOinit();
void IOcheck();
void PB2OffBlink();


#ifdef	__cplusplus
}
#endif

#endif	/* IOS_H */

