#include <Servo.h>
#include "myservo.h"

/**
 * @brief Construct a new MyServo::MyServo object which inherits from Servo.
 */

/**
 * @brief Construct a new MyServo::MyServo object which inherits from Servo.
 * @param servoPin Servo pin.
 * @param servo0 0 deg PWM position.
 * @param servo180 180 deg PWM position.
 */
MyServo::MyServo(unsigned char servoPin, unsigned int servo0, unsigned int servo180)
    : Servo(), m_servoPin{servoPin}, m_servo0{servo0}, m_servo180{servo180}
{
}

/**
 * @brief Destroy the MyServo::MyServo object.
 */
MyServo::~MyServo()
{
}

/**
 * @brief Servo initialization. It can not be done inside Robot constructor.
 */
void MyServo::begin()
{
    Servo::attach(m_servoPin, m_servo0, m_servo180);
    Servo::write(90);
}