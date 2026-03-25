#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <SysTick/SysTickHandlers.h>
#include "gpio.h"
#include "systick.h"
#include "hw_memmap.h"
#include "uart_if.h"

static volatile uint32_t lastEdge = 0;
static volatile uint32_t currentEdge = 0;
static volatile unsigned long duration = 0;

// Captures signals based off of the timing of falling edges
static void IRHandler(void)
{
    unsigned long ulStatus;

    ulStatus = GPIOIntStatus(GPIOA0_BASE, true);
    GPIOIntClear(GPIOA0_BASE, ulStatus);

    if (ulStatus & 0x1) {

        currentEdge = SysTickValueGet();

        if (lastEdge == 0) { //first edge
            lastEdge = currentEdge;
            return;
        }

        if (lastEdge >= currentEdge) {
            duration = (unsigned long)(lastEdge - currentEdge);
        } else {
            duration = (unsigned long)(lastEdge + (SYSTICK_RELOAD_MAX + 1) - currentEdge);
        }

        lastEdge = currentEdge;
    }
}

void SetupIR(void){
    GPIOIntRegister(GPIOA0_BASE, IRHandler);
    GPIOIntTypeSet(GPIOA0_BASE, 0x1, GPIO_FALLING_EDGE);

    unsigned long ulStatus = GPIOIntStatus(GPIOA0_BASE, false);
    GPIOIntClear(GPIOA0_BASE, ulStatus);
    GPIOIntEnable(GPIOA0_BASE, 0x1);
}

int numBitsRead = 0;
int bits[12];
int lastCommand = -1;

bool isHeldButton(int command){
    if(command == lastCommand){
        bool result = numResets < 3;
        numResets = 0;
        return result;
    }
    numResets = 0;
    return false;
}

// Main input handler - returns code of button pressed, but not if it's being held down
// If it returns negative, nothing's been pressed
int InputHandler(void) {
    if (duration != 0) {
        unsigned long temp = duration;
        duration = 0;

        unsigned int startGap = 0.020 * 80000000; //20ms
        if (temp > startGap) {
            numBitsRead = 0;
            return -1;
        }

        unsigned int oneGap = 0.0015 * 80000000; //1.5ms
        if (numBitsRead < 12) {
            if (temp > oneGap) {
                bits[numBitsRead] = 1;
            } else {
                bits[numBitsRead] = 0;
            }
            numBitsRead++;
        } else {
            numBitsRead = 0;
            return -1;
        }

        if (numBitsRead == 12) {
            int i;
            uint32_t num = 0;
            for (i = 0; i < 12; i++) {
                num = num + (bits[i] << i);
            }

            if(!isHeldButton(num)){
                lastCommand = num;
                return num;
            }
        }
    }
    return -1;
}
