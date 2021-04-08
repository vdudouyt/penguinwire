#include <compiler.h>
#include "w1uart.h"
#include "lib/ch554.h"

SBIT(LED, 0x90, 1);

void Uart1_ISR() __interrupt (INT_NO_UART1);

void onW1Reset(uint8_t status) {
   LED = status > 0;
}

void main() {
   P1_MOD_OC = P1_MOD_OC & ~(1<<1);
   LED = 0;

   w1UartInit();
   EA = 1;

   w1UartReset(onW1Reset);
   
   while(1) { }
}
