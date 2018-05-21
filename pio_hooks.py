#!/bin/python

Import("env")

def before_build_spiffs(source, target, env):
    env.Execute("node_modules/.bin/gulp buildfs")

env.AddPreAction("$BUILD_DIR/spiffs.bin", before_build_spiffs)
