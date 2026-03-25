#include "hw_memmap.h"
#include "prcm.h"
#include "uart.h"

void UARTinit(void)
{
    PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);
    UARTConfigSetExpClk(UARTA0_BASE,80000000,115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable(UARTA0_BASE);
    UARTDMADisable(UARTA0_BASE, (UART_DMA_RX | UART_DMA_TX));

    UARTIntDisable(UARTA0_BASE, 0xFFFFFFFF);
}
