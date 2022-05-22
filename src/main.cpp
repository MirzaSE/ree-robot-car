#include "bluetooth.h"
#include "constants.h"
#include "robot.h"

static Robot g_robot = Robot();                     // Initialization of the Robot object
static Bluetooth g_bluetooth = Bluetooth();         // Initialization of the bluetooth object
static RobotMode g_mode = RobotMode::IRCONTROL; // Default robot mode




void setLedColor(){
       
    Serial.println("Set led color");  
    switch(g_mode)
    {
        case RobotMode::CUSTOM:
        Serial.println("Violet");  
            g_robot.setLed(CRGB::Violet);
            break;
        case RobotMode::LINETRACKING:
         Serial.println("Yellow");  
            g_robot.setLed(CRGB::Yellow);
            break;
        case RobotMode::OBSTACLEAVOIDANCE:
         Serial.println("Green");  
           g_robot.setLed(CRGB::Green);
            break;
        case RobotMode::PARK:
         Serial.println("white");  
           g_robot.setLed(CRGB::White);
            break;
        case RobotMode::IRCONTROL:
        Serial.println("blue");  
           g_robot.setLed(CRGB::Blue);
            break;
        default:
           g_robot.setLed(CRGB::Red);
           Serial.println("red");  
            break;
    }
}

void checkIRKey(Key key)
{
    RobotMode previous_mode = g_mode;
    switch(key)
    {
        case Key::key0:
            Serial.println("Custom");  
            g_mode = RobotMode::CUSTOM;            
            break;
        case Key::key1:
        Serial.println("line");  
            g_mode = RobotMode::LINETRACKING;
            break;
        case Key::key2:
        Serial.println("obstacle");  
            g_mode = RobotMode::OBSTACLEAVOIDANCE;
            break;
        case Key::key3:
        Serial.println("park");  
            g_mode = RobotMode::PARK;
            break;
        case Key::key4:
        Serial.println("IR");  
            g_mode = RobotMode::IRCONTROL;
            break;
        default:    
            break;
    }

    if(previous_mode != g_mode)
        setLedColor();

}


/**
 * @brief Main setup. Initialize robot.
 */
void setup()
{
    Serial.print("Begin");
    g_robot.begin();
    setLedColor();
    delay(Constants::serialDelay); // To make Serial work
}

/**
 * @brief Main loop. Process bluetooth order and run the mode.
 */
void loop()
{
    g_bluetooth.receiveData();
    if (g_bluetooth.getData() != "")
    {      
        Serial.print("Data");  
        Serial.print(g_bluetooth.getData());
        g_bluetooth.decodeElegooJSON();
        if (g_mode != g_bluetooth.getMode())
        {
            Serial.println("Restart state");  
            g_robot.restartState();
            setLedColor();
        }
    }
    else
    {        
        Key key = g_robot.IRKeyPressed();
        checkIRKey(key);
    }

    

    switch (g_mode)
    {
    
    case RobotMode::REMOTECONTROL:
        g_mode = RobotMode::REMOTECONTROL;        
        g_robot.remoteControlMode(g_bluetooth.getOrder(), g_bluetooth.getSpeed(), g_bluetooth.getSpeed());
        break;

    case RobotMode::IRCONTROL:
    {
        g_mode = RobotMode::IRCONTROL;
        //g_robot.setLed(CRGB::Orange);
        g_robot.IRControlMode();
        //checkIRKey(key);
        break;
    }

    case RobotMode::OBSTACLEAVOIDANCE:
        g_mode = RobotMode::OBSTACLEAVOIDANCE;
       
        g_robot.obstacleAvoidanceMode();        
        break;

    case RobotMode::LINETRACKING:
        g_mode = RobotMode::LINETRACKING;
        //g_robot.setLed(CRGB::Green);
        g_robot.lineTrackingMode();        
        break;

    case RobotMode::PARK:
        g_robot.parkMode();
        g_mode = RobotMode::REMOTECONTROL;
        //g_robot.setLed(CRGB::Violet);
        g_robot.restartState();
        break;

    case RobotMode::CUSTOM:
        g_robot.customMode();
        break;

    default:
        break;
    }
}
