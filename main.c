#include <compiler.h>
#include "w1uart.h"
#include "lib/ch554.h"
#include "usb.h"

SBIT(LED, 0x90, 1);

void Uart1_ISR() __interrupt (INT_NO_UART1);
void USBInterrupt() __interrupt (INT_NO_USB);

void onW1Reset(uint8_t status) {
   LED = status > 0;
}

void onEp0VendorSpecificRequest(USBSetupRequest *req) {
   if(req->bRequest == 1) {
      Ep1Buffer[0]++;
      UEP1_T_LEN = 1;
   }
}

void main() {
   P1_MOD_OC = P1_MOD_OC & ~(1<<1);
   LED = 0;

   USBInit();
   w1UartInit();
   EA = 1;

   w1UartReset(onW1Reset);
   
   while(1) { }
}
