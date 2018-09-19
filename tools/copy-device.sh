#!/bin/bash

INOUT=$1
DEVICE=$2

ROOT="`dirname $0`/.."
CD="$ROOT/_customDevices"

usage() {
  >&2 echo "Usage: `basename $0` in|out devicename"
}

if ([ "$INOUT" != "in" ] && [ "$INOUT" != "out" ]) || [ -z "$DEVICE" ]; then
  usage
  exit 1
fi


CDD="$CD/$DEVICE"
echo "copy $DEVICE $INOUT, press enter to continue or break to abort"
read

if [ "$INOUT" == "in" ]; then
  if [ ! -d "$CD/$DEVICE" ]; then
    >&2 echo "No folder for devicename $DEVICE"
    exit 2
  fi
  FROM=$CDD
  TO=$ROOT
else
  FROM=$ROOT
  TO=$CDD
fi

echo "Copying from $FROM to $TO"
mkdir -pv $TO/src/customDevice/ $TO/data/customDevice/ && \
cp -av $FROM/src/customDevice/* $TO/src/customDevice/ && \
cp -av $FROM/data/customDevice/* $TO/data/customDevice/
