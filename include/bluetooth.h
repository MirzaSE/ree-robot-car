#ifndef BLUETOOTH
#define BLUETOOTH

#include <ArduinoJson.h>
#include "constants.h"

class Bluetooth
{
private:
    String m_data;
    StaticJsonDocument<150> m_elegooDoc;
    RobotMode m_mode;
    Order m_order;
    unsigned short m_speed;

public:
    Bluetooth();
    ~Bluetooth();
    void decodeElegooJSON();
    String getData() const;
    RobotMode getMode() const;
    Order getOrder() const;
    unsigned short getSpeed() const;
    void receiveData();
    void setMode(RobotMode mode);
};

#endif