#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <libusb-1.0/libusb.h>

#define CMD_RESET 0x01
#define CMD_BYTE_IO 0x02
#define CMD_SEARCH_ROM 0x03

void run_cmd(libusb_device_handle *dev, unsigned int op, uint8_t arg) {
   printf("run_cmd(%d, %02x)\n", op, arg);
   libusb_control_transfer(dev, 0x40, op, arg, 0, NULL, 0, 0);

   unsigned char c;
   int actual_length;
   libusb_bulk_transfer(dev, 0x81, &c, sizeof(c), &actual_length, 0);
   assert(actual_length);
   printf("Response: %02x\n", c);
}

void readROM(libusb_device_handle *dev) {
   run_cmd(dev, CMD_RESET, 0);
   run_cmd(dev, CMD_BYTE_IO, 0x33);

   int i;
   for(i = 0; i < 8; i++) {
      run_cmd(dev, CMD_BYTE_IO, 0xff);
   }
}

void readTemp(libusb_device_handle *dev) {
   run_cmd(dev, CMD_RESET, 0);
   run_cmd(dev, CMD_BYTE_IO, 0xCC); // Skip ROM
   run_cmd(dev, CMD_BYTE_IO, 0x44); // Start conversion
   sleep(1); // Wait
   run_cmd(dev, CMD_RESET, 0);
   run_cmd(dev, CMD_BYTE_IO, 0xCC); // Skip ROM
   printf("*** Read scratchpad ***\n");
   run_cmd(dev, CMD_BYTE_IO, 0xBE); // Read scratchpad

   int i;
   for(i = 0; i < 9; i++) {
      run_cmd(dev, CMD_BYTE_IO, 0xff);
   }
}

void searchDevices(libusb_device_handle *dev) {
   libusb_control_transfer(dev, 0x40, CMD_SEARCH_ROM, 0, 0, NULL, 0, 0);

   unsigned char buf[64];
   int actual_length;
   libusb_bulk_transfer(dev, 0x81, buf, sizeof(buf), &actual_length, 0);
   assert(actual_length);

   printf("Got device: ");

   int i;
   for(i = 0; i < 64; i++) {
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

   searchDevices(dev);

   libusb_close(dev);
}
