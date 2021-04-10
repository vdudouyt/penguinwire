#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef void (*w1ResetCallback) (uint8_t status);
typedef void (*w1WriteCallback) (uint8_t gotByte);
typedef bool (*w1FoundDeviceCallback) (uint8_t *romID, bool last);

void w1Init();
void w1Reset(w1ResetCallback cb);
void w1Write(uint8_t byte, w1WriteCallback cb);
void w1SearchDevices(w1FoundDeviceCallback cb);
