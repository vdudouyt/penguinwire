# penguinwire
An USB-to-1Wire bridge firmware for WCH CH552 microcontroller

## Synopsis (Linux / BSD)
The 1-wire interface is ultimately exposed to the kernel, so there's nothing special to know:
```nohiglight
$ ls -l /sys/devices/w1_bus_master1/
$ cat /sys/devices/w1_bus_master1/28-00000bc02623/w1_slave 
8d 01 4b 46 7f ff 03 10 03 : crc=03 YES
8d 01 4b 46 7f ff 03 10 03 t=24812 ### 24°C
```

## Building
```nohighlight
$ apt-get install sdcc python3-pip
$ python3 -mpip install ch55xtool
$ make && make flash
```

## Wiring
* P1.6 and P1.7 should be short-circuited and connected to DQ
* Optional 1..4K pull-up resistor on DQ
* UART1 is configured in 8051 quasi-bidirectional mode, so a blocking diode is not required
* Two 0.1uF blocking capacitors between Vcc-GND and V33-GND as in CH552 datasheet
* Optional status LED on P1.1
* If you need to reprogram the device, connect a 10k resistor between USB_DP and V33 while powering up
