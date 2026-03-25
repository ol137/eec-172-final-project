#ifndef SYSTICK_H_
#define SYSTICK_H_
#include <stdint.h>

#define SYSTICK_RELOAD_MAX 5000000
extern volatile uint32_t numResets;
extern volatile float numMs;

void SetupSysTick(void);
static void TickHandler(void);
#endif
