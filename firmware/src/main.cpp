#include <Arduino.h>
#include <LeifHomieLib.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#else
#include "WiFi.h"
#endif

#include <ArduinoOTA.h>
#include <initializer_list>
#include <string>

#include "pinDefs.h"
#include "secrets.h"
#include "config.h"
#include "commonFwUtils.h"

HomieDevice homie;
HomieProperty *pPropRed = NULL;
HomieProperty *pPropGreen = NULL;
HomieProperty *pPropBlue = NULL;

static bool enable[3] = { true };

HomieProperty *pPropBrightness = NULL;
HomieProperty *pPropSpeed = NULL;

const uint8_t RED_PIN = D3;
const uint8_t GREEN_PIN = D2;
const uint8_t BLUE_PIN = D1;
static int brightness;
int fadeAmount = 5;

static unsigned char pos = 0;


void init_prop_color(HomieNode* pNode, HomieProperty* pProperty, int i, String id){
  pProperty = pNode->NewProperty();
  pProperty->strFriendlyName = "Color " + id;
  pProperty->strID = id;
  pProperty->SetRetained(true);
  pProperty->SetSettable(true);
  pProperty->datatype = homieBool;
  pProperty->SetBool(true);
  pProperty->strFormat = "";
  //callback is actually not needed cause buzzer is handled in loop
  pProperty->AddCallback([i](HomieProperty *pSource) {
  	Serial.printf("%s is now %s\n", pSource->strFriendlyName.c_str(),pSource->GetValue().c_str()); 
    enable[i] = strcmp(pSource->GetValue().c_str(), "true") == 0;
  });
}

void init_homie_stuff(HomieDevice* pHomie){
  pHomie->strFriendlyName = friendlyName;
  #if defined(APPEND_MAC_TO_HOSTNAME)
    char out[20];
    sprintf(out, "%s-%X",hostname, ESP.getChipId()); // WiFi.macAddress().c_str() is random
    pHomie->strID = out;
  #else
    pHomie->strID = hostname;
  #endif
  pHomie->strID.toLowerCase();

  pHomie->strMqttServerIP = fallbackMqttIp;
	pHomie->strMqttUserName = MQTT_USERNAME;
	pHomie->strMqttPassword = MQTT_PASSWD;
  pHomie->Init();

  Serial.print("\ninitialized ");
  Serial.println(pHomie->strID);

}

void setColor(int r, int g, int b)
{
  analogWrite(RED_PIN, enable[0] ? r : 0);
  analogWrite(GREEN_PIN, enable[1] ? g : 0);
  analogWrite(BLUE_PIN, enable[2] ? b : 0);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(unsigned char WheelPos)
{
  if (WheelPos < 85)
  {
    setColor(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    setColor(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    setColor(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(IOT_WIFI_NAME, IOT_WIFI_PASSWD);

  while (WiFi.status() != WL_CONNECTED) {
    static bool flag = false;
    digitalWrite(PIN_LED,flag);
    flag = !flag;

    Serial.print(".");
    delay(3000);    
  }
  Serial.print("\nconnected\n");


  // begin_hspota();    
  HomieNode *pNode = homie.NewNode();
  pNode->strID = "properties";
  pNode->strFriendlyName = "Properties";

  init_prop_color(pNode, pPropRed, 0, "red");
  init_prop_color(pNode, pPropGreen, 1, "green");
  init_prop_color(pNode, pPropBlue, 2, "blue");

  // Serial.print(pPropRed->GetValue());

  // pPropBlue->AddCallback([](HomieProperty *pSource) {
  // 	Serial.printf("%s is now %s\n",pSource->strFriendlyName.c_str(),pSource->GetValue().c_str()); 
  //   // enableBlue = pSource->GetValue().c_str() != "false";
  // });

  // init_buzzer(pNode,pPropBuzzer, "buzzer");  

  init_homie_stuff(&homie);  
}

void loop() {
  if(homie.IsConnected()){
    handle_io_pattern(PIN_LED,PATTERN_HBEAT);
  } else {
    handle_io_pattern(PIN_LED,PATTERN_ERR);
  }

  // ArduinoOTA.handle();
  homie.Loop();
  pos++;
  if (pos > 255) { pos = 0; }
  Wheel(pos);

  delay(100);
}
