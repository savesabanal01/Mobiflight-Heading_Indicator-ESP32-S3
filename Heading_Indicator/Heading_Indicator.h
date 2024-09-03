#pragma once

#include "Arduino.h"

class Heading_Indicator
{
public:
    Heading_Indicator(uint8_t Pin1, uint8_t Pin2);
    void begin();
    void attach(uint16_t Pin3, char *init);
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;

    float heading = 0; // Heading value from sim
    float hdgBug = 0; // heading bug value from sim
    float instrumentBrightnessRatio = 1;
    int instrumentBrightness = 255;
    int screenRotation = 3;
    int prevScreenRotation = 3;
    bool powerSaveFlag = false;

    // Function declarations
    void drawAll();
    void setHeading(float value);
    void setHeadingBug(float value);
    void setInstrumentBrightnessRatio(float ratio);
    void setScreenRotation(int rotation);
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
};