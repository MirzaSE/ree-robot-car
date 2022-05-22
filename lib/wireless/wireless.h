#include <Arduino.h>

class Wireless
{
public:
    void Get(unsigned int *get_keyValue);
    void Init(unsigned char keyPin);
public:
#define keyValue_Max 4

public:
  
  Wireless();
  static unsigned int keyValue;
};