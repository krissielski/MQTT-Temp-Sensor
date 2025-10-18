
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

#define DEVICE_NAME "sensor-01"

#define VERSION     "0.1.0"


WiFiClientSecure wifiClient;
PubSubClient     mqttClient(wifiClient);

uint32_t msgCount = 0;


String generateJsonData( void )
{

  static String jsonStr;

  jsonStr  = "{";      //Opening bracket

  jsonStr += "\"ts\":";
  jsonStr += time(nullptr);
  jsonStr += ",";

  jsonStr += "\"dev\":\"";
  jsonStr += DEVICE_NAME;
  jsonStr += "\",";

  jsonStr += "\"msg\":";
  jsonStr += msgCount;
  jsonStr += ",";

  jsonStr += "\"temp\":";
  jsonStr += 80.0;
  jsonStr += ",";

  jsonStr += "\"RH\":";
  jsonStr += 40.0;

  jsonStr += "}";      //Closing bracket

  msgCount++;

  return jsonStr;
}

String generateJsonInfo( void )
{

  static String jsonStr;

  jsonStr  = "{";      //Opening bracket

  jsonStr += "\"ts\":";
  jsonStr += time(nullptr);
  jsonStr += ",";

  jsonStr += "\"dev\":\"";
  jsonStr += DEVICE_NAME;
  jsonStr += "\",";

  jsonStr += "\"msg\":";
  jsonStr += msgCount;
  jsonStr += ",";

  jsonStr += "\"ver\":\"";
  jsonStr += VERSION;
  jsonStr += "\"";

  jsonStr += "}";      //Closing bracket

  msgCount++;

  return jsonStr;
}



void publish(void)
{
  static boolean firstPublish = true;

  Serial.println( "Publishing!");
  Serial.println( time(nullptr) );
  

  //Check if connected
  if( !mqttClient.connected() )
  {
    Serial.print( "Connecting... ");

    if ( mqttClient.connect(DEVICE_NAME, MQTT_USER, MQTT_PASS) )
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
    mqttClient.publish("sensors/office/info", generateJsonInfo().c_str() );
    firstPublish = false;
  }


  //ToDo: Get Temperature data here


  //Publish
  if( mqttClient.publish("sensors/office/data", generateJsonData().c_str() ) )
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

  //Get Time
  configTime(0,0,"pool.ntp.org", "time.nist.gov");

  Serial.print("Current Time:  ");
  Serial.println( time(nullptr) );

  msgCount = 0;

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

    publishCount = 30000;
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
