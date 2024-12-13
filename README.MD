**ST7796 driver for RPI zero**

*DISPLAY INFO:*

4.0' SPI Module ST7796

0x2C = data write command = CC
0x2A = column address set
0x2B = page address set

display pin 4 (RESET) from rpi pin 27 (LOW = reset)
display pin 5 (DC/RS) from rpi pin 25 (HIGH = data, LOW = command)

"CS_ACTIVE" = CS LOW, "CS_IDLE" = CS HIGH, gpio 8
