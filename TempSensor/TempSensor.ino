// MQTT Temperature Sensor
//   Waveshare ESP32-C3-Zero
//
// Required Libraries:
//    PubSubClient
//    Adafruit SHT31
//
//MQTT PubSubClient API: https://pubsubclient.knolleary.net/api
//

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include "secrets.h"
#include "config.h"

#define RGB_PIN     10
#define GND_PIN     0
#define I2C_SCL_PIN 1
#define I2C_SDA_PIN 2

#define VERSION     "1.0.0"

#define JAN1_2025   1735689600    // Unix time for Jan 1, 2025

#define PUBLISH_PERIOD  (5 * 60)  // in seconds


WiFiClientSecure wifiClient;
PubSubClient     mqttClient(wifiClient);

Adafruit_SHT31   sht31 = Adafruit_SHT31();

uint32_t msgCount = 0;


String generateTopic( const char* topic )
{
  static String topicStr;

  // Format: root/site/area/sensor-type/deviceID/topic
  topicStr  = "iot";
  topicStr += "/";
  topicStr += MQTT_SITE;
  topicStr += "/";
  topicStr += MQTT_AREA;
  topicStr += "/";
  topicStr += MQTT_TYPE;
  topicStr += "/";
  topicStr += DEVICE_NAME;
  topicStr += "/";
  topicStr += topic;

  return topicStr;
}


String generateJsonData( float temperature, float humidity )
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
  jsonStr += String(temperature,1);
  jsonStr += ",";

  jsonStr += "\"RH\":";
  jsonStr += String(humidity,1);

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


float getRH()
{
  float rh = sht31.readHumidity();

  if (isnan(rh)) 
  {
    return -1.0;
  }
  return rh;
}


float getTemp()
{
  float tc = sht31.readTemperature(); // Celsius

  if (isnan(tc))
  {
    return -1.0;
  }

  //Convert C to F
  float tf = (tc * 9.0f / 5.0f) + 32.0f;
  return tf;
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
    mqttClient.publish(generateTopic("info").c_str(), generateJsonInfo().c_str() );
    firstPublish = false;
  }


  //ToDo: Get Temperature data here
  float tmp = getTemp();
  float rh  = getRH();

  Serial.println("Temp = " + String(tmp) + "    RH = " + String(rh) );

  //Publish
  if( mqttClient.publish(generateTopic("data").c_str(), generateJsonData(tmp,rh).c_str() ) )
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

  //Create Virtual Gnd
  pinMode(GND_PIN, OUTPUT); 
  digitalWrite(GND_PIN, LOW);

  rgbLedWrite(RGB_PIN, 0, 20, 0);  // Red, indicating startup

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

  //Enable NT sync
  configTime(0,0,"pool.ntp.org", "time.nist.gov");

  //wait until NTP syncs
  while(true)
  {
    time_t now = time(nullptr);
    if( now > JAN1_2025 )
    {
      break;
    }
    delay(500);
    Serial.print("*");
  }

  Serial.println("");
  Serial.print("Current Time:  ");
  Serial.println( time(nullptr) );


  //Initialize SHT31
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  if( !sht31.begin(0x44) ) 
  {
    Serial.println(" ** SHT31 ERROR **");
    while (1){ delay(1); }
  }
  Serial.println("Found SHT31");

  rgbLedWrite(RGB_PIN, 0, 0, 0);  // Off, startup successful

  msgCount = 0;

}


void loop() 
{

  static int    ledCount        = 0;  //Count roughly in milliseconds
  static time_t nextPublishTime = 0;  //time in seconds
  time_t        currTime;


  //Process MQTT messages
  mqttClient.loop();

  //Get current time
  currTime = time(nullptr); 

  //Publish every so often
  if( currTime >= nextPublishTime )
  {
    rgbLedWrite(RGB_PIN, 20, 0, 0);  // Green
    delay(5);
    publish();
    delay(5);
    rgbLedWrite(RGB_PIN, 0, 0, 0);  // Off

    nextPublishTime = currTime + PUBLISH_PERIOD;
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
  }


  delay(1);

  ledCount--;

}
