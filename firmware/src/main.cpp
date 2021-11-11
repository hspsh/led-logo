#include <Arduino.h>

const uint8_t RED_PIN = D1;
const uint8_t GREEN_PIN = D2;
const uint8_t BLUE_PIN = D3;
static int brightness;
int fadeAmount = 5;
static unsigned char pos = 0;

void setColor(int r, int g, int b)
{
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(unsigned char WheelPos)
{
  if (WheelPos < 85)
  {
    setColor(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    setColor(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    setColor(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void setup()
{
}

void loop()
{
  pos++;
  Wheel(pos);
  delay(300); //delaying the speed of fading
}