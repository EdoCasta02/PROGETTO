
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <math.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdlib.h>
#include <stdint.h>
#include "HwInit.h"
// Configurazione dei pulsanti S1 e S2
#define BUTTON_S1_PORT   GPIO_PORT_P5
#define BUTTON_S1_PIN    GPIO_PIN1

#define BUTTON_S2_PORT   GPIO_PORT_P1
#define BUTTON_S2_PIN    GPIO_PIN1

//-------------------------------AUDIO-------------------------------
/* Timer_A Compare Configuration Parameter  (PWM) */
Timer_A_CompareModeConfig compareConfig_PWM = {
TIMER_A_CAPTURECOMPARE_REGISTER_4,          // Use CCR3
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        10000                                        // Duty Cycle initially
        };

/* Timer_A Up Configuration Parameter */

const Timer_A_UpModeConfig upConfig = {
TIMER_A_CLOCKSOURCE_SMCLK,        // Cambia la sorgente del clock, se necessario
        TIMER_A_CLOCKSOURCE_DIVIDER_1, // Modifica il divisore del clock per aumentare la frequenza
        20000, // Modifica il periodo del timer per ottenere la frequenza desiderata
        TIMER_A_TAIE_INTERRUPT_DISABLE,   // Disabilita l'interruzione del timer
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disabilita l'interruzione del timer basata su CCR0
        TIMER_A_DO_CLEAR                    // Cancella il valore
        };


void _graphicsInit()
{
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                           &g_sCrystalfontz128x128_funcs);


}
void _configureButtons()
{
    // Configura i pulsanti S1 e S2
    GPIO_setAsInputPinWithPullUpResistor(BUTTON_S1_PORT, BUTTON_S1_PIN);
    GPIO_enableInterrupt(BUTTON_S1_PORT, BUTTON_S1_PIN);

    GPIO_setAsInputPinWithPullUpResistor(BUTTON_S2_PORT, BUTTON_S2_PIN);
    GPIO_enableInterrupt(BUTTON_S2_PORT, BUTTON_S2_PIN);

    // Configura gli interrupt per S1 e S2 (opzionale)
    Interrupt_enableInterrupt(INT_PORT5);
    Interrupt_enableInterrupt(INT_PORT1);

    // Imposta gli stati iniziali dei pulsanti come non premuti
     GPIO_setOutputHighOnPin(BUTTON_S1_PORT, BUTTON_S1_PIN);
     GPIO_setOutputHighOnPin(BUTTON_S2_PORT, BUTTON_S2_PIN);
}

void _buzzerInit()
{
    /* Configures P2.7 to PM_TA0.4 for using Timer PWM to control the buzzer */
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7,
    GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring Timer_A0 for Up Mode and starting */
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Initialize compare registers to generate PWM */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM); // For P2.7
    // Azzera il duty cycle per fermare il suono
    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_4,
                            0);
}




void _ledInit()
{

    // Set P1.0 to output direction
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
}

void _accelSensorInit()
{
    // Pin 4.0, 4.2, 6.1 as ADC input
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN0 | GPIO_PIN2,
                                               GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                               GPIO_PIN1,
                                               GPIO_TERTIARY_MODULE_FUNCTION);
}

void _joystickInit()
{
    // Pin 6.0, 4.4 as ADC input
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                               GPIO_PIN0,
                                               GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN4,
                                               GPIO_TERTIARY_MODULE_FUNCTION);
}


void _ADC_Interrupt_enable_init(){
    // Enabling the interrupt when a conversion on channel 2 (end of sequence)
    // is complete and enabling conversions
    ADC14_enableInterrupt(ADC_INT4);

    // Enabling Interrupts
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();
}



void _ADCInit()
{
    _accelSensorInit();
    _joystickInit();

    // Initializing ADC (ADCOSC/64/8)
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

    // Configuring ADC Memory (ADC_MEM0 - ADC_MEM4 with no repeat)
    // with internal 2.5v reference
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM4, true);

    // Accelerometer X - MEM0 - A14
    ADC14_configureConversionMemory(ADC_MEM0,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A14, ADC_NONDIFFERENTIAL_INPUTS);

    // Accelerometer Y - MEM1 - A13
    ADC14_configureConversionMemory(ADC_MEM1,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A13, ADC_NONDIFFERENTIAL_INPUTS);

    // Accelerometer Z - MEM2 - A11
    ADC14_configureConversionMemory(ADC_MEM2,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A11, ADC_NONDIFFERENTIAL_INPUTS);

    // Joystick X - MEM3 - A15
    ADC14_configureConversionMemory(ADC_MEM3,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A15, ADC_NONDIFFERENTIAL_INPUTS);

    // Joystick Y - MEM4 - A9
    ADC14_configureConversionMemory(ADC_MEM4,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);


    // Setting up the sample timer to automatically step through the sequence
    // convert
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    // Triggering the start of the sample
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();
}

void _hwInit()
{
    // Halting WDT and disabling master interrupts
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    // Set the core voltage level to VCORE1
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    // Set 2 flash wait states for Flash bank 0 and 1
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    // Initializes Clock System
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    _graphicsInit();
    _buzzerInit();
    _configureButtons();
    _ledInit();

    _ADCInit();
    _ADC_Interrupt_enable_init();

}


