#include <stdint.h>
#include <stdlib.h>
#include "gpio.h"
#include "hw_memmap.h"
#include "prcm.h"
#include "rom_map.h"
#include "timer.h"
#include "uart_if.h"

#define FAILURE                 -1
#define SUCCESS                 0

void HCSR04_Init(void)
{

    PRCMPeripheralClkEnable(PRCM_TIMERA0, PRCM_RUN_MODE_CLK);
    PRCMPeripheralReset(PRCM_TIMERA0);


    TimerDisable(TIMERA0_BASE, TIMER_A);
    TimerConfigure(TIMERA0_BASE, TIMER_CFG_PERIODIC_UP);
    TimerLoadSet(TIMERA0_BASE, TIMER_A, 0xFFFFFFFF);
    TimerEnable(TIMERA0_BASE, TIMER_A);

    // Make sure TRIG starts low
    GPIOPinWrite(GPIOA1_BASE, 0x01, 0);
}

uint32_t HCSR04_GetTimerTicks(void)
{
    return TimerValueGet(TIMERA0_BASE, TIMER_A);
}

uint32_t HCSR04_ElapsedTicks(uint32_t start, uint32_t end)
{
    if (end >= start)
    {
        return (end - start);
    }
    else
    {
        return (0xFFFFFFFFUL - start + 1UL + end);
    }
}

void HCSR04_DelayUs(uint32_t us)
{
    uint32_t start = HCSR04_GetTimerTicks();
    uint32_t wait_ticks = us * 80UL;

    while (HCSR04_ElapsedTicks(start, HCSR04_GetTimerTicks()) < wait_ticks)
    {
    }
}

void HCSR04_Trigger(void)
{
    // Clean low before pulse
    GPIOPinWrite(GPIOA1_BASE, 0x01, 0);
    HCSR04_DelayUs(2);

    GPIOPinWrite(GPIOA1_BASE, 0x01, 0x01);
    HCSR04_DelayUs(10);

    GPIOPinWrite(GPIOA1_BASE, 0x01, 0);
}

int HCSR04_ReadEchoUs(uint32_t *pulse_us)
{
    uint32_t wait_start;
    uint32_t pulse_start;
    uint32_t pulse_end;
    uint32_t elapsed_ticks;

    if (pulse_us == NULL)
    {
        return FAILURE;
    }

    *pulse_us = 0;

    // If echo is stuck high, wait briefly for it to go low first
    wait_start = HCSR04_GetTimerTicks();
    while (GPIOPinRead(GPIOA0_BASE, 0x80))
    {
        if (HCSR04_ElapsedTicks(wait_start, HCSR04_GetTimerTicks()) >=
            (30000UL * 80UL))
        {
            return FAILURE;
        }
    }

    HCSR04_Trigger();

    // Wait for echo to go high
    wait_start = HCSR04_GetTimerTicks();
    while (!GPIOPinRead(GPIOA0_BASE, 0x80))
    {
        if (HCSR04_ElapsedTicks(wait_start, HCSR04_GetTimerTicks()) >=
            (30000UL * 80UL))
        {
            return FAILURE;
        }
    }

    pulse_start = HCSR04_GetTimerTicks();

    // Wait for echo to go low
    while (GPIOPinRead(GPIOA0_BASE, 0x80))
    {
        if (HCSR04_ElapsedTicks(pulse_start, HCSR04_GetTimerTicks()) >=
            (30000UL * 80UL))
        {
            return FAILURE;
        }
    }

    pulse_end = HCSR04_GetTimerTicks();

    elapsed_ticks = HCSR04_ElapsedTicks(pulse_start, pulse_end);

    // Convert timer ticks to microseconds
    *pulse_us = elapsed_ticks / 80UL;

    return SUCCESS;
}

int HCSR04_GetDistanceCm(uint32_t *distance_cm)
{
    uint32_t pulse_us;

    if (distance_cm == NULL)
    {
        return FAILURE;
    }

    *distance_cm = 0;

    if (HCSR04_ReadEchoUs(&pulse_us) != SUCCESS)
    {
        return FAILURE;
    }

    *distance_cm = pulse_us / 58U;

    return SUCCESS;
}
