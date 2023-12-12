

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <math.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdlib.h>
#include <stdint.h>
#include "alarm.h"



void flashLights()
{

    // Toggle P1.0 output
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);

}
void flashRedScreen()
{

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    GrContextFontSet(&g_sContext, &g_sFontCmtt26);
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, (int8_t*) "CADUTA!",
    AUTO_STRING_LENGTH,
                                64, 30,
                                OPAQUE_TEXT);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_drawStringCentered(&g_sContext, (int8_t*) "Premi 1 per OK",
    AUTO_STRING_LENGTH,
                                64, 50,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t*) "Premi 2 per SOS",
    AUTO_STRING_LENGTH,
                                64, 70,
                                OPAQUE_TEXT);
    flashLights();
    __delay_cycles(7000000);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontCmtt26);
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, (int8_t*) "CADUTA!",
    AUTO_STRING_LENGTH,
                                64, 30,
                                OPAQUE_TEXT);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_drawStringCentered(&g_sContext, (int8_t*) "Premi 1 per OK",
    AUTO_STRING_LENGTH,
                                64, 50,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t*) "Premi 2 per SOS",
    AUTO_STRING_LENGTH,
                                64, 70,
                                OPAQUE_TEXT);
    flashLights();
    __delay_cycles(7000000);
}

void generateBeep()
{
    // Imposta il duty cycle per generare il suono desiderato
    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_4,
                            10000);  // Duty Cycle al 100%

    // Aspetta per un breve periodo per generare il suono
    __delay_cycles(5000000); //durata del suono

    // Azzera il duty cycle per fermare il suono
    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_4,
                            0);
}
void generateBeep2()
{
    // Imposta il duty cycle per generare il suono desiderato
    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_4,
                            10000);  // Duty Cycle al 100%

    // Aspetta per un breve periodo per generare il suono
    __delay_cycles(5000000); //durata del suono

    // Azzera il duty cycle per fermare il suono
    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_4,
                            0);
}

void alarm()
{
    // Genera il suono
    //generateBeep();

    flashRedScreen();

}
void sos(){
   // generateBeep2();
    Graphics_clearDisplay(&g_sContext);
       GrContextFontSet(&g_sContext, &g_sFontCmtt26);

       Graphics_drawStringCentered(&g_sContext, (int8_t*) "SOS!",
       AUTO_STRING_LENGTH,
                                   64, 55,
                                   OPAQUE_TEXT);
       _delay_cycles(50000000);
}
