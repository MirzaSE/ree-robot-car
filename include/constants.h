/**
 * @file constants.h
 * @author José Ángel Sánchez (https://github.com/gelanchez)
 * @brief Constants used along the program.
 * @version 1.1.1
 * @date 2021-04-17
 * @copyright GPL-3.0
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

namespace Pins
{
    // Right motors pins
    constexpr unsigned char motorsEnA{5};
    constexpr unsigned char motorsInA1{7};
    //constexpr unsigned char motorsIn2{9};

    // Left motors pins
    constexpr unsigned char motorsEnB{6};
    constexpr unsigned char motorsInB1{8};
    //constexpr unsigned char motorsIn4{10};

    constexpr unsigned char motorsStby{3};

    // Line tracking sensor
    constexpr unsigned char ltLeftPin{A2};
    constexpr unsigned char ltMidPin{A1};
    constexpr unsigned char ltRightPin{A0};

    // Servo
    constexpr unsigned char servoPin{10};//11

    // Ultrasonic sensor
    constexpr unsigned char echoPin{12};    // Pin 18
    constexpr unsigned char triggerPin{13}; // Pin 19

    // Infrared
    constexpr unsigned char IRPin{9};

    // Key
    constexpr unsigned char KeyPin{2};

}

namespace Constants
{
    // Serial
    constexpr long serialBaud{9600}; // bps for Serial.begin
    constexpr long serialDelay{300}; // Initial serial delay (ms)

    // Motors min speed (measured)
    constexpr unsigned char crankSpeed{140}; // Around 120 @ full battery
    constexpr unsigned char idleSpeed{90};

    // Default speeds
    constexpr unsigned char linearSpeed{170};
    constexpr unsigned char rotateSpeed{150};
    constexpr unsigned short rotate90Time{650};   // Time to rotate 90 deg @ linearSpeed @ full battery
    constexpr unsigned short rotate180Time{1200}; // Time to rotate 180 deg @ rotateSpeed @ full battery

    // Obstacle avoidance
    constexpr unsigned short updateInterval{250}; // Default update time for states
    constexpr unsigned short minDistance{30};

    // Line tracking
    constexpr unsigned short minDetourDistance{10}; // Distance to keep with the obstacle
    constexpr unsigned short updateUltrasonicInterval{20};
    constexpr unsigned short extraTime{100};      // Extra time to keep moving the robot to pass the object
    constexpr unsigned short extraTimeLine{50};   // Extra time to pass the line
    constexpr unsigned short timeUntilLost{1000}; // Wait time until LOSTLINE and turn back
    constexpr unsigned short timeLost{5000};      // Time lost to find a new line
    constexpr unsigned char marginObject{1};      // Margin +- distance to the object

    // Park mode
    constexpr unsigned short timeMoving{500};
    constexpr unsigned short timeMoveAway{150};

    // Servo 0 deg and 180 deg PWM positions
    constexpr unsigned int servo0{500};    // Calibration 450, default 544
    constexpr unsigned int servo180{2400}; // Calibration 2430, default 2400

    // Ultrasonic sensor
    constexpr unsigned short maxDistance{250};             // Maximun distance to meassure in cm
    constexpr unsigned short maxDistanceLineTracking{100}; // Maximun distance to meassure in cm used in the linetraking mode

    // Infrared
    constexpr unsigned short IRMovingInterval{100}; // Default time for moving in IR
}

/**
 * @brief Different modes for the robot.
 */
enum class RobotMode
{
    REMOTECONTROL,
    IRCONTROL,
    OBSTACLEAVOIDANCE,
    LINETRACKING,
    PARK,
    CUSTOM,
};

/**
 * @brief Orders for the RemoteControl.
 */
enum class Order
{
    LEFT = 1,
    RIGHT,
    FORWARD,
    BACKWARD,
    STOP,
    FORWARD_LEFT,
    BACKWARD_LEFT,
    FORWARD_RIGHT,
    BACKWARD_RIGHT,
    UNKNOWN,
};

/**
 * @brief States for the different Robotmode.
 */
enum class RobotModeState
{
    START,
    FORWARD,
    OBSTACLE,
    ROTATE,
    BLOCKED,
    LINELOST, // Only used in linetracking
};

#endif