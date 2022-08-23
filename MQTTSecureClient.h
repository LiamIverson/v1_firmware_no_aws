#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>

class MQTTSecureClient : public MQTTClient
{
public:
  MQTTSecureClient(const char certCA[], const char certClient[], const char pkeyClient[])
      : mCertCA(certCA), mCertClient(certClient), mPrivateKeyClient(pkeyClient)
  {
    mClient.setTrustAnchors(&mCertCA);
    mClient.setClientRSACert(&mCertClient, &mPrivateKeyClient);
  }

  void begin(const char hostname[], int port)
  {
    MQTTClient::begin(hostname, port, mClient);
  }

  WiFiClientSecure *getClient()
  {
    return &mClient;
  }

private:
  X509List mCertCA;
  X509List mCertClient;
  PrivateKey mPrivateKeyClient;
  WiFiClientSecure mClient;
};
