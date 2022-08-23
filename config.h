#pragma once

#include <pgmspace.h>

#define Version "V14.2.6P" 
//  Version includes:
//  Temp sent on heartbeat
//  minTemp control sent through mqtt maintopic/mintemp
//  minTemp and maxTemp set in config file
//  LED Temp warning -- uncomment LED Whipe in TempController.h
//  Temp is float, Compressor State On/Off tracked with time of state and sent in heartbeat
//  Parallel mqtt networks
//  LEDS blue for comp on. orange warning LED >7, red warning LED >10.
//  CompState is int and CompTime only changes with change to CompState
//  TempTime is tracked
//  multiple data points are sent to Liam's mqtt network: Temp,TempTime, Time at send (for testing lag)- needs to be converted to milliseconds to be useful.
//  reduced delay in getNTPtime()
//  Recconect Parallel mqtt after Wifi down
//  In this Version:
//  2nd network resets the board on message 'R'
//  Parallel mqtt connection is checked after main mqtt connection/wifi,mqtt connect is checked. This should avoid interuptions with service if wifi was not dropped but parallel network was
//  CompState 2 is added to track when the compressor has run more then MaxRunTime. RestTime also used to keep comp off after MaxRunTime. This should assist whith preventing freezing in units when a high thermal load is added to the thermistor 


const char* TZ_INFO    = "CST6";  //<-- Set timezone here

float minTemp = 2;
float maxTemp = 4;

#define CONFIG_WIFI_SSID "ArcticShelf-AB005"
#define CONFIG_WIFI_PASSWORD "keepcool"

#define CONFIG_THING_NAME "AB005"


#define CONFIG_MQTT_HOST "a3lgi4bsmoc5uk-ats.iot.us-east-1.amazonaws.com"
#define CONFIG_MQTT_PORT 8883

#define CONFIG_LED_PIN D3
#define CONFIG_LED_NUMPIXELS 28

#define CONFIG_ONE_WIRE_BUS D7
#define CONFIG_RELAY_PIN D5

// Set the default mqtt topic here. It will be used as the default value on the WiFiManager web page
String MQTT_MAIN_TOPIC = "CA-SK/MMK/AB005";

//standard root certificate do not change public.pem
static const char CONFIG_CA_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

//this is the uniqe device certificate can be redownloaded
//xxxxxx-certificate.pem.crt
static const char CONFIG_THING_CERT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAKGlcRYmf0o7tqD3PWP6Lvp4JWOCMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDEwMjEyMDAx
MjRaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCwqY6R9Cjh7rJ/IHYt
Ryx7J5iR+/t0b04SIouXK7d1ChvMsnoQRxBs/7unYbqpNgV5SxCQQ/gRnlHXgW6E
Arf4f7YnsJ5kJ7wE3u8MSnYnwFH7yttF6YjE4yCV1ntgyC4FJSOXgd19vfeevy3i
bJ1E45iFAXIOJefZZxGyESlOKWYMLuj6cs5PGbSzlj/9J+M6yZiqqZOsJm2G+ebd
CcQ9+M3BPNwj6cs8OFl6YRRYwYlBGzdWwBjwRjsSsXFaRDrK1CCXKAUrNhLarBtH
gQRbaD1zGHYeqngwp9aENcF2S1LSdWdEdOtzlRyKF/uGN7jyJ/2T4YfkGf8AlGPg
dmD3AgMBAAGjYDBeMB8GA1UdIwQYMBaAFJvzv+TjT1Zqtxm24CoHZP6h+HfJMB0G
A1UdDgQWBBTuiXiWnDttSL3N4nZXBucWxFyKrTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEArJCDHKTBRq3LN/JcROcM7WFv
Mz4iC1WzP4bxXekGLwydn1/yUfCHvGihHJPGVnfiOHeqkhx+C12DATCjSYOdqgMD
iTDwj0O1DeKcozEpVQsFD7hJsxkcWjb6WGjzSkiGe51zllchbq8GepE2KGxRdOI4
4Hh1uquDVleYQpqHcEZCjU3w04pwDZt5EOWQCpOiW+WKxsxTpnev5zilML1tJM/N
chD/JN4L+CLz+bLqdAWeVOhgzk8GwbMmS1oX/6mqAxngdARME5Mwttv3qmfjOZia
gYuo5HMr7A4uu/fiOyiDdsEo4Q6DKBD0Wz/UJGHwK5JAqZcFMs3jPDwTHDny/w==
-----END CERTIFICATE-----

)KEY";

//xxxxxx-private.pem.key
static const char CONFIG_THING_PKEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAsKmOkfQo4e6yfyB2LUcseyeYkfv7dG9OEiKLlyu3dQobzLJ6
EEcQbP+7p2G6qTYFeUsQkEP4EZ5R14FuhAK3+H+2J7CeZCe8BN7vDEp2J8BR+8rb
RemIxOMgldZ7YMguBSUjl4Hdfb33nr8t4mydROOYhQFyDiXn2WcRshEpTilmDC7o
+nLOTxm0s5Y//SfjOsmYqqmTrCZthvnm3QnEPfjNwTzcI+nLPDhZemEUWMGJQRs3
VsAY8EY7ErFxWkQ6ytQglygFKzYS2qwbR4EEW2g9cxh2Hqp4MKfWhDXBdktS0nVn
RHTrc5Ucihf7hje48if9k+GH5Bn/AJRj4HZg9wIDAQABAoIBAQCewK/yrtNyPHtA
zF4GUDhDr5BmV2de+YrRtyNzxIBG97+FkjkomYPEP4dVYfhIiNggaHlI+xtYUco/
+/rhPEQwVivNvOSJIj/OrLtqcxt8m1HREDk9uSADtgsZOzWCAkGAtCAozTXKAkUb
7bcBCxPEzTXJCESDPEL91aZMOynAJutrjvnk72oTh1QDNft8wvCubBZMXPW/ASeA
93ksuMNK/0rC5s+W9SAfNbZLXM83FFD6nECISCuyaflSVoUf/Iq7lMoFT/cPQQx7
yOO/Z4iDtRh1afrrI+SRSElGtNgWvn8CRQ2baNfBEMJxPRxmvsoYsHHkltnFAwzI
QaU+Ow55AoGBANTow69rTlrvMdWCnFyN5l4/OSSO88M16cvcpc9++jnxgYwBiVqR
ZbL21RgUca7Mv0h71hbTx1Szjd6b56yCeGgpBkgvnIewh4UVGH6DE4EVBbJvehKq
qY/7mTfnP2V0yuvD8mZdLsgcUwRsAVAcoTNJg59BEE0RUSgTvyS8uftNAoGBANRq
xVwHujsI/0Fw8dOH0Fnpj4C3b4Hy7DvnmQBktRPY/j7XBiRsoUQrKNF/GUR8MU2h
AekpH5EYAOyxElZ8opkrl9dync1ISuGcSvTfZhxFQsMqLGqjUwie/sb02MVrTlzN
rqsYIXwMOYUdfragxCo2LrADlozESdwa8nE/GANTAoGBALOfiPCnLF0Frlp+reJc
CT38Qv536GoKQywuBGnRq9RUz0HKThJt1ootOszs+phJvxus9osdsu7f6VIk4Ekn
beQasGUYXgwQP9VkXdzmQv9FOCA5cpjrCNEjqDmEzHSRG13bW9dGyGAlnVh+soDf
FpUJcZVbNCCA+qJAPqQE4TeFAoGAYEakfTnjZq6FQxv1/3InNRlvOV/q4nNAL0ah
SlRa3kwTsFe1KP0zero1Hf8FbMz9a7n2K7+/1vuSL7VyIlELQXFIuRXyAAEsL2Wv
c51topKh1+NdjlfIYrftsi93sMMaGWYALPwa8SZ5S6Q6snvbbWZxcsWtjs/IykJM
cJewQpECgYEAu1xntSg8B93ImPnlVis8VZtRzfqcfeTysS/hI3aqOGcMfCTIbdD3
0PYppNxGww7OvYXhk675TS7moWTsA7woCByG6u1MA7ZFyID7VKn6ahwKE7YYu2e4
QoNg9D8NGydSXC8us0rX6YjHXifdLPgQXD03zqJT/jOnkvR5p01tlR4=
-----END RSA PRIVATE KEY-----


)KEY";
