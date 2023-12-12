
#include "game.h"

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <math.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdlib.h>
#include <stdint.h>
#include "HwInit.h"

//----------------------------------------------------------------

/* Graphic library context */
Graphics_Context g_sContext;

/* ADC results buffer */
static uint16_t resultsBuffer[2];

void drawMove(int moveNumber)
{
    Graphics_clearDisplay(&g_sContext);

    char moveString[10]; // Considerando il massimo numero di cifre (5) più il terminatore di stringa
    sprintf(moveString, "Muovi %d", moveNumber);

    Graphics_drawStringCentered(&g_sContext, (int8_t*) moveString,
    AUTO_STRING_LENGTH,
                                64, 30, OPAQUE_TEXT);
    _delay_cycles(30000000);
    Graphics_clearDisplay(&g_sContext);
}

void drawSequence(char c)
{
    Graphics_clearDisplay(&g_sContext);
    char charArray[2] = { c, '\0' }; // Crea un array di caratteri con il carattere e il terminatore di stringa
    Graphics_drawStringCentered(&g_sContext, (int8_t*) charArray,
    AUTO_STRING_LENGTH,
                                64, 30, OPAQUE_TEXT);
    _delay_cycles(50000000);
    Graphics_clearDisplay(&g_sContext);
}

void drawArrow(char c)
{
    int x = 64;
    int y = 62;
    int size = 20;
    Graphics_clearDisplay(&g_sContext);

    switch (c)
    {
    case 'U':
        // Disegna la freccia verso l'alto con una punta più definita
        Graphics_drawLine(&g_sContext, x, y, x, y - size);
        Graphics_drawLine(&g_sContext, x, y - size, x - size / 2,
                          y - size + size / 4);
        Graphics_drawLine(&g_sContext, x, y - size, x + size / 2,
                          y - size + size / 4);
        Graphics_drawLine(&g_sContext, x - 2, y - size + size / 4, x + 2,
                          y - size + size / 4);
        break;

    case 'D':
        // Disegna la freccia verso il basso con una punta più definita
        Graphics_drawLine(&g_sContext, x, y, x, y + size);
        Graphics_drawLine(&g_sContext, x, y + size, x - size / 2,
                          y + size - size / 4);
        Graphics_drawLine(&g_sContext, x, y + size, x + size / 2,
                          y + size - size / 4);
        Graphics_drawLine(&g_sContext, x - 2, y + size - size / 4, x + 2,
                          y + size - size / 4);
        break;

    case 'R':
        // Disegna la freccia verso destra con una punta più definita
        Graphics_drawLine(&g_sContext, x, y, x + size, y);
        Graphics_drawLine(&g_sContext, x + size, y, x + size - size / 4,
                          y - size / 2);
        Graphics_drawLine(&g_sContext, x + size, y, x + size - size / 4,
                          y + size / 2);
        Graphics_drawLine(&g_sContext, x + size - size / 4, y - 2,
                          x + size - size / 4, y + 2);
        break;

    case 'L':
        // Disegna la freccia verso sinistra con una punta più definita
        Graphics_drawLine(&g_sContext, x, y, x - size, y);
        Graphics_drawLine(&g_sContext, x - size, y, x - size + size / 4,
                          y - size / 2);
        Graphics_drawLine(&g_sContext, x - size, y, x - size + size / 4,
                          y + size / 2);
        Graphics_drawLine(&g_sContext, x - size + size / 4, y - 2,
                          x - size + size / 4, y + 2);
        break;
    }

    _delay_cycles(30000000);

}

char generaDirezioneCasuale()
{
    int direzione = rand() % 4;

    switch (direzione)
    {
    case 0:
        return 'U'; // Su
    case 1:
        return 'D'; // Giù
    case 2:
        return 'R'; // Destra
    case 3:
        return 'L'; // Sinistra
    default:
        return 'C'; // Default a Centro in caso di errore
    }
}
void playGame()
{
    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, (int8_t*) "RICORDA:",
    AUTO_STRING_LENGTH,
                                64, 30,
                                OPAQUE_TEXT);

    // Genera una sequenza casuale di 5 lettere (U, D, R, L)
    _delay_cycles(50000000); //circa 3 secondi
    char sequence[5];
    int i = 0;
    for (i = 0; i < 5; i++)
    {
        sequence[i] = generaDirezioneCasuale(); // Assumi che esista una funzione per generare direzioni casuali
        drawArrow(sequence[i]);
        _delay_cycles(3000000); // Ritardo tra le direzioni (adattare alle esigenze)
    }
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, (int8_t*) "YOUR TURN:",
    AUTO_STRING_LENGTH,
                                64, 30,
                                OPAQUE_TEXT);
    _delay_cycles(10000000);

    for (i = 0; i < 5; i++)
    {
        drawMove(i + 1);
        resultsBuffer[0] = ADC14_getResult(ADC_MEM3);
        resultsBuffer[1] = ADC14_getResult(ADC_MEM4);
        char direzione = 'C';
        if (resultsBuffer[0] < 14500 && resultsBuffer[0] > 4000
                && resultsBuffer[1] > 15000)
        {
            direzione = 'U';
        }
        if (resultsBuffer[0] < 14500 && resultsBuffer[0] > 4000
                && resultsBuffer[1] < 1200)
        {
            direzione = 'D';
        }
        if (resultsBuffer[0] > 13500 && resultsBuffer[1] > 3500
                && resultsBuffer[1] < 13000)
        {
            direzione = 'R';
        }
        if (resultsBuffer[0] < 3000 && resultsBuffer[1] > 3500
                && resultsBuffer[1] < 13000)
        {
            direzione = 'L';
        }

        if (direzione != sequence[i])
        {
            Graphics_clearDisplay(&g_sContext);

            Graphics_drawStringCentered(&g_sContext, (int8_t*) "ERRATO!",
            AUTO_STRING_LENGTH,
                                        64, 30,
                                        OPAQUE_TEXT);
            break;
        }
        if (i == 4)
        {
            Graphics_clearDisplay(&g_sContext);

            Graphics_drawStringCentered(&g_sContext, (int8_t*) "CORRETTO!",
            AUTO_STRING_LENGTH,
                                        64, 30,
                                        OPAQUE_TEXT);
        }
    }

}
