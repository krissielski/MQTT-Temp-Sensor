
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



void publish(void)
{
  static boolean firstPublish = true;

  Serial.println( "Publishing!");

  //Check if connected
  if( !mqttClient.connected() )
  {
    Serial.print( "Connecting... ");

    if ( mqttClient.connect("sensor-01", MQTT_USER, MQTT_PASS) )
    {
      Serial.print("Success: "); Serial.println( mqttClient.state() );
    }
    else
    {
      Serial.print("**FAIL**: "); Serial.println( mqttClient.state() );   
      return;
    }
  }


  //Is this the first publish?
  if( firstPublish )
  {
    mqttClient.publish("sensors/office1/online","True");
    firstPublish = false;
  }


  //ToDo: Get Temperature data here


  //Publish
  if( mqttClient.publish("sensors/office1/temperature","hello world") )
  {
    Serial.println("++");
  }
  else
  {
    Serial.print("MQTT Publish failure: "); Serial.println( mqttClient.state() );
  }


  //ToDo:  Should we disconnect after publish?

}




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

}


void loop() 
{

  //Counts are roughly in milliseconds
  static int ledCount     = 0;
  static int publishCount = 0;

  mqttClient.loop();


  //Publish every so often
  if( publishCount == 0 )
  {
    rgbLedWrite(RGB_PIN, 20, 0, 0);  // Green
    delay(5);
    publish();
    delay(5);
    rgbLedWrite(RGB_PIN, 0, 0, 0);  // Off

    publishCount = 20000;
  }


  //Blink LED
  if( ledCount == 10 )
  {
    rgbLedWrite(RGB_PIN, 0, 0, 5);  // Very dim Blue
  }
  if( ledCount == 0 )
  {
    rgbLedWrite(RGB_PIN, 0, 0, 0);  // Off
    ledCount = 5000;
 
    Serial.print(".");
  }


  delay(1);

  ledCount--;
  publishCount--;

}
