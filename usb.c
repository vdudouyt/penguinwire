#include <compiler.h>
#include <stdint.h>
#include <string.h>
#include "lib/ch554.h"
#include "lib/ch554_usb.h"
#include "lib/usb_structs.h"
#include "usb.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

__xdata __at (0x0000) uint8_t  Ep0Buffer[0x40];
__xdata __at (0x0040) uint8_t  Ep1Buffer[0x40];
__xdata __at (0x0080) uint8_t  Ep2Buffer[0x40];
__xdata __at (0x00C0) uint8_t  Ep3Buffer[0x40];

__code USBDeviceDescriptor devDesc = {
   .bLength = sizeof(devDesc),
   .bDescriptorType = USB_DESCR_TYP_DEVICE,
   .bcdUSB = 0x0110,
   .bDeviceClass = 0xFF,
   .bDeviceSubClass = 0x00,
   .bDeviceProtocol = 0x00,
   .bMaxPacketSize0 = 0x40,
   .idVendor = 0x1d50,
   .idProduct = 0x5711,
   .bcdDevice = 0x0100,
   .iManufacturer = 0x00,
   .iProduct = 0x00,
   .iSerialNumber = 0x00,
   .bNumConfigurations = 0x01
};

__code struct {
   USBConfigurationDecriptor cfg;
   USBInterfaceDescriptor ifDesc;
   USBEndpointDescriptor endpoints[3];
} cfgDesc = {
   .cfg = {
      .bLength = sizeof(cfgDesc.cfg),
      .bDescriptorType = USB_DESCR_TYP_CONFIG,
      .wTotalLength = sizeof(cfgDesc),
      .bNumInterfaces = 1,
      .bConfigurationValue = 1,
      .iConfiguration = 0,
      .bmAttributes = 0x80,
      .bMaxPower = 0x32,
   },
   .ifDesc = {
      .bLength = sizeof(cfgDesc.ifDesc),
      .bDescriptorType = USB_DESCR_TYP_INTERF,
      .bInterfaceNumber = 0x00,
      .bAlternateSetting = 0x03,
      .bNumEndpoints = 3,
      .bInterfaceClass = 0xff,
      .bInterfaceSubClass = 0xff,
      .bInterfaceProtocol = 0xff,
      .iInterface = 0x00,
   },
   .endpoints = {
      {
         .bLength = sizeof(cfgDesc.endpoints[0]),
         .bDescriptorType = USB_DESCR_TYP_ENDP,
         .bEndpointAddress = 0x81,
         .bmAttributes = 0x03, // interrupt
         .wMaxPacketSize = 0x40,
         .bInterval = 10,
      },
      {
         .bLength = sizeof(cfgDesc.endpoints[1]),
         .bDescriptorType = USB_DESCR_TYP_ENDP,
         .bEndpointAddress = 0x02,
         .bmAttributes = 0x02, // bulk
         .wMaxPacketSize = 0x40,
         .bInterval = 0,
      },
      {
         .bLength = sizeof(cfgDesc.endpoints[2]),
         .bDescriptorType = USB_DESCR_TYP_ENDP,
         .bEndpointAddress = 0x83,
         .bmAttributes = 0x02, // bulk
         .wMaxPacketSize = 0x40,
         .bInterval = 0,
      }
   },
};

void USBInit() {
   /* Device */
   USB_CTRL = 0x00;
   USB_CTRL |=  bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;
   USB_DEV_AD = 0x00;
   
   USB_CTRL &= ~bUC_LOW_SPEED;
   UDEV_CTRL &= ~bUD_LOW_SPEED;
   UDEV_CTRL = bUD_PD_DIS;
   UDEV_CTRL |= bUD_PORT_EN;

   /* Endpoints */

   UEP0_DMA = (uint16_t) Ep0Buffer;
   UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
   UEP0_T_LEN = 0;

   UEP1_DMA = (uint16_t) Ep1Buffer;
   UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_NAK | UEP_T_RES_ACK;
   UEP1_T_LEN = 0;
   UEP4_1_MOD |= bUEP1_TX_EN;

   UEP2_DMA = (uint16_t) Ep2Buffer;
   UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
   UEP2_T_LEN = 0;
   UEP2_3_MOD |= bUEP2_RX_EN;

   UEP3_DMA = (uint16_t) Ep3Buffer;
   UEP3_CTRL = bUEP_AUTO_TOG | UEP_R_RES_NAK | UEP_T_RES_ACK;
   UEP3_T_LEN = 0;
   UEP2_3_MOD |= bUEP3_TX_EN;

   /* Interrupts */
   USB_INT_EN |= bUIE_TRANSFER;
   USB_INT_FG |= 0x1F;
   IE_USB = 1;
}

void Ep0Handler();

void USBInterrupt() __interrupt (INT_NO_USB) {
   if(UIF_TRANSFER) {
      uint8_t ep = USB_INT_ST & MASK_UIS_ENDP;

      switch(ep) {
         case 0: Ep0Handler(); break;
         case 2: onEp2TransferDone(); break;
      }

      UIF_TRANSFER = 0;
   }
}

void Ep0Handler() {
   static int setAddressRequest = 0;
   unsigned char token = USB_INT_ST & MASK_UIS_TOKEN;

   if(token == UIS_TOKEN_SETUP && USB_RX_LEN == sizeof(USBSetupRequest)) {
      __xdata USBSetupRequest *setupReq = (USBSetupRequest*) Ep0Buffer;
      uint8_t bytesToTransmit = 0;

      if(setupReq->bRequestType == USB_REQ_TYP_VENDOR) {
         onEp0VendorSpecificRequest(setupReq);
      }

      if((setupReq->bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD) {
         if(setupReq->bRequest == USB_GET_DESCRIPTOR) {
            // Get descriptor
            switch(Ep0Buffer[3]) {
               case USB_DESCR_TYP_DEVICE:
                  bytesToTransmit = sizeof(devDesc);
                  memcpy(Ep0Buffer, &devDesc, bytesToTransmit);
                  break;
               case USB_DESCR_TYP_CONFIG:
                  bytesToTransmit = MIN(setupReq->wLength, sizeof(cfgDesc));
                  memcpy(Ep0Buffer, &cfgDesc, bytesToTransmit);
                  break;
            }
         } else if(setupReq->bRequest == USB_SET_ADDRESS) {
            setAddressRequest = setupReq->wValue;
         }

         UEP0_T_LEN = bytesToTransmit;
         UEP0_CTRL = bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
      }
   }

   if(token == UIS_TOKEN_IN) {
      if(setAddressRequest) {
         USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | setAddressRequest;
         UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
         setAddressRequest = 0;
      }
   }
}
