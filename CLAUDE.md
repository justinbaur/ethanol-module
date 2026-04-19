# ethanol-module

Arduino sketch that reads a GM-style flex-fuel ethanol content sensor and exposes the reading over I2C.

## Hardware target

- **MCU**: Adafruit Pro Trinket 5V (ATmega328P @ 8 MHz, same silicon as Arduino Uno)
- **Sensor**: OEM GM flex-fuel sensor, outputs a 50–150 Hz square wave proportional to ethanol content
- **Host**: Raspberry Pi reads over I2C using the `smbus` Python library

## Sensor protocol

The sensor outputs a frequency signal on digital pin 8 (ICP1):
- 50 Hz = 0% ethanol (pure gasoline)
- 150 Hz = 100% ethanol (pure E100)
- Linear interpolation: `ethanol% = Hz - 50`

## Timer1 register breakdown

`setupTimer()` configures Timer/Counter1 for input capture mode:

| Register | Value | Meaning |
|---|---|---|
| `TCCR1B` | 132 = `0b10000100` | `ICNC1`=1 (noise cancel), `ICES1`=1 (rising edge), `CS12`=1 (prescaler /256) |
| `TIMSK1` | 33 = `0b00100001` | `ICIE1`=1 (input capture interrupt), `TOIE1`=1 (overflow interrupt) |

The prescaler divides the 8 MHz clock by 256, giving a timer tick of 32 µs. When the sensor signal's rising edge arrives, `ICR1` holds the tick count since the last edge, and the ISR copies it to `revTick`.

## Frequency calculation

```
HZ = 62200 / revTick
```

The theoretical constant is `F_CPU / prescaler = 8,000,000 / 256 = 31,250`. The 62200 figure comes from the original forum-sourced code and targets a 16 MHz Arduino Uno (16,000,000 / 256 = 62,500), calibrated slightly down to 62200 empirically. If porting to a true 8 MHz board, this constant should be revisited.

The overflow ISR sets `revTick = 0` when no edge arrives within one full timer period (~2.1 s at 8 MHz / 256), which `loop()` interprets as no signal → 0% ethanol.

## I2C protocol

The Arduino is an I2C slave at address `0x04`. The Raspberry Pi:
1. Writes a single identifier byte (`0x01` = `FUEL_CONTENT_NUMBER`) to request ethanol data
2. Reads a single byte response containing the ethanol percentage (0–100)

The `receiveData` ISR stores the pending response; `sendData` ISR transmits it.

## Atomicity note

`revTick` is a `volatile uint16_t` written by the Timer1 ISR. On the 8-bit ATmega, 16-bit reads are two separate 8-bit operations — non-atomic. `loop()` wraps the read in `ATOMIC_BLOCK(ATOMIC_RESTORESTATE)` from `<util/atomic.h>` to prevent a torn read if the ISR fires between the two bytes.

## Build

```sh
# Install PlatformIO
pip install platformio

# Compile for Pro Trinket 5V
pio run -e pro_trinket_5v

# Run unit tests on host (no hardware required)
pio test -e native
```

## Diagram

`diagram/ethanolModule.kicad_sch` — editable KiCad 7+ schematic showing wiring between sensor, Arduino, and Raspberry Pi. Open with KiCad 7.0 or later.
