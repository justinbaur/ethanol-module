#include <Wire.h>
#include <util/atomic.h>
#include "ethanol_math.h"

#define I2C_ADDRESS 0x04

const int INPUT_PIN = 8;
const int LED_PIN = 13;
const int FUEL_CONTENT_NUMBER = 1;

int data = 0;
int number = 0;

volatile uint16_t revTick;
int HZ = 0;
int ethanol = 0;

void setupTimer(void)
{
  TCCR1A = 0;
  // TCCR1B: ICNC1=1, ICES1=1 (rising edge capture), CS12=1 (prescaler /256) → 0b10000100 = 132
  TCCR1B = 132;
  TCCR1C = 0;
  // TIMSK1: ICIE1=1 (input capture interrupt), TOIE1=1 (overflow interrupt) → 0b00100001 = 33
  TIMSK1 = 33;
  TCNT1 = 0;
}

/* Timer/Counter1 Capture Event */
ISR(TIMER1_CAPT_vect)
{
  revTick = ICR1;
  TCNT1 = 0;
}

/* Timer/Counter1 Overflow — sensor signal lost */
ISR(TIMER1_OVF_vect)
{
  revTick = 0;
}

void setup(void)
{
  Wire.begin(I2C_ADDRESS);

  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  Serial.begin(9600);
  pinMode(INPUT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  setupTimer();
}

void loop(void)
{
  // revTick is uint16_t written by ISR — read atomically to avoid torn 16-bit read on 8-bit AVR
  uint16_t tick;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    tick = revTick;
  }

  if (tick > 0)
  {
    // 62200 ≈ F_CPU(16 MHz) / prescaler(256); empirically calibrated for this sensor
    HZ = 62200 / tick;
    ethanol = hz_to_ethanol(HZ);
  }
  else
  {
    HZ = 0;
    ethanol = 0;
  }

  delay(1000);
}

void receiveData(int byteCount)
{
  while (Wire.available())
  {
    digitalWrite(LED_PIN, HIGH);
    number = Wire.read();

    if (number == FUEL_CONTENT_NUMBER)
    {
      data = ethanol;
    }
  }
}

void sendData(void)
{
  Wire.write((uint8_t)data);
  digitalWrite(LED_PIN, LOW);
}
