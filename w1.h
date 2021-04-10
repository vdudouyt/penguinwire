#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef void (*w1ResetCallback) (uint8_t status);
typedef void (*w1WriteCallback) (uint8_t gotByte);

void w1Init();
void w1Reset(w1ResetCallback cb);
void w1Write(uint8_t byte, w1WriteCallback cb);
void w1WriteBit(uint8_t bit, w1WriteCallback cb);
