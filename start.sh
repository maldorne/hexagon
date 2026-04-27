#!/bin/sh
#
# hexagon mudlib startup script.
#
# Expects the DGD driver at /opt/mud/bin/dgd and the config file at
# /opt/mud/bin/config.dgd. Runs the driver in the foreground so the
# supervising process (docker, systemd, etc.) sees stdout/stderr directly.

set -e

DRIVER=/opt/mud/bin/dgd
CONFIG=/opt/mud/bin/config.dgd

if [ ! -x "$DRIVER" ]; then
  echo "ERROR: dgd driver not found or not executable at $DRIVER" >&2
  exit 1
fi
if [ ! -f "$CONFIG" ]; then
  echo "ERROR: config not found at $CONFIG" >&2
  exit 1
fi

echo "Starting hexagon ..."
exec "$DRIVER" "$CONFIG"
