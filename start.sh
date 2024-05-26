#!/bin/sh

DRIVER=/opt/mud/bin/dgd
CONFIG=/opt/mud/bin/config.dgd

test -f $DRIVER || exit 0
test -f $CONFIG || exit 0

echo "Starting mud..."
$DRIVER $CONFIG 2> /dev/null 1>&2

echo "Mud online!"
