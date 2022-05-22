#ifndef MYSERVO_H
#define MYSERVO_H

#include <Servo.h>

class MyServo : public Servo
{
private:
    unsigned char m_servoPin; // Servo pin
    unsigned int m_servo0;    // 0 deg PWM position
    unsigned int m_servo180;  // 180 deg PWM position
public:
    MyServo(unsigned char servoPin, unsigned int servo0, unsigned int servo180);
    ~MyServo();
    void begin();
};

#endif