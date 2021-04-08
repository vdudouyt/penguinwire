#include <compiler.h>
#include "w1uart.h"
#include "lib/ch554.h"
#include "usb.h"

SBIT(LED, 0x90, 1);

void Uart1_ISR() __interrupt (INT_NO_UART1);
void USBInterrupt() __interrupt (INT_NO_USB);

void onW1RecvByte(uint8_t status);

void onEp0VendorSpecificRequest(USBSetupRequest *req) {
   if(req->bRequest == 1) {
      UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK; // Busy
      w1UartReset(onW1RecvByte);
   } else if(req->bRequest == 2) {
      UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK; // Busy
      w1UartWrite(req->wValue, onW1RecvByte);
   }
}

void onW1RecvByte(uint8_t gotByte) {
   Ep1Buffer[0] = gotByte;
   UEP1_T_LEN = 1;
   UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;
}

void main() {
   P1_MOD_OC = P1_MOD_OC & ~(1<<1);
   LED = 0;

   USBInit();
   w1UartInit();
   EA = 1;
   
   while(1) { }
}
