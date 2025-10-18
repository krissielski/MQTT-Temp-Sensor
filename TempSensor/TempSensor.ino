
//Waveshare ESP32-C3-Zero

//MQTT PubSubClient API: https://pubsubclient.knolleary.net/api

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "secrets.h"

#define RGB_PIN     10
#define GND_PIN     4
#define I2C_SCL_PIN 5
#define I2C_SDA_PIN 6


WiFiClientSecure wifiClient;
PubSubClient     mqttClient(wifiClient);



void setup() 
{
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while( WiFi.status() != WL_CONNECTED )
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected. IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");


  wifiClient.setInsecure();
  mqttClient.setServer(MQTT_URL, MQTT_PORT);


  if ( mqttClient.connect("sensor-01", MQTT_USER, MQTT_PASS) )
  {
    Serial.print("MQTT Connect Success: "); Serial.println( mqttClient.state() );
  }
  else
  {
    Serial.print("MQTT Connect FAIL: "); Serial.println( mqttClient.state() );   
  }

}

void loop() 
{

  uint32_t loopCount = 0;
  boolean  success;

  mqttClient.loop();

  while(1)
  {

    if( loopCount == 0 )
    {

      success = mqttClient.publish("sensors/office1/temperature","hello world");
      if( success )
      {
        Serial.println("++");
      }
      else
      {
        Serial.print("MQTT Publish failure: "); Serial.println( mqttClient.state() );
      }

      loopCount = 50;
    }

    rgbLedWrite(RGB_PIN, 25, 0, 0);  // Red
    delay(500);
    rgbLedWrite(RGB_PIN, 0, 25, 0);  // Green
    delay(500);
    rgbLedWrite(RGB_PIN, 0, 0, 25);  // Blue
    delay(500);

    Serial.print("Hello Kris  ");
    Serial.println(loopCount);

    loopCount--;

  }

  
  
 
}
