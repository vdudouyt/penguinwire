all: penguinwire.bin

%.bin: %.ihx
	objcopy -I ihex -O binary $< $@

penguinwire.ihx: main.rel w1uart.rel
	sdcc $^ -o $@

%.rel: %.c
	sdcc -c -mmcs51 --model-small --xram-size 0x0400 --xram-loc 0x0000 --code-size 0x3800 $< -o $@

clean:
	rm *.ihx *.rel *.bin

flash: all
	count_times python3 -m ch55xtool -r -f penguinwire.bin
