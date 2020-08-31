#include "BMI088.h"
#include "WiFi.h"
#include <PubSubClient.h>
 
const char* SSID = "MustiNetz";
const char* PSK = "Spongebob040!";
const char* MQTT_BROKER = "hivemq.dock.moxd.io";
 
bool tester = true;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

float ax = 0, ay = 0, az = 0;
float gx = 0, gy = 0, gz = 0;
int16_t temp = 0;

const int schwellwert = 1;
 
void setup(void)
{

    pinMode(16, OUTPUT);
    pinMode(17, OUTPUT);

    Wire.begin();
    Serial.begin(115200);
 
    Serial.println("connection to Wifi");
    WiFi.begin(SSID, PSK);
    client.setServer(MQTT_BROKER, 1883);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    while(!Serial);

    while(1)
    {
        if(bmi088.isConnection())
        {
            bmi088.initialize();
            Serial.println("BMI088 is connected");
            break;
        }
        else Serial.println("BMI088 is not connected");
 
        delay(2000);
    }
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Reconnecting...");
        if (!client.connect("KollisionsClient")) {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}

void loop(void)
{ 

  if (!client.connected()) {
        reconnect();
    }
  client.loop();

  bmi088.getGyroscope(&gx, &gy, &gz);
 
  Serial.print(gx/1000);
  Serial.print(",");
  Serial.print(gy/1000);
  Serial.print(",");
  Serial.print(gz/1000);




if(gx/1000>schwellwert|| gx/1000<-schwellwert){
    String temp = "ALARM;" + String(abs(gx/1000));

    client.publish("kollisions_alarm", temp.c_str());
    Serial.print("ALARM.");

  delay(5000);
  } 

  if(gy/1000>schwellwert|| gy/1000<-schwellwert ){
    String temp = "ALARM;" + String(abs(gy/1000));

    client.publish("kollisions_alarm", temp.c_str());
    Serial.print("ALARM.");

   delay(5000);
  } 

  if(gz/1000>schwellwert|| gz/1000<-schwellwert ){
    String temp = "ALARM;" + String(abs(gz/1000));

    client.publish("kollisions_alarm", temp.c_str());
    Serial.print("ALARM.");

    delay(5000);
  }
 
  Serial.println();
  delay(50);
}