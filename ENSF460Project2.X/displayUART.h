/* 
 * File:   displayUART.h
 * Author: admin_user
 *
 * Created on November 4, 2025, 2:56 PM
 */

#ifndef DISPLAYUART_H
#define	DISPLAYUART_H

#ifdef	__cplusplus
extern "C" {
#endif

extern volatile uint16_t timerCount;
extern volatile uint16_t ADCvalue;

void displayInfo();
void dispADC();
void sendtoPython();

#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAYUART_H */

