#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#define NUM_UPDATE_RETRIES 20

String overTheAirURL;

void enterOTA() {
	
	
	String chipid = String(ESP.getChipId());

  
  t_httpUpdate_return ret = ESPhttpUpdate.update("http://arcticshelf-public-ota.s3.amazonaws.com/"+chipid+".bin");
switch(ret) {
    case HTTP_UPDATE_FAILED:
         Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
        }
}
