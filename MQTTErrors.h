#pragma once

#include <Arduino.h>
#include <MQTT.h>

String getError(lwmqtt_err_t reason)
{
  switch (reason)
  {
  case LWMQTT_SUCCESS:
    return "Success";
  case LWMQTT_BUFFER_TOO_SHORT:
    return "Buffer too short";
  case LWMQTT_VARNUM_OVERFLOW:
    return "Varnum overflow";
  case LWMQTT_NETWORK_FAILED_CONNECT:
    return "Network failed connect";
  case LWMQTT_NETWORK_TIMEOUT:
    return "Network timeout";
  case LWMQTT_NETWORK_FAILED_READ:
    return "Network failed read";
  case LWMQTT_NETWORK_FAILED_WRITE:
    return "Network failed write";
  case LWMQTT_REMAINING_LENGTH_OVERFLOW:
    return "Remaining length overflow";
  case LWMQTT_REMAINING_LENGTH_MISMATCH:
    return "Remaining length mismatch";
  case LWMQTT_MISSING_OR_WRONG_PACKET:
    return "Missing or wrong packet";
  case LWMQTT_CONNECTION_DENIED:
    return "Connection denied";
  case LWMQTT_FAILED_SUBSCRIPTION:
    return "Failed subscription";
  case LWMQTT_SUBACK_ARRAY_OVERFLOW:
    return "Suback array overflow";
  case LWMQTT_PONG_TIMEOUT:
    return "Pong timeout";
  default:
    return "Unknown";
  }
}

String getError(lwmqtt_return_code_t reason)
{
  switch (reason)
  {
  case LWMQTT_CONNECTION_ACCEPTED:
    return "Connection Accepted";
  case LWMQTT_UNACCEPTABLE_PROTOCOL:
    return "Unacceptable Protocol";
  case LWMQTT_IDENTIFIER_REJECTED:
    return "Identifier Rejected";
  case LWMQTT_SERVER_UNAVAILABLE:
    return "Server Unavailable";
  case LWMQTT_BAD_USERNAME_OR_PASSWORD:
    return "Bad UserName/Password";
  case LWMQTT_NOT_AUTHORIZED:
    return "Not Authorized";
  case LWMQTT_UNKNOWN_RETURN_CODE:
    return "Unknown Return Code";
  default:
    return "Unknown";
  }
}
