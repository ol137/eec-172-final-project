#include <stdint.h>
#include "../IR/IR.h"
#include "systick.h"
#include "hw_nvic.h"
#include "hw_types.h"
#include "SysTickHandlers.h"
#include "uart_if.h"

volatile uint32_t numResets = 0;
volatile float numMs = 0;

void SetupSysTick(void){
    SysTickDisable();
    SysTickPeriodSet(SYSTICK_RELOAD_MAX + 1);
    HWREG(NVIC_ST_CURRENT) = 0;// force reload
    SysTickEnable();
    SysTickIntRegister(&TickHandler);
    SysTickIntEnable();
}

static void TickHandler(void){
    numResets++;
    numMs += 62.5;

    // Reset if a long time passes
    if(numResets > 100){
        numResets = 0;
        lastEdge = 0;
    }
}
