#pragma once

#include <Arduino.h>

class HexString : public String
{
public:
  HexString(const String &value) : String(value) {}

  inline unsigned int safeLength() const
  {
    auto len = String::length();
    return len - len % 2;
  }

  inline unsigned int convertToByteArray(unsigned char *dest) const
  {
    const auto bytesLength = safeLength() / 2;
    for (size_t i = 0, j = 0; j < bytesLength; i += 2, j++)
      dest[j] = (String::charAt(i) % 32 + 9) % 25 * 16 + (String::charAt(i + 1) % 32 + 9) % 25;
    return bytesLength;
  }
};
