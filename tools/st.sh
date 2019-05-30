#!/bin/sh
#
# uses https://github.com/littleyoda/EspStackTraceDecoder
#
echo "Copy & paste stack trace here, then press Ctrl+D"
TRACE=/tmp/stacktrace-$$.txt
BUILD_DIR=../.pio/build/d1_mini
cat > $TRACE
java -jar EspStackTraceDecoder.jar ~/.platformio/packages/toolchain-xtensa/bin/xtensa-lx106-elf-addr2line $BUILD_DIR/firmware.elf $TRACE
rm $TRACE
