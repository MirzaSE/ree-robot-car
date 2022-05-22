#ifndef INFRARED_H
#define INFRARED_H

// To avoid using old versions of Arduino IDE, which used WPProgram.h instead or Arduino.h
// Also possible #define IRPRONTO
#ifndef ARDUINO
//#define ARDUINO 108012 // Arduino IDE version used when writing the program
#endif

/**
 * @brief Keys.
 */
enum class Key
{
    keyOk,
    keyUp,
    keyDown,
    keyLeft,
    keyRight,
    key0,
    key1,
    key2,
    key3,
    key4,
    key5,
    key6,
    key7,
    key8,
    key9,
    keyAsterisk,
    keySharp,
    unkwown
};

class Infrared
{
private:
    unsigned char m_IRPin;       // IR receiver pin
    unsigned long m_previousKey; // Store previous order for repeating key    
public:
    Infrared(unsigned char IRPin);
    ~Infrared();
    void begin();
    Key decodeIR();
};

#endif