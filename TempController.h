#pragma once

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "SimpleTask.h"
#include "PrintLogger.h"

#include "LED_status.h"

float Temp;
int TempTime;
int CompState = 0;
int CompTime;
int MaxRunTime = 1200;
int RestTime = 600;

tm timeinfo;
time_t now;
int time_output;
int getNTPtime ()
{
   time_output = time(nullptr);
   delay (200);
   return (time_output);
}

class TempController
{
public:
  TempController(uint8_t oneWireBus, uint8_t relayPin) : mOneWireBus(oneWireBus), mRelayPin(relayPin), mOneWire(oneWireBus), mSensor(&mOneWire)
  {
  }

  void setup(float minTemp, float maxTemp, PrintLogger &Logger)
  {
    pinMode(mRelayPin, OUTPUT);
    pinMode(mOneWireBus, INPUT_PULLUP);
    mSensor.begin();

    mTaskCheck.setup([&, minTemp, maxTemp](int iteration) {
      mSensor.requestTemperatures();
      float temp = mSensor.getTempCByIndex(0);

//consider adding the following for offset from the calibration of the V0 fridge thermister
//    Temp_offset = 3 //make a global value in the config file
//    temp = temp + Temp_offset
      
      Temp = temp;  //for the heartbeat temperature
      TempTime = getNTPtime();
      
      if (CompState < 2) //Check the compressor is not in rest mode, Then run usual operations
      {
        if (temp < -10)
        {
          Logger.println("Too Cold [ALARM]");
        }
  
        if ((temp>7) && (temp<10))
        {
          Logger.println("Too Warm [ALARM]");
         
         // WipeBlack();
         // WipeOrange();
         // WipeYellow();
         // WipeOrange();
         // WipeBlack();
        
        } 
  
        if (temp > 10)
        {
          Logger.println("Too Warm [ALARM]");
          
         // WipeBlack();
         // WipeRed();
         // WipePink();
         // WipeWhite();
         // WipePink();
         // WipeRed();
         // WipeBlack();
        
        }
  
        if (temp > maxTemp)
        {
          digitalWrite(mRelayPin, HIGH);
          if (CompState == 0)
          {
          CompState = 1;
          CompTime = TempTime;
          }
          Logger.println("Temp: " + String(temp) + "[Start Cooling]");
          //PulseBlue;
          WipeBlue();
        }
        else if (temp < minTemp)
        {
          digitalWrite(mRelayPin, LOW);
          if (CompState == 1)
          { 
          CompState = 0;
          CompTime = TempTime;
          }
          Logger.println("Temp: " + String(temp) + "[Stop Cooling]");
          WipeBlack();
        }
        if ((temp>minTemp) && (temp<maxTemp) && (CompState == 0))
        {
         // PulseGreen();
         //WipeGreen();
        }
  
        if (CompState == 1 && (TempTime - CompTime) > MaxRunTime) //if the compressor in this model has been running for more than 20 minutes it is likely due to a large thermal load in the locker with the thermistor. it needs to take a rest for the sake of the rest of the food
          {
            digitalWrite(mRelayPin, LOW); //turn Comp off
            CompState = 2;
            CompTime = TempTime;
          }
      }
      
      if (CompState == 2) //if the compressor is in rest mode. Check how long it has been in rest mode and turn off after set time
      {
        if ((TempTime-CompTime) >  RestTime)  //if the rest time is complete
          {
            CompState = 0;
            CompTime = TempTime;
          }
      }
      
    });
  }

  void attach(unsigned long checkInterval)
  {
    mTaskCheck.attach(checkInterval);
  }

  void loop()
  {
    mTaskCheck.loop();
  }

private:
  uint8_t mOneWireBus;
  uint8_t mRelayPin;

  OneWire mOneWire;
  DallasTemperature mSensor;

  SimpleTask mTaskCheck;
};
