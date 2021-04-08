#include "w1uart.h"
#include "lib/ch554.h"

SBIT(LED, 0x90, 1);

enum { W1_NONE = 0, W1_RESET, W1_WRITE, W1_SEARCH_DEVICES } pendingOp;

__xdata uint8_t sendByteBuf[8], recvByteBuf[8];
uint8_t sendCtr, recvCtr;

__xdata struct {
   w1ResetCallback resetCb;
   w1WriteCallback writeCb;
   w1FoundDeviceCallback searchCb;
} pendingCallbacks;

static uint8_t decodeByte(uint8_t *byteBuf);

void w1UartInit() {
   U1SM0 = 0;
   U1SMOD = 1;
   U1REN = 1;
   SBAUD1 = 217; // 9600 baud
   IE_UART1 = 1;

   pendingOp = W1_NONE;
}

void w1UartReset(w1ResetCallback cb) {
   pendingCallbacks.resetCb = cb;
   pendingOp = W1_RESET;

   SBAUD1 = 217; // 9600 baud
   SBUF1 = 0xf0;
}

void w1UartWrite(uint8_t byte, w1WriteCallback cb) {
   uint8_t i = 0;

   for(i = 0; i < 8; i++) {
      sendByteBuf[i] = (byte & 1) ? 0xff : 0x00;
      byte >>= 1;
   }

   pendingCallbacks.writeCb = cb;
   pendingOp = W1_WRITE;

   recvCtr = sendCtr = 0;
   SBAUD1 = 253; // 115200 baud
   SBUF1 = sendByteBuf[0];
   sendCtr++;
}

void Uart1_ISR() __interrupt (INT_NO_UART1) {
   if(U1RI) {
      uint8_t sbuf = SBUF1;

      switch(pendingOp) {
         case W1_RESET:
            pendingCallbacks.resetCb(sbuf);
            pendingOp = W1_NONE;
            break;
         case W1_WRITE:
            recvByteBuf[recvCtr] = sbuf;
            recvCtr++;

            if(sendCtr >= 8) {
               pendingCallbacks.writeCb(decodeByte(recvByteBuf));
               pendingOp = W1_NONE;
            } else {
               SBUF1 = sendByteBuf[sendCtr];
               sendCtr++;
            }
            break;
      }

      U1RI = 0;
   }
   if(U1TI) {
      U1TI = 0;
   }
}

uint8_t decodeByte(uint8_t *byteBuf) {
   uint8_t byte = 0;
   int i;
   for(i = 7; i >= 0; i--) {
      byte |= byteBuf[i] == 0xff;
      if(i > 0) byte <<= 1;
   }
   return byte;
}
