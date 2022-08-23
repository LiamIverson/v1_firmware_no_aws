#pragma once

#include <Arduino.h>
#include <functional>

class TimeManager
{
public:
  using YieldCallback = std::function<void()>;

  TimeManager() : mCallback(nullptr)
  {
  }

  void setup(YieldCallback callback)
  {
    mCallback = callback;
  }

  void pass()
  {
    if (mCallback)
      mCallback();

    yield();
  }

  void delayMillis(unsigned long delayMillis)
  {
    auto startMillis = millis();
    pass();
    auto passedMillis = millis() - startMillis;

    if (delayMillis > passedMillis)
    {
      delay(delayMillis - passedMillis);
    }
  }

private:
  YieldCallback mCallback;
};
