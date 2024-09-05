#include "Heading_Indicator.h"
#include "allocateMem.h"
#include "commandmessenger.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <logo.h>
#include <main_gauge.h>
#include <compass_rose.h>
#include <hdg_bug.h>

#define PANEL_COLOR 0x7BEE

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

TFT_eSprite mainGaugeSpr = TFT_eSprite(&tft);   // Main Sprite for gauge
TFT_eSprite compassRoseSpr = TFT_eSprite(&tft); // Sprite for compass rose
TFT_eSprite hdgBugSpr = TFT_eSprite(&tft);      // Sprite to hold Heading bug

/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

int data;


Heading_Indicator::Heading_Indicator(uint8_t Pin1, uint8_t Pin2)
{
    _pin1 = Pin1;
    _pin2 = Pin2;
}

void Heading_Indicator::begin()
{

  delay(1000); // wait for serial monitor to open
  digitalWrite(LED_BUILTIN, LOW);

  tft.begin();
  tft.setRotation(screenRotation);
  tft.fillScreen(PANEL_COLOR);
  tft.setPivot(320, 160);
  tft.setSwapBytes(true);
  tft.pushImage(160, 80, 160, 160, logo);
  delay(3000);
  tft.fillScreen(TFT_BLACK);

  mainGaugeSpr.createSprite(320, 320);
  mainGaugeSpr.setSwapBytes(true);
  mainGaugeSpr.fillSprite(TFT_BLACK);
  mainGaugeSpr.pushImage(0, 0, 320, 320, main_gauge);
  mainGaugeSpr.setPivot(160, 160);

  compassRoseSpr.createSprite(320, 320);
  compassRoseSpr.setSwapBytes(true);
  compassRoseSpr.fillSprite(TFT_BLACK);
  compassRoseSpr.pushImage(0, 0, 320, 320, compass_rose);
  compassRoseSpr.setPivot(160, 160);

  hdgBugSpr.createSprite(hdg_bug_width, hdg_bug_height);
  hdgBugSpr.setSwapBytes(true);
  hdgBugSpr.fillSprite(TFT_BLACK);
  hdgBugSpr.pushImage(0, 0, hdg_bug_width, hdg_bug_height, hdg_bug);
  hdgBugSpr.setPivot(hdg_bug_width / 2.0, 153);

}

void Heading_Indicator::attach(uint16_t Pin3, char *init)
{
    _pin3 = Pin3;
}

void Heading_Indicator::detach()
{
    if (!_initialised)
        return;
    _initialised = false;
}

void Heading_Indicator::set(int16_t messageID, char *setPoint)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -2 will be send from the board when PowerSavingMode is set
            Message will be "0" for leaving and "1" for entering PowerSavingMode
        MessageID == -1 will be send from the connector when Connector stops running
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */
    uint16_t data  = atoi(setPoint);

    // uint16_t output;

    // do something according your messageID
    switch (messageID) {
    case -1:
        // tbd., get's called when Mobiflight shuts down
        setPowerSaveMode(true);
        break;
    case -2:
        if (data == 1)
            setPowerSaveMode(true);
        else
            setPowerSaveMode(false);
        break;
        // tbd., get's called when PowerSavingMode is entered
    case 0:
        // output = (uint16_t)data;
        // data   = output;
        setHeading(atof(setPoint));
        break;
    case 1:
        setHeadingBug(atof(setPoint));
        /* code */
        break;
    case 2:
        /* code */
        setInstrumentBrightnessRatio(atof(setPoint));
        break;
    case 100:
        /* code */
        setScreenRotation(atoi(setPoint));
        break;
    default:
        break;
    }
}

void Heading_Indicator::update()
{

    analogWrite(TFT_BL, instrumentBrightness);
    if(prevScreenRotation != screenRotation)
    {
        tft.setRotation(screenRotation);
        prevScreenRotation = screenRotation;
    }
    drawAll();
    
}

void Heading_Indicator::drawAll()
{
  mainGaugeSpr.pushImage(0, 0, 320, 320, main_gauge);
  compassRoseSpr.pushRotated(&mainGaugeSpr, heading, TFT_BLACK);
  hdgBugSpr.pushRotated(&mainGaugeSpr, heading + hdgBug, TFT_BLACK);

  mainGaugeSpr.pushSprite(80, 0);
  mainGaugeSpr.fillSprite(TFT_BLACK);
}

void Heading_Indicator::setHeading(float value)
{
    heading = value * -1.0;  // Direction is reversed compared to the sim
}

void Heading_Indicator::setHeadingBug(float value)
{
    hdgBug = value;
}

void Heading_Indicator::setInstrumentBrightnessRatio(float ratio)
{
  instrumentBrightnessRatio = ratio;
  instrumentBrightness = round(scaleValue(instrumentBrightnessRatio, 0, 1, 0, 255));
}

void Heading_Indicator::setScreenRotation(int rotation)
{
  if(rotation == 1 || rotation == 3)
    screenRotation = rotation;
}


void Heading_Indicator::setPowerSaveMode(bool enabled)
{
    if(enabled)
    {
        powerSaveFlag = true;
    }
    else
    {
        powerSaveFlag = false;
    }
}

// Scale function
float Heading_Indicator::scaleValue(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}