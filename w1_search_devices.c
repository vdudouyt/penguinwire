#include "w1.h"
#include "w1_search_devices.h"
#include <stdlib.h>
#include <string.h>
#include <compiler.h>

SBIT(LED, 0x90, 1);

uint8_t rIdx = 0;
__xdata uint8_t buf[64];
__xdata w1SearchDeviceCallback searchDeviceCallback;

__xdata uint8_t *getBuf() {
   return buf;
}

#define BRANCH_FLAG 0x02

static void onW1Reset(uint8_t gotByte);
static void onSearchCmdSent(uint8_t gotByte);

static void onReadBit(uint8_t gotByte);
static void onReadBitInv(uint8_t gotByte);
static void onWriteBit(uint8_t gotByte);

static uint8_t decodeByte(__xdata uint8_t *byteBuf);

void w1InitBuf() {
   memset(buf, 0, sizeof(buf));
}

void w1SearchDevices(w1SearchDeviceCallback cb) {
   searchDeviceCallback = cb;
   w1Reset(onW1Reset);
}

void onW1Reset(uint8_t gotByte) {
   __xdata w1SearchCtx ctx;
   memset(&ctx, 0, sizeof(ctx));

   switch(gotByte) {
      case 0xf0: ctx.status = W1SEARCH_DONE; break; /* No devices on line */
      case 0x00: ctx.status = W1SEARCH_RESET_FAILED; break; /* Something is holding the line */
   }

   if(ctx.status != 0) {
      searchDeviceCallback(&ctx);
      return;
   }

   w1Write(0xF0, onSearchCmdSent);
}

void onSearchCmdSent(uint8_t gotByte) {
   (void)(gotByte);

   /* Starting 2-read 1-write 64 bit search sequence described in DS manuals */
   rIdx = 0;
   w1WriteBit(1, onReadBit);
}

static int b = 0, bInv = 0;

void onReadBit(uint8_t gotByte) {
   b = gotByte == 0xFF;
   w1WriteBit(1, onReadBitInv);
}

void onReadBitInv(uint8_t gotByte) {
   __xdata w1SearchCtx ctx;

   bInv = gotByte == 0xFF;

   if(b == 1 && bInv == 1) {
      /* No devices responded */
      ctx.status = W1SEARCH_ERR;
      searchDeviceCallback(&ctx);
      return;
   } else if(b == 0 && bInv == 0) {
      /* Two or more devices are holding the line simultaneously, make a branch here */
      /* As you probably mentioned, an existing value in buf[] is getting preserved */
      buf[rIdx] |= BRANCH_FLAG;
      b = buf[rIdx];
   } else {
      buf[rIdx] = b;
   }

   rIdx++;

   if(rIdx < 64) {
      return w1WriteBit(b & 0x01, onWriteBit);
   } else {
      __xdata uint8_t romID[8];

      uint8_t i;
      for(i = 0; i < 8; i++) {
         romID[i] = decodeByte(&buf[i * 8]);
      }

      ctx.status = W1SEARCH_DEVICE_FOUND;
      ctx.romID = romID;
      searchDeviceCallback(&ctx);
   }
}

void onWriteBit(uint8_t gotByte) {
   (void)(gotByte);
   w1WriteBit(1, onReadBit);
}

uint8_t decodeByte(__xdata uint8_t *byteBuf) {
   uint8_t byte = 0;
   int i;
   for(i = 7; i >= 0; i--) {
      byte |= byteBuf[i] & 0x01;
      if(i > 0) byte <<= 1;
   }
   return byte;
}

int nextBranch() {
   int i;
   for(i = 63; i >= 0; i--) {
      if(buf[i] == 2) {
         buf[i] = 0x01;   // flip bit
         return true;      // there's one more branch to visit
      } else {
         buf[i] = 0;
      }
   }
   return false;
}
