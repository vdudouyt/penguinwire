#include "lib/usb_structs.h"

void onEp0VendorSpecificRequest(__xdata USBSetupRequest *setupReq);
void onEp1TransferDone();
void onEp2TransferDone();

extern __xdata __at (0x0040) uint8_t  Ep1Buffer[0x40];
extern __xdata __at (0x0080) uint8_t  Ep2Buffer[0x40];
extern __xdata __at (0x00C0) uint8_t  Ep3Buffer[0x40];

#define SET_TX_NAK(ctrl_reg) (ctrl_reg) = (ctrl_reg) & ~ MASK_UEP_T_RES | UEP_T_RES_NAK
#define SET_TX_ACK(ctrl_reg) (ctrl_reg) = (ctrl_reg) & ~ MASK_UEP_T_RES | UEP_T_RES_ACK

#define SET_RX_NAK(ctrl_reg) (ctrl_reg) = (ctrl_reg) & ~ MASK_UEP_R_RES | UEP_R_RES_NAK
#define SET_RX_ACK(ctrl_reg) (ctrl_reg) = (ctrl_reg) & ~ MASK_UEP_R_RES | UEP_R_RES_ACK

void USBInit();
