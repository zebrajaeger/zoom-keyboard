# Zoom Keyboard

This is a simple BLE Keyboard for [Zoom](https://zoom.us/) meetings.

The default behavior is Alt+A to toggle muting.
Press SPACE to unmute, release SPACE to mute.

## Motivation 

Bluetooth Headphones gives us a radius around our computer to listen and talk, but no way to mute/unmute remotely.

Yes, there are wireless keyboards but...

## Solution

ESP32 + BLE + Touch sensors + Battery = Simple 2-key remote keyboard, dedicated to this meetings.

## Schematic

SPACE-Wire to 'Touch 3' alias 'GPIO15'
ALT+A-Wire to 'Touch 0' alias 'GPIO4'

On ESP32 devkit v1.0 are these pin 3 and 5 (USB directs left; pin 1 bottom left) 