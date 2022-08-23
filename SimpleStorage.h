#pragma once

#include <Arduino.h>
#include <EEPROM.h>

class SimpleStorage
{
public:
  SimpleStorage(size_t size)
      : mSize(size)
  {
  }

  bool store(const String &value)
  {
    EEPROM.begin(mSize);
    auto success = _write(value);
    EEPROM.end();

    return success;
  }

  String restore()
  {
    EEPROM.begin(mSize);
    String value = _read();
    EEPROM.end();

    return value;
  }

  void clean()
  {
    EEPROM.begin(mSize);
    uint8_t *data = EEPROM.getDataPtr();
    memset(data, 0, mSize);
    EEPROM.end();
  }

private:
  bool _write(const String &str)
  {
    uint8_t *data = EEPROM.getDataPtr();
    if (data && str.length() < mSize - 1)
    {
      // '-1' in 'if' statement guarantees place for the '\0' character
      memcpy(data, (const uint8_t *)str.c_str(), str.length());
      memset(data + str.length(), 0, mSize - str.length());
      return true;
    }
    return false;
  }

  String _read()
  {
    if (!EEPROM.read(mSize - 1))
    {
      // String in storage must be terminated by the '\0' character
      // also EEPROMClass::read(int address) returns 0 if internal error occurs
      // EEPROM.getDataPtr() marks data as dirty and EEPROM commits at closing
      uint8_t *data = EEPROM.getDataPtr();
      if (data)
      {
        return String((const char *)(data));
      }
    }
    return String();
  }

  size_t mSize;
};
