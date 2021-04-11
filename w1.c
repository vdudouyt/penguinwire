#include "w1.h"
#include "lib/ch554.h"

SBIT(LED, 0x90, 1);

enum { W1_NONE = 0, W1_RESET, W1_WRITE, W1_WRITE_BIT } pendingOp;

__xdata uint8_t sendByteBuf[8], recvByteBuf[8];
uint8_t sendCtr, recvCtr;

__xdata struct {
   w1ResetCallback resetCb;
   w1WriteCallback writeCb;
} pendingCallbacks;

static uint8_t decodeByte(__xdata uint8_t *byteBuf);

void w1Init() {
   U1SM0 = 0;
   U1SMOD = 1;
   U1REN = 1;
   SBAUD1 = 217; // 9600 baud
   IE_UART1 = 1;

   pendingOp = W1_NONE;
}

void w1Reset(w1ResetCallback cb) {
   pendingCallbacks.resetCb = cb;
   pendingOp = W1_RESET;

   SBAUD1 = 217; // 9600 baud
   SBUF1 = 0xf0;
}

void w1Write(uint8_t byte, w1WriteCallback cb) {
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

void w1WriteBit(uint8_t bit, w1WriteCallback cb) {
   pendingCallbacks.writeCb = cb;
   pendingOp = W1_WRITE_BIT;

   SBAUD1 = 253; // 115200 baud
   SBUF1 = bit ? 0xff : 0x00;
}

void Uart1_ISR() __interrupt (INT_NO_UART1) {
   if(U1RI) {
      uint8_t sbuf = SBUF1;

      switch(pendingOp) {
         case W1_RESET:
            pendingOp = W1_NONE;
            pendingCallbacks.resetCb(sbuf);
            break;
         case W1_WRITE:
            recvByteBuf[recvCtr] = sbuf;
            recvCtr++;

            if(sendCtr >= 8) {
               pendingOp = W1_NONE;
               pendingCallbacks.writeCb(decodeByte(recvByteBuf));
            } else {
               SBUF1 = sendByteBuf[sendCtr];
               sendCtr++;
            }
            break;
         case W1_WRITE_BIT:
            pendingOp = W1_NONE;
            pendingCallbacks.writeCb(sbuf);
            break;
      }

      U1RI = 0;
   }
   if(U1TI) {
      U1TI = 0;
   }
}

uint8_t decodeByte(__xdata uint8_t *byteBuf) {
   uint8_t byte = 0;
   int i;
   for(i = 7; i >= 0; i--) {
      byte |= byteBuf[i] == 0xff;
      if(i > 0) byte <<= 1;
   }
   return byte;
}
