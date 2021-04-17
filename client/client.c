#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <libusb-1.0/libusb.h>

void searchDevices(libusb_device_handle *dev) {
   libusb_control_transfer(dev, 0x40, 0x01, 0x49fd, 0, NULL, 0, 0);

   unsigned char buf[64];
   int actual_length;
   libusb_bulk_transfer(dev, 0x83, buf, sizeof(buf), &actual_length, 0);
   assert(actual_length);

   printf("Got devices: ");

   int i;
   for(i = 0; i < actual_length; i++) {
      if(i % 8 == 0) printf("\n");
      printf("%02x ", buf[i]);
   }

   printf("\n");
}

int main(int argc, char **argv) {
   int r;
   libusb_context *ctx = NULL;
   assert(libusb_init(&ctx) >= 0);

   libusb_device_handle *dev = libusb_open_device_with_vid_pid(ctx, 0x1d50, 0x5711);
   assert(dev);

   assert(libusb_claim_interface(dev, 0) == 0);
   assert(libusb_set_interface_alt_setting(dev, 0, 3) == 0);

   libusb_control_transfer(dev, 0x40, 0x01, 0x0042, 0, NULL, 0, 0); // Reset
   libusb_control_transfer(dev, 0x40, 0x01, 0x0052, 0xCC, NULL, 0, 0); // Skip ROM

   int actual_length;
   char buf[1];
   libusb_bulk_transfer(dev, 0x83, buf, sizeof(buf), &actual_length, 0);
   assert(actual_length);
}
