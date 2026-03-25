#include "hw_memmap.h"
#include "gpio.h"
#include "utils.h"

void BuzzerBeep()
{
    int i;
    for(i = 0; i < 1000; i++){
        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x2);
        UtilsDelay(1000);

        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x0);
        UtilsDelay(1000);
    }

}

void BuzzerBeepGoDown(){
    int i;
    for(i = 0; i < 100; i++){
        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x2);
        UtilsDelay(10000);

        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x0);
        UtilsDelay(10000);
    }
}

void BuzzerBeepGoUp(){
    int i;
    for(i = 0; i < 1500; i++){
        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x2);
        UtilsDelay(660);

        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x0);
        UtilsDelay(660);
    }
}


void BuzzerBeepGood()
{
    int i;
    for(i = 0; i < 500; i++){
        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x2);
        UtilsDelay(5000);

        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x0);
        UtilsDelay(5000);
    }
}

void BuzzerBeepDone()
{
    int i;
    for(i = 0; i < 250; i++){
        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x2);
        UtilsDelay(6000);

        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x0);
        UtilsDelay(6000);
    }

    UtilsDelay(20000);

    for(i = 0; i < 500; i++){
        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x2);
        UtilsDelay(2500);

        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x0);
        UtilsDelay(2500);
    }

    UtilsDelay(20000);

    for(i = 0; i < 1000; i++){
        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x2);
        UtilsDelay(1250);

        GPIOPinWrite(GPIOA2_BASE, 0x2, 0x0);
        UtilsDelay(1250);
    }
}
