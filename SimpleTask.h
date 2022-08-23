#pragma once

#include <Arduino.h>
#include <functional>

class SimpleTask
{
public:
  using TaskCallback = std::function<void(int)>;

  SimpleTask()
      : mCallback(nullptr), mStepMillis(0), mTimes(0) {}

  SimpleTask *setup(TaskCallback callback)
  {
    mCallback = callback;
    return this;
  }

  SimpleTask *attach(unsigned long ms, int times = -1)
  {
    mStepMillis = ms;
    mWholeTimes = mTimes = times;
    mOldMillis = millis();
    return this;
  }

  void detach()
  {
    mStepMillis = 0;
  }

  void change(unsigned long ms)
  {
    mStepMillis = ms;
  }

  virtual void loop()
  {
    if (mStepMillis && mTimes && (millis() - mOldMillis >= mStepMillis))
    {
      if (mCallback)
      {
        int iteration = mTimes > 0 ? (mWholeTimes - mTimes) : mTimes;
        mCallback(iteration);
      }

      if (mTimes > 0)
        --mTimes;

      mOldMillis = millis();
    }
  }

private:
  TaskCallback mCallback;
  unsigned long mOldMillis;
  unsigned long mStepMillis;
  int mTimes;
  int mWholeTimes;
};
