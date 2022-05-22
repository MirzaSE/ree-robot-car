#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

class Motors
{
private:
    unsigned char m_enableA, m_input_a1; // Right motors pins
    unsigned char m_enableB, m_input_b1; // Left motors pins
    unsigned char m_stby; //stand by pin
    unsigned char m_crankSpeed, m_idleSpeed;     // Minimum speeds    
    short m_leftSpeed, m_rightSpeed;

public:
    Motors(unsigned char enableA, unsigned char input_a1, 
            unsigned char enableB, unsigned char input_b1, 
            unsigned char in_stby, unsigned char crankSpeed, unsigned char idleSpeed);
 
    ~Motors();
    short getLeftSpeed() const;
    short getRightSpeed() const;
    bool isStopped() const;
    bool isRotatingLeft() const;
    bool isRotatingRight() const;
    void move(short leftSpeed, short rightSpeed);
    void forward(unsigned char speed);
    void backward(unsigned char speed);
    void left(unsigned char speed);
    void right(unsigned char speed);
    void forwardLeft(unsigned char speed);
    void forwardRight(unsigned char speed);
    void backwardLeft(unsigned char speed);
    void backwardRight(unsigned char speed);
    void off();
    void stop();
};

#endif