#include "lib/usb_structs.h"

void onEp0VendorSpecificRequest(USBSetupRequest *setupReq);
void onEp1TransferDone();

extern __xdata __at (0x0040) uint8_t  Ep1Buffer[0x40];

void USBInit();
