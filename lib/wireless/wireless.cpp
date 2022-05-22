#include "wireless.h"


unsigned int Wireless::keyValue = 0;
static void GetValue(void)
{
  Wireless Key;
  static unsigned int keyValue_time = 0;
  static unsigned int keyValue_temp = 0;
  if ((millis() - keyValue_time) > 500)
  {
    keyValue_temp++;
    keyValue_time = millis();
    if (keyValue_temp > keyValue_Max)
    {
      keyValue_temp = 0;
    }
    Key.keyValue = keyValue_temp;
  }
}

void Wireless::Init(unsigned char keyPin)
{
     pinMode(keyPin, INPUT_PULLUP);
     attachInterrupt(0, GetValue, FALLING);
}

Wireless::Wireless()
{   
}

void Wireless::Get(unsigned int *get_keyValue)
{
  *get_keyValue = keyValue;
}