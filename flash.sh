#!/usr/bin/env bash
# Waits for a Lulu half in the atmel-dfu bootloader (03eb:2ff4), then flashes it.
# Run it with the keyboard still plugged in (it needs the sudo password first), then unplug USB
# and TRRS, hold Esc on the left half and plug that half in alone.
set -euo pipefail
hex=${1:?usage: flash.sh <path to boardsource_lulu_avr_twizLulu.hex from qmk compile>}
[ -f "$hex" ] || { echo "no hex at $hex" >&2; exit 1; }
dfu=$(nix build --no-link --print-out-paths nixpkgs#dfu-programmer)/bin/dfu-programmer
[ -x "$dfu" ] || { echo "dfu-programmer not built" >&2; exit 1; }

in_dfu() { grep -lqx 2ff4 $(grep -lx 03eb /sys/bus/usb/devices/*/idVendor 2>/dev/null | sed 's/idVendor$/idProduct/') /dev/null 2>/dev/null; }

sudo -v  # ask for the password now, not mid-flash
echo "flashing $(basename "$hex"). Waiting up to 5 min: unplug USB + TRRS, hold Esc, plug the left half in alone."
for ((i = 0; i < 600; i++)); do
  if in_dfu; then
    sudo "$dfu" atmega32u4 erase --force
    sudo "$dfu" atmega32u4 flash "$hex"
    sudo "$dfu" atmega32u4 launch
    echo "done: unplug, reconnect the TRRS cable, plug the left half back into USB."
    exit 0
  fi
  (( i % 60 )) || sudo -n -v  # keep the sudo ticket fresh; nobody can retype the password now
  sleep 0.5
done
echo "timed out: no 03eb:2ff4 device seen in 5 min" >&2
exit 1
