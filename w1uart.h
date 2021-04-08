#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef void (*w1ResetCallback) (uint8_t status);
typedef void (*w1WriteCallback) (uint8_t gotByte);
typedef bool (*w1FoundDeviceCallback) (uint8_t *romID, bool last);

void w1UartInit();
void w1UartReset(w1ResetCallback cb);
void w1UartWrite(uint8_t byte, w1WriteCallback cb);
void w1UartSearchDevices(w1FoundDeviceCallback cb);
