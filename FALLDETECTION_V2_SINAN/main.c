/*
 * game.c
 *
 *  Created on: Nov 27, 2023
 *      Author: edocasta02
 */

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <math.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdlib.h>
#include <stdint.h>
#include "game.h"
#include "HwInit.h"
#include "acc.h"
#include "alarm.h"
// Configurazione dei pulsanti S1 e S2
#define BUTTON_S1_PORT   GPIO_PORT_P5
#define BUTTON_S1_PIN    GPIO_PIN1

#define BUTTON_S2_PORT   GPIO_PORT_P1
#define BUTTON_S2_PIN    GPIO_PIN1
volatile int situazione = 0;





// Interrupt Service Routine (ISR) per gestire gli interrupt dai pulsanti (opzionale)
void PORT5_IRQHandler(void)
{
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(BUTTON_S1_PORT);
    GPIO_clearInterruptFlag(BUTTON_S1_PORT, status);

    /* check if we received P3.5 interrupt */
    if ((status & BUTTON_S1_PIN))
    {

        situazione = 2;

    }
}

void PORT3_IRQHandler(void)
{
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(BUTTON_S2_PORT);
    GPIO_clearInterruptFlag(BUTTON_S2_PORT, status);
    if ((status & BUTTON_S2_PIN))
    {

        situazione = 2;
    }
}
/*
 * Main function
 */
int main(void)
{
    _hwInit();

    int counter = 10;
    while (1)
    {

        switch (situazione)
        {

        case 0:
            situazione = fallDetected();
           break;
        case 1:
            if(counter>0){
                alarm();
                counter--;
            }
            break;
        case 2:
            playGame();
            break;
        }
        if(counter<=0){
            sos();
        }
    }
}
/* This interrupt is fired whenever a conversion is completed and placed in
 * grabbed and placed in resultsBuffer */
void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

}
