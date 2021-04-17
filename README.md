# penguinwire
An USB-to-1Wire bridge firmware for WCH CH552 microcontroller

## Synopsis (Linux / BSD)
The 1-wire interface is gracefully exposed to the system kernel, so there's nothing special to know:
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
* Pull-up resistor is optional but highly recommended
* Two 0.1uF blocking capacitors between Vcc-GND and V33-GND as in CH552 datasheet
* USB_DM and USB_DP as in CH552 datasheet
* Optional status LED on P1.1
