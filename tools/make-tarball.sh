#!/bin/sh

PIO=$HOME/.platformio/penv/bin/pio

$PIO run -t clean && \
    $PIO run && \
    $PIO run -t buildfs && \
    gulp tarball
