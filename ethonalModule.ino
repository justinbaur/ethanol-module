#include <Wire.h>

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
  TCCR1B = 132;
  TCCR1C = 0;
  TIMSK1 = 33;
  TCNT1 = 0;
}

/* Timer/Counter1 Capture Event */
ISR(TIMER1_CAPT_vect)
{
  revTick = ICR1;
  TCNT1 = 0;
}

/* Timer/Counter1 Overflow */
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
  if (revTick > 0)
  {
    HZ = 62200 / revTick;
    ethanol = (HZ - 50);
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
  Wire.write(data);
  digitalWrite(LED_PIN, LOW);
}