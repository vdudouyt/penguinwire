#pragma once

typedef struct {
   __xdata uint8_t *romID;
   bool done;
   enum { W1SEARCH_DEVICE_FOUND = 0, W1SEARCH_DONE, W1SEARCH_RESET_FAILED, W1SEARCH_ERR } status;
} w1SearchCtx;

typedef bool (*w1SearchDeviceCallback) (__xdata w1SearchCtx *ctx);
void w1SearchDevices(w1SearchDeviceCallback cb);
__xdata uint8_t *getBuf();
bool nextBranch();
