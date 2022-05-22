#include <Arduino.h>
#include "constants.h"
#include "infrared.h"
#include "linetracking.h"
#include "motors.h"
#include "myservo.h"
#include "robot.h"
#include "ultrasonic.h"

/**
 * @brief Construct a new Robot::Robot object.
 */
Robot::Robot()
    : m_motors{Pins::motorsEnA, Pins::motorsInA1,Pins::motorsEnB, Pins::motorsInB1, Pins::motorsStby, Constants::crankSpeed, Constants::idleSpeed},
      m_servo{Pins::servoPin, Constants::servo0, Constants::servo180},
      m_ultrasonic{Pins::triggerPin, Pins::echoPin},
      m_lineTracking{Pins::ltLeftPin, Pins::ltMidPin, Pins::ltRightPin},
      m_sonarMap{Constants::maxDistance, Constants::maxDistance, Constants::maxDistance, Constants::maxDistance, Constants::maxDistance},
      m_state{RobotModeState::START}, m_previousAngle{90}, m_interval{Constants::updateInterval}, m_infrared{Pins::IRPin}
{
    m_lastUpdate = millis();
    key.Init(Pins::KeyPin);
}

/**
 * @brief Destroy the Robot::Robot object.
 */
Robot::~Robot()
{
    m_servo.detach();
}

void Robot::restartState()
{
    if (m_servo.read() != 90)
        m_servo.write(90);
    m_lastUpdate = millis();
    if (!m_motors.isStopped())
        m_motors.stop();
    m_state = RobotModeState::START;
    m_previousAngle = 90;
    m_interval = Constants::updateInterval;
    for (size_t i{0}; i < 5; ++i)
    {
        m_sonarMap[i] = Constants::maxDistance; // Default values
    }
}

/**
 * @brief Start the robot.
 */
void Robot::begin()
{
    Serial.begin(Constants::serialBaud); // Can not be inside a constructor
    m_servo.begin();                     // Servo initialization can not be done inside Robot constructor
    m_infrared.begin();                  // Infrared initialization
}

/**
 * @brief Move the robot based on a remote order received by Bluetooth.
 * @param order Order.
 * @param linearSpeed
 * @param rotateSpeed
 */
void Robot::remoteControlMode(Order order, unsigned char linearSpeed, unsigned char rotateSpeed)
{

    switch (order)
    {
    case Order::LEFT:
        Serial.print("Remote control left");

        m_motors.left(rotateSpeed);
        return;
    case Order::RIGHT:
        m_motors.right(rotateSpeed);
        return;
    case Order::FORWARD:
        m_motors.forward(linearSpeed);
        return;
    case Order::BACKWARD:
        m_motors.backward(linearSpeed);
        return;
    case Order::STOP:
        m_motors.stop();
        return;
    case Order::FORWARD_LEFT:
        m_motors.forwardLeft(linearSpeed);
        return;
    case Order::BACKWARD_LEFT:
        m_motors.backwardLeft(linearSpeed);
        return;
    case Order::FORWARD_RIGHT:
        m_motors.forwardRight(linearSpeed);
        return;
    case Order::BACKWARD_RIGHT:
        m_motors.backwardRight(linearSpeed);
        return;
    default:
        return;
    }
}

/**
 * @brief Move the robot based on a remote order received by IR.
 * @param linearSpeed
 * @param rotateSpeed
 */
void Robot::IRControlMode(unsigned char linearSpeed, unsigned char rotateSpeed)
{
    Key key = m_infrared.decodeIR();
    switch (key)
    {
    case Key::keyOk:
        m_motors.stop();
        m_lastUpdate = millis();
        break;
    case Key::keyUp:
        m_motors.forward(linearSpeed);
        m_lastUpdate = millis();
        break;
    case Key::keyDown:
        m_motors.backward(linearSpeed);
        m_lastUpdate = millis();
        break;
    case Key::keyLeft:
        m_motors.left(rotateSpeed);
        m_lastUpdate = millis();
        break;
    case Key::keyRight:
        m_motors.right(rotateSpeed);
        m_lastUpdate = millis();
        break;
    default:
        break;
    }

    if ((millis() - m_lastUpdate) >= Constants::IRMovingInterval) // Stop after IRMovingInterval
    {
        m_lastUpdate = millis();
        m_motors.stop();
    }
}


/**
 * @brief Rerturns key received by IR.
 */
Key Robot::IRKeyPressed()
{
    return m_infrared.decodeIR(); 
}

/**
 * @brief Obstacle avoidance mode.
 */
void Robot::obstacleAvoidanceMode()
{
    if ((millis() - m_lastUpdate) >= m_interval)
    {
        m_lastUpdate = millis();
        m_sonarMap[mapAngle(m_servo.read())] = m_ultrasonic.getDistance(Constants::maxDistance);
        switch (m_state)
        {
        case RobotModeState::START:
        Serial.println("start");  
            if (m_sonarMap[2] >= Constants::minDistance)
            {
                m_previousAngle = 30;
                moveServoSequence();
                m_motors.forward(calculateSpeed(m_sonarMap[2]));
                m_state = RobotModeState::FORWARD;
                m_interval = Constants::updateInterval; // Reset scan interval
            }
            else
            {
                m_previousAngle = 0;
                moveServoSequence();
                m_motors.stop();
                m_state = RobotModeState::OBSTACLE;
                m_interval = Constants::updateInterval; // Reset scan interval
            }
            break;
        case RobotModeState::FORWARD:
        Serial.println("frwd");  
            if (m_sonarMap[2] < Constants::minDistance)
            {
                m_previousAngle = 0;
                moveServoSequence(); // Go to 180
                m_motors.stop();
                m_state = RobotModeState::OBSTACLE;
                m_interval = Constants::updateInterval; // Reset scan interval
            }
            else if (m_sonarMap[1] < Constants::minDistance)
            {
                if (!m_motors.isRotatingLeft())
                    m_motors.left(Constants::rotateSpeed);
                m_interval = 0; // Scan faster and don't turn the servo
            }
            else if (m_sonarMap[3] < Constants::minDistance)
            {
                if (!m_motors.isRotatingRight())
                    m_motors.right(Constants::rotateSpeed);
                m_interval = 0; // Scan faster and don't turn the servo
            }
            else
            {
                moveServoSequence();
                m_motors.forward(calculateSpeed(m_sonarMap[2]));
                m_interval = Constants::updateInterval; // Reset scan interval
            }
            break;
        case RobotModeState::OBSTACLE:
        Serial.println("obstacle");  
            if (m_servo.read() != 0)
                moveServoSequence();
            else
            {
                moveServoSequence(); // Go back to 90
                if ((m_sonarMap[0] < Constants::minDistance) && (m_sonarMap[2] < Constants::minDistance) && (m_sonarMap[4] < Constants::minDistance))
                    m_state = RobotModeState::BLOCKED;
                else
                    m_state = RobotModeState::ROTATE;
            }
            break;
        case RobotModeState::ROTATE:
        Serial.println("rt");
            (m_sonarMap[0] < m_sonarMap[4]) ? m_motors.left(Constants::rotateSpeed) : m_motors.right(Constants::rotateSpeed); // Condicional operator
            m_interval = Constants::rotate90Time;                                                                             // Rotate 90
            m_state = RobotModeState::START;
            m_sonarMap[1] = Constants::maxDistance; // Reset values
            m_sonarMap[3] = Constants::maxDistance; // Reset values
            break;
        case RobotModeState::BLOCKED:
        Serial.println("bl");
            (m_sonarMap[0] < m_sonarMap[4]) ? m_motors.left(Constants::rotateSpeed) : m_motors.right(Constants::rotateSpeed); // Condicional operator
            m_interval = Constants::rotate180Time;                                                                            // Rotate 180
            m_state = RobotModeState::START;
            m_sonarMap[1] = Constants::maxDistance; // Reset values
            m_sonarMap[3] = Constants::maxDistance; // Reset values
            break;
        default:
            break;
        }
    }
}

/**
 * @brief Line tracking mode.
 */
void Robot::lineTrackingMode()
{
    switch (m_state)
    {
    case RobotModeState::START:
        if (m_lineTracking.allLines()) // Car not on the floor
            break;
        if ((millis() - m_lastUpdate) >= Constants::updateUltrasonicInterval)
        {
            m_sonarMap[mapAngle(90)] = m_ultrasonic.getDistance(Constants::maxDistanceLineTracking);
            m_lastUpdate = millis();
            if ((m_sonarMap[mapAngle(90)] >= Constants::minDetourDistance) && m_lineTracking.anyLine())
                m_state = RobotModeState::FORWARD; // Move only if no obstacle and any line detected
        }
        break;
    case RobotModeState::FORWARD:
        // Update ultrasonic map
        if ((millis() - m_lastUpdate) >= Constants::updateUltrasonicInterval)
        {
            m_lastUpdate = millis();
            m_sonarMap[2] = m_ultrasonic.getDistance(Constants::maxDistanceLineTracking);
            if (m_sonarMap[2] < Constants::minDetourDistance) // Obstacle found
            {
                m_motors.stop();
                m_servo.write(0); // Look right
                m_state = RobotModeState::ROTATE;
                m_motors.left(Constants::rotateSpeed);
                m_lastUpdate = millis();
                delay(Constants::rotate90Time / 2); // To avoid the line detection in ROTATE
                return;
            }
        }

        if (m_lineTracking.leftLine())
            m_motors.left(Constants::rotateSpeed);
        else if (m_lineTracking.rightLine())
            m_motors.right(Constants::rotateSpeed);
        else if (m_lineTracking.midLine())
            m_motors.forward(calculateSpeed(m_sonarMap[2], Constants::minDetourDistance, Constants::maxDistanceLineTracking, Constants::linearSpeed));
        else // No line detected
        {
            // Wait to avoid line missing in between sensors
            unsigned long timeNoLine = millis();
            while ((millis() - timeNoLine) < Constants::timeUntilLost)
            {
                if (m_lineTracking.anyLine())
                    return;
            }
            m_motors.stop();
            m_state = RobotModeState::LINELOST;
        }
        break;
    case RobotModeState::OBSTACLE:
        if (!m_lineTracking.midLine())
        {
            if ((millis() - m_lastUpdate) >= Constants::updateUltrasonicInterval)
            {
                m_sonarMap[0] = m_ultrasonic.getDistance(Constants::maxDistanceLineTracking);
                if (m_sonarMap[0] > (Constants::minDetourDistance - Constants::marginObject) && m_sonarMap[0] < (Constants::minDetourDistance + Constants::marginObject))
                    m_motors.forward(Constants::linearSpeed);
                else if (m_sonarMap[0] > Constants::minDetourDistance) // Go closer
                    m_motors.move(calculateSpeed(m_sonarMap[0], 0, 100, Constants::linearSpeed), 0);
                else // Go further
                    m_motors.move(0, calculateSpeed(m_sonarMap[0], 0, 100, Constants::linearSpeed));
            }
        }
        else // Going around finished, line detected, last rotation
        {
            m_motors.forward(Constants::linearSpeed);
            delay(Constants::extraTimeLine); // Extra time to over pass the line
            m_motors.stop();
            m_servo.write(90); // Look front
            m_motors.left(Constants::rotateSpeed);
            while (!m_lineTracking.midLine())
                ; // Keep rotating until you find the line
            m_state = RobotModeState::START;
            m_motors.stop();
        }
        break;
    case RobotModeState::ROTATE: // Rotate 90 deg
        if ((millis() - m_lastUpdate) >= Constants::rotate90Time)
        {
            m_motors.stop();
            m_lastUpdate = millis();
            m_state = RobotModeState::OBSTACLE;
        }
        break;
    case RobotModeState::LINELOST:
    {
        m_motors.right(Constants::rotateSpeed); // Rotate 180 and find the line
        delay(Constants::rotate180Time);
        m_motors.forward(Constants::linearSpeed); // Try and find the line backwards

        // Wait and see if backwards you can find the line
        unsigned long timeNoLine = millis();
        while ((millis() - timeNoLine) < Constants::timeLost)
        {
            if (m_lineTracking.anyLine())
                break;
        }
        m_motors.stop();
        m_state = RobotModeState::START;
        break;
    }
    case RobotModeState::BLOCKED:
        break;
    default:
        break;
    }
}

/**
 * @brief Park mode.
 */
void Robot::parkMode()
{
    for (size_t i{0}; i <= 1; ++i)
    {
        m_servo.write(i * 180);
        delay(2 * Constants::updateInterval);
        m_sonarMap[mapAngle(m_servo.read())] = m_ultrasonic.getDistance(Constants::maxDistance);
    }
    if (m_sonarMap[mapAngle(0)] < m_sonarMap[mapAngle(180)]) // Park on the right
        m_servo.write(0);
    else
        m_servo.write(180); // Park on the left

    delay(2 * Constants::updateInterval); // Enough time to move the servo

    // Pass the 1st object
    while (m_ultrasonic.getDistance(Constants::maxDistance) < Constants::minDistance)
    {
        delay(20);
        m_motors.forward(Constants::crankSpeed);
    }

    // Arrive to the second object
    while (m_ultrasonic.getDistance(Constants::maxDistance) > Constants::minDistance)
    {
        delay(50);
        m_motors.forward(Constants::crankSpeed);
    }

    m_motors.backward(Constants::crankSpeed);
    delay(Constants::timeMoveAway); // Time to move away

    if (m_sonarMap[mapAngle(0)] < m_sonarMap[mapAngle(180)]) // Park on the right
        m_motors.right(Constants::rotateSpeed);
    else // Park on the left
        m_motors.left(Constants::rotateSpeed);
    delay(Constants::rotate90Time);
    m_motors.stop();
    m_motors.forward(Constants::crankSpeed);
    delay(Constants::timeMoving);

    if (m_sonarMap[mapAngle(0)] < m_sonarMap[mapAngle(180)]) // Park on the right
        m_motors.left(Constants::rotateSpeed);
    else // Park on the left
        m_motors.right(Constants::rotateSpeed);
    delay(Constants::rotate90Time);
    m_motors.stop();
}

/**
 * @brief Custom mode. If you have reached this, you won't probably use
 * the programming capabilites on the app but this one :).
 */
void Robot::customMode()
{
    if (m_servo.read() != 0)
    {
        m_servo.write(0);
        delay(300);
        return;
    }
    m_sonarMap[0] = m_ultrasonic.getDistance(Constants::maxDistanceLineTracking);
    if (m_sonarMap[0] > (Constants::minDetourDistance - Constants::marginObject) && m_sonarMap[0] < (Constants::minDetourDistance + Constants::marginObject))
        m_motors.forward(Constants::linearSpeed);
    else if (m_sonarMap[0] > Constants::minDetourDistance) // Go closer
        m_motors.move(calculateSpeed(m_sonarMap[0], 0, 100, Constants::linearSpeed), 0);
    else // Go further
        m_motors.move(0, Constants::linearSpeed);
}

/**
 * @brief Map an angle to a position in the m_sonarMap array.
 * @param angle Angle of the servo.
 * @return unsigned char Position in the array.
 */
unsigned char Robot::mapAngle(unsigned char angle) const
{
    switch (angle)
    {
    case 0:
        return 0;
    case 30:
        return 1;
    case 90:
        return 2;
    case 150:
        return 3;
    case 180:
        return 4;
    default:
        return 0;
    }
}

/**
 * @brief Move the servo for the obstable avoidance mode.
 */
void Robot::moveServoSequence() // Sequences: {90, 150, 90, 30} {90, 180, 90, 0}
{    
    unsigned char currentAngle = m_servo.read();
    if (currentAngle != 90)
        m_servo.write(90);
    else
    {
        if (m_previousAngle == 150)
            m_servo.write(30);
        else if (m_previousAngle == 30)
            m_servo.write(150);
        else if (m_previousAngle == 180)
            m_servo.write(0);
        else if (m_previousAngle == 0)
            m_servo.write(180);
    }
    m_previousAngle = currentAngle;
}

/**
 * @brief Calculate a limited linear robot speed based on the object distance in front.
 * @param distance Object distance.
 * @param minDistance Minimum distance.
 * @param maxDistance Maxiumn distance.
 * @param minSpeed Minimum speed.
 * @return unsigned char Calculated speed.
 */
unsigned char Robot::calculateSpeed(unsigned short distance, unsigned short minDistance, unsigned short maxDistance, unsigned char minSpeed) const
{
    return minSpeed + static_cast<unsigned char>((distance - minDistance) * (255 - minSpeed) / (maxDistance - minDistance));
}


void Robot::setLed(CRGB color)
{
    m_led.Set(0, 2, color);
}