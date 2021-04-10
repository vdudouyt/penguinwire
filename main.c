#include <compiler.h>
#include <string.h>
#include "w1.h"
#include "w1_search_devices.h"
#include "lib/ch554.h"
#include "usb.h"

SBIT(LED, 0x90, 1);

void Uart1_ISR() __interrupt (INT_NO_UART1);
void USBInterrupt() __interrupt (INT_NO_USB);

void onW1RecvByte(uint8_t status);
bool onW1SearchDevice(w1SearchCtx *ctx);

void onEp0VendorSpecificRequest(USBSetupRequest *req) {
   if(req->bRequest == 1) {
      UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK; // Busy
      w1Reset(onW1RecvByte);
   } else if(req->bRequest == 2) {
      UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK; // Busy
      w1Write(req->wValue, onW1RecvByte);
   } else if(req->bRequest == 3) {
      UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK; // Busy
      w1SearchDevices(onW1SearchDevice);
   }
}

void onW1RecvByte(uint8_t gotByte) {
   Ep1Buffer[0] = gotByte;
   UEP1_T_LEN = 1;
   UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;
}

bool onW1SearchDevice(w1SearchCtx *ctx) {
   memcpy(Ep1Buffer, ctx->romID, 64);
   UEP1_T_LEN = 64;
   UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;
   (void) (ctx);
   return false;
}

void main() {
   P1_MOD_OC = P1_MOD_OC & ~(1<<1);
   LED = 0;

   USBInit();
   w1Init();
   EA = 1;
   
   while(1) { }
}
