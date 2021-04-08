#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <libusb-1.0/libusb.h>

int main(int argc, char **argv) {
   int r;
   libusb_context *ctx = NULL;
   assert(libusb_init(&ctx) >= 0);

   libusb_device_handle *dev = libusb_open_device_with_vid_pid(ctx, 0x1d50, 0x5711);
   assert(dev);

   printf("Sending\n");
   libusb_control_transfer(dev, 0x40, 1, 0, 0, NULL, 0, 0);

   printf("Reading\n");

   unsigned char buf[64];
   int actual_length;
   libusb_bulk_transfer(dev, 0x81, buf, sizeof(buf), &actual_length, 0);
   assert(actual_length);
   
   printf("Got bytes:\n");
   int i = 0;
   for(i = 0; i < actual_length; i++) {
      printf("%02x ", buf[i]);
   }

   printf("\n");

   libusb_close(dev);
}
