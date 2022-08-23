#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "SimpleTask.h"

class Strip
{
public:
  uint8_t effect;
  uint8_t effects;
  uint16_t effStep;
  unsigned long effStart;
  Adafruit_NeoPixel strip;
  Strip(uint16_t leds, uint8_t pin, uint8_t toteffects, uint16_t striptype) : strip(leds, pin, striptype)
  {
    effect = -1;
    effects = toteffects;
    Reset();
  }
  void Reset()
  {
    effStep = 0;
    effect = (effect + 1) % effects;
    effStart = millis();
  }
};

class PixelController
{
public:
  PixelController(Adafruit_NeoPixel &strip) : mStrip(strip)
  {
  }

  PixelController &setupWave(int firstLedIdx, int countLed)
  {
    mTaskWave.setup([&, firstLedIdx, countLed](int iteration) {
      _effectRainbow(firstLedIdx, countLed, iteration);
    });

    return *this;
  }

  PixelController &setupFrame(int firstLedIdx, int countLed)
  {
    mTaskFrame.setup([&, firstLedIdx, countLed](int iteration) {
      _effectFade(firstLedIdx, countLed, 1, 25, 200, iteration);
    });

    return *this;
  }

  PixelController &setupLock(int firstLedIdx, int countLed)
  {
    mTaskLock.setup([&, firstLedIdx, countLed](int iteration) {
      _effectFade(firstLedIdx, countLed, 2, 20, 150, iteration);
    });

    return *this;
  }

  void attachWave(unsigned long ms, int times)
  {
    // detach();
    mTaskWave.attach(ms, times);
  }

  void attachFrame(unsigned long ms, int times)
  {
    // detach();
    mTaskFrame.attach(ms, times);
  }

  void attachLock(unsigned long ms, int times)
  {
    // detach();
    mTaskLock.attach(ms, times);
  }

  void loop()
  {
    mTaskWave.loop();
    mTaskFrame.loop();
    mTaskLock.loop();
  }

  void detach()
  {
    mTaskWave.detach();
    mTaskFrame.detach();
    mTaskLock.detach();
  }

private:
  void _effectRainbow(int firstLedIdx, int countLed, int iteration)
  {
    float factor1, factor2;
    uint16_t ind;
    for (uint16_t j = firstLedIdx; j < (firstLedIdx + countLed); j++)
    {
      ind = iteration + j * 2.6666666666666665;
      switch ((int)((ind % 160) / 53.333333333333336))
      {
      case 0:
        factor1 = 1.0 - ((float)(ind % 160 - 0 * 53.333333333333336) / 53.333333333333336);
        factor2 = (float)((int)(ind - 0) % 160) / 53.333333333333336;
        mStrip.setPixelColor(j, 0 * factor1 + 139 * factor2, 141 * factor1 + 241 * factor2, 212 * factor1 + 255 * factor2);
        break;
      case 1:
        factor1 = 1.0 - ((float)(ind % 160 - 1 * 53.333333333333336) / 53.333333333333336);
        factor2 = (float)((int)(ind - 53.333333333333336) % 160) / 53.333333333333336;
        mStrip.setPixelColor(j, 139 * factor1 + 0 * factor2, 241 * factor1 + 141 * factor2, 255 * factor1 + 212 * factor2);
        break;
      case 2:
        factor1 = 1.0 - ((float)(ind % 160 - 2 * 53.333333333333336) / 53.333333333333336);
        factor2 = (float)((int)(ind - 106.66666666666667) % 160) / 53.333333333333336;
        mStrip.setPixelColor(j, 0 * factor1 + 0 * factor2, 141 * factor1 + 141 * factor2, 212 * factor1 + 212 * factor2);
        break;
      }
    }

    mStrip.show();
  }

  void _effectFade(int firstLedIdx, int countLed, uint8_t red, uint8_t green, uint8_t blue, int iteration)
  {
    int duration = 100;
    double e = (iteration % duration) / (double)duration;
    if (iteration % duration == iteration % (2 * duration))
      e = 1.0 - e;

    for (int j = firstLedIdx; j < (firstLedIdx + countLed); j++)
    {
      mStrip.setPixelColor(j, red * e, green * e, blue * e);
    }

    mStrip.show();
  }

  Adafruit_NeoPixel mStrip;

  SimpleTask mTaskWave;
  SimpleTask mTaskFrame;
  SimpleTask mTaskLock;
};
