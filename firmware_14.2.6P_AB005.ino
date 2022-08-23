#include <ESP8266WiFi.h> //2.7.4 
#include <time.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

// Parallel
#define ARDUINOJSON_USE_LONG_LONG 1 
#include <PubSubClient.h>
//

#include "WiFiManager.h"
#include "PrintLogger.h"
#include "MQTTSecureClient.h"
#include "MQTTErrors.h"
#include "HexString.h"
#include "SimpleStorage.h"
#include "SimpleTask.h"
#include "PixelController.h"
#include "TempController.h"
#include "TimeManager.h"

#include "LockTable.h"
#include "config.h"
#include "LED_status.h"

#include "OTA.h"

//PrintLogger Logger(&Serial); //Enables Logging for testing Do not deploy this line
PrintLogger Logger; //Disables Logging for deployment

//GLOBALS
byte mac[6];
const char* NTP_SERVER = "ch.pool.ntp.org";


Adafruit_NeoPixel Pixels(CONFIG_LED_NUMPIXELS, CONFIG_LED_PIN, NEO_GRB + NEO_KHZ800);
PixelController MyPixelController(Pixels);

TempController MyTempController(CONFIG_ONE_WIRE_BUS, CONFIG_RELAY_PIN);

MQTTSecureClient MQTT(CONFIG_CA_CERT, CONFIG_THING_CERT, CONFIG_THING_PKEY);

const size_t MQTT_TOPIC_MAX_LENGTH = 256;

TimeManager GlobalTimeManager; // Used to do useful work while delay.
WiFiManager GlobalWiFiManager;
SimpleStorage MqttTopicStorage(MQTT_TOPIC_MAX_LENGTH);
SimpleTask MqttHeartbeatTask;

//CONSTS FOR PARALLEL NETWORK -->
const char* mqttServer = "143.244.214.93";
const int mqttPort = 1883;
const char* mqttUser = "YourMqttUser";
const char* mqttPassword = "YourMqttUserPassword";

uint64_t chipid = ESP.getChipId();
char array[64];
int myInteger = chipid; 

WiFiClient espClient;
PubSubClient client(espClient);
// <--Parallel

void(* resetFunc) (void) = 0;

String getMqttExtraTopic(const String &postfix)
{
  return MQTT_MAIN_TOPIC + "/" + postfix;
}

void MqttTopicStore(const String &topic)
{
  MQTT_MAIN_TOPIC = topic;
  if (MqttTopicStorage.store(topic))
    Logger.println("MQTT topic successfully stored to persistent storage!");
  else
    Logger.println("Failed to store MQTT topic to persistent storage!");
}

void MqttTopicRestore()
{
  auto topic = MqttTopicStorage.restore();
  if (topic.length())
  {
    MQTT_MAIN_TOPIC = topic;
    Logger.println("MQTT topic loaded");
  }
  else
    Logger.println("Persistent storage is empty!");
}

void NTPsetup() {
  configTime(0, 0, NTP_SERVER);
  setenv("TZ", TZ_INFO, 1);
}

void NTPConnect(unsigned long delayMillis = 500)
{
  Logger.print("NTP connecting ...");
  static const time_t reference = 1234567890;
  //configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  int now = time(nullptr);
  while (now < reference)
  {
    Logger.print(".");
    GlobalTimeManager.delayMillis(delayMillis);
    now = time(nullptr);
  }

  Logger.println(" connected!");
  CompTime = now;
}

void WiFiManagerSaveParamCallback()
{
  if (GlobalWiFiManager.server->hasArg("mqtt_topic"))
  {
    auto topic = GlobalWiFiManager.server->arg("mqtt_topic");
    MqttTopicStore(topic);
  }
  Logger.println("MQTT topic set to: " + MQTT_MAIN_TOPIC);
}

bool WiFiIsConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

bool WiFiRetry(unsigned long delayMillis = 500, size_t times = 100)
{
  auto result = false;

  auto enableConfigPortal = GlobalWiFiManager.getEnableConfigPortal();
  GlobalWiFiManager.setEnableConfigPortal(false);
  if (GlobalWiFiManager.autoConnect(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD))
  {
    result = true;
  }
  else
  {
    Logger.println("Failed to auto-connect. Waiting for connection (" + String(delayMillis * times / 1000) + " seconds) ");
    for (size_t i = 0; i < times; i++)
    {
      Logger.print(".");
      GlobalTimeManager.delayMillis(delayMillis);
      if (WiFiIsConnected())
      {
        result = true;
        Logger.println(" connected!");
        break;
      }
    }
    if (!result)
    {
      Logger.println(" skipped");
    }
  }

  GlobalWiFiManager.setEnableConfigPortal(enableConfigPortal);

  return result;
}

void WiFiSetup()
{
  WiFi.mode(WIFI_STA);

  WiFiManagerParameter mqttTopicParam("mqtt_topic", "Mqtt main topic", MQTT_MAIN_TOPIC.c_str(), MQTT_TOPIC_MAX_LENGTH);

  //GlobalWiFiManager.setDebugOutput(true); // uncomment this line to enable WiFiManager's debug prints
  GlobalWiFiManager.addParameter(&mqttTopicParam);
  GlobalWiFiManager.setSaveParamsCallback(WiFiManagerSaveParamCallback);

  GlobalTimeManager.delayMillis(500);

  while (true)
  {
    if (GlobalWiFiManager.getWiFiIsSaved())
    {
      Logger.println("A saved network was found. Trying to connect to " + GlobalWiFiManager.getWiFiSSID(true));
      for (size_t i = 0; i < 1; i++)
      {
        if (WiFiRetry(300, 100))
          return;
      }
    }else{

         WiFi.begin("LiamsHotspot","keepcool");

         int retries = 0;
          while ((WiFi.status() != WL_CONNECTED) && (retries < 15)) {
             retries++;
             delay(500);
             Serial.print(".");
          }
          if (retries > 14) {
              Serial.println(F("WiFi connection FAILED"));
          }
          if (WiFi.status() == WL_CONNECTED) {
              Serial.println(F("WiFi connected!"));
              Serial.println("IP address: ");
              Serial.println(WiFi.localIP());
          }
    Serial.println(F("Setup ready"));
   }

    Logger.println("Proceed to WiFi Configuration Manager");
    WipeOrange();
    GlobalWiFiManager.setConfigPortalTimeout(120);
    if (GlobalWiFiManager.autoConnect(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD))
      return;
    else
      Logger.println("Failed to connect");
  }
}

void WiFiConnect(unsigned long delayMillis = 500)
{
  Logger.println("WiFi restoring connection");
  while (!WiFiIsConnected())
    WiFiRetry(delayMillis, 100);
}

void MQTTSetup()
{
  MQTT.begin(CONFIG_MQTT_HOST, CONFIG_MQTT_PORT);
  MQTT.onMessage(MQTTMessageReceived);
}

void MQTTConnect(unsigned long delayMillis = 500)
{
  Logger.print("MQTT connecting ...");

  while (!MQTT.connected() && WiFiIsConnected())
  {
    if (MQTT.connect(CONFIG_THING_NAME))
    {
      Logger.println(" connected!");
      auto success = true;
      success = success && MQTT.subscribe(MQTT_MAIN_TOPIC);
      success = success && MQTT.subscribe(getMqttExtraTopic("ping"));
      success = success && MQTT.subscribe(getMqttExtraTopic("update"));
      success = success && MQTT.subscribe(getMqttExtraTopic("minTemp"));
      success = success && MQTT.subscribe(getMqttExtraTopic("maxTemp"));


      if (success)
        Logger.println("Subscribed to: " + MQTT_MAIN_TOPIC + " and extra topics");
      else
        Logger.println("Subscribe failed: " + getError(MQTT.lastError()));
      break;
    }
    else
    {
      Logger.println(" failed!");
      Logger.println("SSL Error Code: " + String(MQTT.getClient()->getLastSSLError()));
      Logger.println("MQTT Reason: " + getError(MQTT.returnCode()));
      Logger.print("Retrying ");
    }

    Logger.print(".");
    GlobalTimeManager.delayMillis(delayMillis);
  }
}

void MQTTMessageReceived(String &topic, String &payload)
{
  Logger.println("Recieved [" + topic + "]: " + payload);

  if (topic.equals(MQTT_MAIN_TOPIC))
  {
    auto hexPayload = payload;
    hexPayload.replace("x", "");
    HexString hexCommand(hexPayload);

    const auto bytesLength = hexCommand.safeLength() / 2;
    unsigned char bytesCommand[bytesLength];
    hexCommand.convertToByteArray(bytesCommand);

    Logger.println("Command in Hex: " + hexCommand);
    Logger.println("Bytes Length: " + String(bytesLength));

    Serial.write(bytesCommand, bytesLength);

    //blink lights to show successful unlock
    //blink1();
    MyPixelController.setupLock(0, 28).attachLock(19, 100);
    //MyPixelController.setupWave(0, 30).attachWave(5, 1050);
  }

  else if (topic.equals(getMqttExtraTopic("ping")))
  {
    Logger.println("Ping message received: " + payload);
    if (MQTT.publish(getMqttExtraTopic("pong"), payload))
      Logger.println("Pong message published: " + payload);

    else
      Logger.println("Failed to publish pong message");
  }

    else if (topic.equals(getMqttExtraTopic("update")))
  {
    Logger.println("Update message received: " + payload);
    WipeTeal();
    blink2();
    blink2();
     if (MQTT.publish(getMqttExtraTopic("updating firmware"), payload))
      Logger.println("Updating message published: " + payload);

    else
      Logger.println("Failed to publish update message");
    enterOTA();
      
  }
  
    else if (topic.equals(getMqttExtraTopic("minTemp")))
  {
    Logger.println("minTemp message received: " + payload);
    String floatTemp = payload;
        minTemp = floatTemp.toFloat();
        MyTempController.setup(minTemp, maxTemp, Logger);
    if (MQTT.publish(getMqttExtraTopic("minTemp set"), payload))
      Logger.println("Set minTemp message published: " + payload);

    else
      Logger.println("Failed to publish minTemp message");
  }
  
    else if (topic.equals(getMqttExtraTopic("maxTemp")))
  {
    Logger.println("maxTemp message received: " + payload);
    String floatTemp = payload;
        maxTemp = floatTemp.toFloat();
        MyTempController.setup(minTemp, maxTemp, Logger);
    if (MQTT.publish(getMqttExtraTopic("maxTemp set"), payload))
      Logger.println("Set maxTemp message published: " + payload);

    else
      Logger.println("Failed to publish maxTemp message");
  }
  else
  {
    Logger.println("The handler is not described for this topic");
  }
}

void MQTTHeartbeatPublish(int iteration)
{
  if (MQTT.connected())
  {
    auto heartbeatTopic = MQTT_MAIN_TOPIC + "/heartbeat";
    String heartbeatMessage;
    String Compbinary;
    //String minmaxTemp = String(minTemp)+ "," + String(maxTemp); 
    if (CompState == 0)
    {Compbinary = "Off";}
    if (CompState == 1)
    {Compbinary = "On";}
    if (CompState == 2)
    {Compbinary = "Rest";}
    StaticJsonDocument<200> messageDoc;
    messageDoc["Version"]= Version;
    //messageDoc["Time"]= TempTime;
    messageDoc["Temp"] = Temp;
    //messageDoc["minmaxTemp"] = minmaxTemp;
    messageDoc["minTemp"] = minTemp;
    messageDoc["maxTemp"] = maxTemp;
    //messageDoc["CompState"] = CompState;
    messageDoc["Comp " + Compbinary] = CompTime;
    //messageDoc["Comp"] = Compbinary;
    serializeJson(messageDoc, heartbeatMessage);
    MQTT.publish(heartbeatTopic, heartbeatMessage );
  }
}

// Parallel -->
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
   message+=((char)payload[i]);
  }
    //int LocalTime = getNTPtime();
    //int LT2 = time(nullptr);
    char tempMessage[64];
    sprintf( tempMessage, "%f,%i,%i,%i", Temp,TempTime,CompState,CompTime);
    //Serial.print(message);
    if(message[0] == 'Q'){
      char array2[64];
        array2[0] = 'F';
        for(int i = 1; i < 64; i++){
          array2[i] = array[i-1];
        }
        client.publish(array2,tempMessage);
      
      }

else if(message[0] == 'R'){
      resetFunc();
    }
      
    else{
          //Serial.print(message);
        HexString hexCommand(message);
        const auto bytesLength = hexCommand.safeLength() / 2;
        unsigned char bytesCommand[bytesLength];
        hexCommand.convertToByteArray(bytesCommand);
        
        hexCommand.convertToByteArray(bytesCommand);
        
        //Serial.println("Command in Hex: " + hexCommand);
        //Serial.println("Bytes Length: " + String(bytesLength));
        Serial.write(bytesCommand, bytesLength);
        char array2[64];
        array2[0] = 'F';
        for(int i = 1; i < 64; i++){
          array2[i] = array[i-1];
        }
        //String response; // for incoming serial data
        
        //response = Serial.read();
    
        //Serial.println(response);
    }  
    
    }   

void ParallelReconnect ()
{    
while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    sprintf( array, "%d", myInteger );
    if (client.connect(array, array, mqttPassword )) {
     
      //Serial.println("connected");
      //Serial.println(array);
      client.publish("chipdIDtracker",array);
      client.subscribe(array);  

    } else {

      //Serial.print("failed with state ");
      //Serial.print(client.state());
      delay(2000);

    }
  } 
}
//Parallel <--

void setup()
{
  Serial.begin(9600);

  NTPsetup();
  
  Logger.println("");

  // Adding tasks to execute during the delay.
  GlobalTimeManager.setup([&]() {
    MyTempController.loop();
  });

  MqttHeartbeatTask.setup(MQTTHeartbeatPublish)->attach(60000);
  MyTempController.setup(minTemp, maxTemp, Logger); // Temperature relay configuration, parameters: minTemp, maxTemp, system logger
  MyTempController.attach(5000);        // check interval is indicated here

  pinMode(LED, OUTPUT); //For blink notification

  //DISPLAY CHIP ID AND MAC ADDRESS
  WiFi.macAddress(mac);
  Serial.println();
  Serial.println("Serial Numbers");
  Serial.print(ESP.getChipId());
  Serial.print(",");
  Serial.print(WiFi.macAddress());
  Serial.print(",");

  //PRINTESPID
  WipeTeal();
  Serial.print(mac[3], HEX);
  Serial.print(mac[4], HEX);
  Serial.println(mac[5], HEX);
//  MqttTopicRestore();
  WiFiSetup();
  WiFiConnect();
  WipeYellow();
  NTPConnect();
  WipeGreen();
  WipeBlue();
  WipeBlack();

//PARALLEL NETWORK  -->
  client.setCallback(callback);
  client.setServer(mqttServer, mqttPort);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    sprintf( array, "%d", myInteger );
    if (client.connect(array, array, mqttPassword )) {
     
      //Serial.println("connected");
      //Serial.println(array);
      client.publish("chipdIDtracker",array);
      client.subscribe(array);  

    } else {

      //Serial.print("failed with state ");
      //Serial.print(client.state());
      delay(2000);

    }
  } 
// Parallel <-- 
  
  // The setup functions has 2 parameters: the index of the sub-strip and the number of LEDs. You can only call it once during configuration.
  // The attach functions has 2 parameters: frame switching time and number of played frames. Call this every time the animation needs to be played.
  // PixelController relies on the loop function not to be blocked.
//  MyPixelController.setupLock(0, 12).attachLock(10, 1000);
//  MyPixelController.setupWave(13, 30).attachWave(1, 2000);
}

void loop()
{ if (!client.connected())
  { 
    WiFiConnect();
    ParallelReconnect();
  }

  client.loop();// Parallel
  
  MyPixelController.loop();
  MyTempController.loop();
  
}
