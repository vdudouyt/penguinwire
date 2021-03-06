all: penguinwire.bin

%.bin: %.ihx
	objcopy -I ihex -O binary $< $@

penguinwire.ihx: main.rel w1.rel w1_search_devices.rel usb.rel
	sdcc --xram-size 0x0400 --xram-loc 0x0100 --code-size 0x3800 $^ -o $@

%.rel: %.c
	sdcc -c -mmcs51 --model-small --xram-size 0x0400 --xram-loc 0x0100 --code-size 0x3800 $< -o $@

clean:
	rm *.ihx *.rel *.bin

flash: all
	python3 -m ch55xtool -r -f penguinwire.bin
