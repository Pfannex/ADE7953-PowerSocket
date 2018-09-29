#!/bin/sh

# https://github.com/espressif/esptool/wiki
esptool --chip esp8266 --port /dev/ttyUSB0  write_flash 0x0 update/firmware.bin 0x000bb000 update/spiffs.bin
