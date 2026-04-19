# ethanol-module

Module to read the ethanol percentage of a standard automotive 50hz to 150hz ethanol content sensor

## Description

The goal of this project was to build an affordable way to read ethanol for cars being tuned on e85. The code is built with the purpose of being installed onto an Arduino based device that receives input from an OEM ethanol content sensor.

## Reference

This is a cleaned up example I put together to integrate with a Raspberry Pi and Adafruit Trinket. Credit for the original idea and code can be tied back to the user Dala on the [sr20 forums](https://www.sr20-forum.com/nismotronic/76787-flex-fuel-sensor-output.html) and referenced through other various forums from [NASIOC](https://forums.nasioc.com/forums/showthread.php?t=2810122) and [evolutionm](https://www.evolutionm.net/forums/e85-ethanol/734384-e85-gauge-w-output-under-100-a.html). The code was included in snippets across forums and were specific to displaying on an small LCD but this project returns data over [I2C SDA/SCL](https://learn.adafruit.com/introducing-pro-trinket/pinouts) to the Raspberry Pi.

## Usage

An example of interacting with the module using Python and the [smbus-cffi](https://github.com/bivab/smbus-cffi) library.

```python
import smbus
import time

I2C_ADDRESS = 0x04
ETHANOL_DATA_IDENTIFIER = 0x01
DEVICE_DELAY = 0.1

bus = smbus.SMBus(1)

bus.write_byte_data(I2C_ADDRESS,0x00,ETHANOL_DATA_IDENTIFIER)
time.sleep(DEVICE_DELAY)
fuelContent = bus.read_byte(I2C_ADDRESS)
```

The full picture of how this can be implemented and pinned.

![pin out diagram](e85_diagram.png)

Pin out pictures pulled from <https://pinout.xyz> and <https://learn.adafruit.com/introducing-pro-trinket/pinouts>

An editable schematic (KiCad 7+) is in [`diagram/ethanolModule.kicad_sch`](diagram/ethanolModule.kicad_sch).

## System diagram

```mermaid
graph LR
  S["Ethanol Sensor<br/>50–150 Hz"] -->|"Pin 8 / ICP1"| A["Arduino Pro Trinket<br/>ATmega328 @ 8 MHz"]
  A -->|"I2C 0x04<br/>SDA / SCL"| R["Raspberry Pi"]
  R -->|"smbus read"| P["Python Consumer"]
```

## Pinout

| Signal | Arduino Pin | Raspberry Pi | Notes |
|---|---|---|---|
| Sensor signal | 8 (ICP1) | — | 50–150 Hz square wave from flex-fuel sensor |
| I2C SDA | SDA (A4) | GPIO 2 (Pin 3) | |
| I2C SCL | SCL (A5) | GPIO 3 (Pin 5) | |
| GND | GND | GND (Pin 6) | Common ground required |
| 5V | BAT/5V | 5V (Pin 2) | Power Arduino from Pi or shared supply |

## Build

Uses [PlatformIO](https://platformio.org/). Install via `pip install platformio`.

```sh
# Compile for Pro Trinket 5V
pio run -e pro_trinket_5v

# Run unit tests (host, no hardware needed)
pio test -e native
```
