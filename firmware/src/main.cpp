#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


const char *resource = "https://status.hsp.sh/api/now";
const char* exthost = "https://status.hsp.sh";
HTTPClient http;
WiFiClientSecure client;

const uint8_t RED_PIN = D1;
const uint8_t GREEN_PIN = D2;
const uint8_t BLUE_PIN = D3;
static int brightness;
int fadeAmount = 5;

static unsigned char pos = 0;

StaticJsonDocument<1024> json;
static bool isOpen = true;
unsigned long lastRequest = 0;

bool getSpaceStatus()
{
  String payload = "";
  http.begin(client, resource);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK)    
  {
    payload = http.getString();

    http.end(); //Free the resources
    DeserializationError error = deserializeJson(json, payload);
    if (error != DeserializationError::Ok)
    {
      return false;
    }
    bool result = json["state"]["open"];
    return result;
  }
  http.end(); // Close connection
  return false;
}

void setColor(int r, int g, int b)
{
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
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

void setup()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  client.setInsecure(); //the magic line, use with caution
  client.connect(exthost, 433);
  isOpen = false;

  while (WiFi.status() != WL_CONNECTED)
  {
    pos += 10;
    Wheel(pos);
    delay(10);
  }
  isOpen = getSpaceStatus();
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED && millis() - lastRequest >= 1000 * 60)
  {
    isOpen = getSpaceStatus();
    lastRequest = millis();
  }
  pos++;
  if (isOpen)
  {
    Wheel(pos);
  }
  else
  {
    setColor(0, 0, 0);
  }

  delay(300); // delaying the speed of fading
}