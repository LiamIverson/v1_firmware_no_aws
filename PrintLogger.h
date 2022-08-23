#pragma once

#include <Arduino.h>

class PrintLogger
{
public:
  PrintLogger(Print *pPrint = nullptr)
      : mpPrint(pPrint)
  {
  }

  size_t print(const String &s)
  {
    if (mpPrint)
      return mpPrint->print(s);
    return 0;
  }

  size_t println(const String &s)
  {
    if (mpPrint)
      return mpPrint->println(s);
    return 0;
  }

private:
  Print *mpPrint;
};
