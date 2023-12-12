#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <math.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdlib.h>
#include <stdint.h>

#include "HwInit.h"
#include "acc.h"

#define Z 2
#define N_SAMPLE 10000
#define SIGMAS 3
#define MIN 200
#define NOISE_DEV 4

#define BUTTON_1_PRESSED !GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1)
#define BUTTON_2_PRESSED !GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5)

//Globals
const uint32_t uart_baudrate = 115200;
const uint32_t min = (MIN - 3*NOISE_DEV);

// Graphic library context
Graphics_Context g_sContext;


void startFallAlert()
{
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "CADUTA!",
    AUTO_STRING_LENGTH,64, 30,OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Press S1 if OK",
    AUTO_STRING_LENGTH,64, 50,OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Press S2 if SOS",
    AUTO_STRING_LENGTH,64, 70,OPAQUE_TEXT);

    char string[3];
    int i;
    bool isOK = 0;
    for(i=10000000; i>=0; i--)
    {
        if(i%1000000 == 0)  // Countdown
        {
            sprintf(string, "%2d", i/1000000);
            Graphics_drawStringCentered(&g_sContext, (int8_t *)string, AUTO_STRING_LENGTH, 64, 90, OPAQUE_TEXT);
        }

        if(BUTTON_1_PRESSED)    // S1 button pressed
        {
            isOK = 1;
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, (int8_t *) "OK",
            AUTO_STRING_LENGTH,60, 64,OPAQUE_TEXT);
            _delay_cycles(5000000);
            Graphics_clearDisplay(&g_sContext);
            break;
        }
        if(BUTTON_2_PRESSED)    // S2 button pressed
        {
            break;
        }
    }
    if(!isOK)
    {
        //startSOS();
    }
    Graphics_clearDisplay(&g_sContext);
}

int fallDetected(){
int tmpZ_1, tmpZ_2, detect = 0;


    tmpZ_1 = ADC14_getResult(ADC_MEM2);
    _delay_cycles(25);
    tmpZ_2 = ADC14_getResult(ADC_MEM2);

        if(tmpZ_1 < min ){
            if(tmpZ_2 < min){
                detect = 1;
            }
        }


    if(detect){
       return 1;
    }
    return 0;

}
