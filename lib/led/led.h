#include "FastLED.h"

class RBGLed
{
public:
  RBGLed();
  void Init(uint8_t set_Brightness);
  void Set(uint16_t Duration, uint8_t Traversal_Number, CRGB colour);
  void SetColor(uint8_t LED_s, uint8_t r, uint8_t g, uint8_t b);

public:
private:
#define PIN_RBGLED 4
#define NUM_LEDS 1
public:
  CRGB leds[NUM_LEDS];
};