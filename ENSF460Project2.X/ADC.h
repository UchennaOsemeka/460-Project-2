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

extern volatile uint16_t ADCvalue; 
void ADCInit();
void do_ADC();
void ADCEnd();


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

