#include <compiler.h>
#include <string.h>
#include "w1.h"
#include "w1_search_devices.h"
#include "lib/ch554.h"
#include "usb.h"
#include "ds2490.h"
#include "ds_status.h"

SBIT(LED, 0x90, 1);

void Uart1_ISR() __interrupt (INT_NO_UART1);
void USBInterrupt() __interrupt (INT_NO_USB);

void onW1Reset(uint8_t gotByte);
void onW1RecvByte(uint8_t gotByte);
bool onW1SearchDevice(__xdata w1SearchCtx *ctx);

unsigned int wIdx;
unsigned int bytesReceived = 0;

#define COMM_MASK 0xf6

static void SetDeviceBusy() {
   LED = 1;
   struct ds_status *status = (struct ds_status*) Ep1Buffer;
   status->status = status->data_in_buffer_status = 0;
   UEP0_CTRL = UEP_R_RES_NAK | UEP_T_RES_NAK;
}

static void SetDeviceReady() {
   LED = 0;
   struct ds_status *status = (struct ds_status*) Ep1Buffer;
   status->status = ST_IDLE | ST_HALT;
   status->data_in_buffer_status = 1;
   UEP0_CTRL = bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
}

void onEp2TransferDone() {
   bytesReceived = USB_RX_LEN;
}

void onEp0VendorSpecificRequest(__xdata USBSetupRequest *setupReq) {
   if(setupReq->bRequest != COMM_CMD) {
      return;
   }

   switch(setupReq->wValue & COMM_MASK) {
      case COMM_1_WIRE_RESET:
         SetDeviceBusy();
         w1Reset(onW1Reset);
         break;
      case COMM_BYTE_IO:
         SET_TX_NAK(UEP3_CTRL);
         SetDeviceBusy();
         w1Write(setupReq->wIndex, onW1RecvByte);
         break;
      case COMM_SEARCH_ACCESS:
         SET_TX_NAK(UEP3_CTRL);
         wIdx = 0;

         SetDeviceBusy();
         w1SearchDevices(onW1SearchDevice);
         break;
   }
}

void onW1Reset(uint8_t gotByte) {
   (void) (gotByte);
   SetDeviceReady();
}

void onW1RecvByte(uint8_t gotByte) {
   Ep3Buffer[0] = gotByte;
   UEP3_T_LEN = 1;
   SetDeviceReady();
   SET_TX_ACK(UEP3_CTRL);
}

bool onW1SearchDevice(__xdata w1SearchCtx *ctx) {
   memcpy(&Ep3Buffer[wIdx * 8], ctx->romID, 8);
   wIdx++;

   if(ctx->done || wIdx >= 8) {
      UEP3_T_LEN = wIdx * 8;
      SetDeviceReady();
      SET_TX_ACK(UEP3_CTRL);
      return false;
   }

   return true; // Read more
}

void main() {
   P1_MOD_OC = P1_MOD_OC & ~(1<<1);
   LED = 0;

   USBInit();
   w1Init();

   UEP1_T_LEN = sizeof(struct ds_status);
   memset(Ep1Buffer, 0, sizeof(UEP1_T_LEN));

   EA = 1;
   
   while(1) { }
}
