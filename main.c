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
bool onW1SearchDevice(__xdata w1SearchCtx *ctx);

int wIdx;

void onEp0VendorSpecificRequest(__xdata USBSetupRequest *req) {
   if(req->bRequest == 1) {
      UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK; // Busy
      w1Reset(onW1RecvByte);
   } else if(req->bRequest == 2) {
      UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK; // Busy
      w1Write(req->wValue, onW1RecvByte);
   } else if(req->bRequest == 3) {
      UEP3_CTRL = UEP3_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK; // Busy
      wIdx = 0;
      w1SearchDevices(onW1SearchDevice);
   } else if(req->bRequest == 4) {
      __xdata uint8_t *buf = getBuf();
      int i;
      for(i = 0; i < 64; i++) {
         Ep1Buffer[i] = buf[i];
      }
      UEP1_T_LEN = 64;
   } else if(req->bRequest == 5) {
      nextBranch();
   }
}

void onW1RecvByte(uint8_t gotByte) {
   Ep1Buffer[0] = gotByte;
   UEP1_T_LEN = 1;
   UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;
}

bool onW1SearchDevice(__xdata w1SearchCtx *ctx) {
   memcpy(&Ep3Buffer[wIdx * 8], ctx->romID, 8);
   wIdx++;

   if(ctx->done || wIdx >= 8) {
      UEP3_T_LEN = wIdx * 8;
      UEP3_CTRL = UEP3_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;
      return false;
   }

   return true; // Read more
}

void main() {
   P1_MOD_OC = P1_MOD_OC & ~(1<<1);
   LED = 0;

   USBInit();
   w1Init();
   EA = 1;
   
   while(1) { }
}
