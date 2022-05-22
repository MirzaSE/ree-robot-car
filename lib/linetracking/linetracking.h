#ifndef LINETRACKING_H
#define LINETRACKING_H

class LineTracking
{
private:
    unsigned char m_leftPin; // Left sensor pin
    unsigned char m_midPin; // Mid sensor pin
    unsigned char m_rightPin; // Right sensor pin
public:
    LineTracking(unsigned char leftPin, unsigned char midPin, unsigned char rightPin);
    ~LineTracking();
    bool allLines() const;
    bool anyLine() const;
    bool leftLine() const;
    bool midLine() const;
    void printLines() const;
    bool rightLine() const;
};

#endif