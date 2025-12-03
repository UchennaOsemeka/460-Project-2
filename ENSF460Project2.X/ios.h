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

extern volatile uint16_t incrSecond;
extern volatile uint16_t incrMinute;
extern volatile uint16_t decrSecond;
extern volatile uint16_t decrMinute;
extern volatile uint16_t resTmr;
extern volatile uint16_t dispInfo;
extern volatile uint16_t strtTmr;
extern volatile uint16_t timerOn;
extern volatile uint16_t prsLfilter;
extern volatile uint16_t paused;
extern volatile uint16_t pseTmr;
extern volatile uint16_t restartTmr;
extern volatile uint16_t TMR3Flag;

extern volatile uint16_t timerCount;
extern volatile uint16_t tmrSet;
extern volatile uint16_t mode;
extern volatile uint16_t ADCvalue;
extern volatile uint16_t on;
extern volatile uint16_t overState;

void IOinit();
void IOcheck();


#ifdef	__cplusplus
}
#endif

#endif	/* IOS_H */

