#include <xc.h>
#include <p24F16KA101.h>
#include <stdio.h>
#include "displayUART.h"
#include "UART2.h"

// Disp2String("PB2 and PB3 are pressed\n\r");
void displayInfo(){
    /*Display info if all buttons are pressed and timer is off
     */
    Disp2String("2025 ENSF 460 L02 Group 28\r\n");
}
//EXPERIMENT WITH ansii tomorrow
/*void sispClrTerm() {
    // "\033[2K" clears the entire line
    // "\r" moves the cursor back to the beginning
    const char* clear_line = "\033[2K\r";
    uart_send_string(clear_line);
}
 */


void dispADC(){
    char bar[30];
    int barIndex= 0;
    for(uint16_t temp= ADCvalue; temp > 0 && barIndex < sizeof(bar)-1; temp -=50 ){
        bar[barIndex]= '*';
        barIndex += 1;
    }
    bar[barIndex] = '\0';
    char disp[64];
    sprintf(disp, "Mode 0:%s  0x%04X\r\n", bar, ADCvalue );
    //\f\r experiment
    Disp2String(disp);
}

void sendtoPython(){
    char disp[64];
    sprintf(disp, "%u\n", ADCvalue);  // convert ADCvalue to string and add newline
    Disp2String(disp);                // send string over UART
}