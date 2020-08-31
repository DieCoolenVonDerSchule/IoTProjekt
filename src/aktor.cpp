#include <WiFi.h>
#include <PubSubClient.h>
#include "TM1637.h" 
 
const char* SSID = "";
const char* PSK = "";
const char* MQTT_BROKER = "hivemq.dock.moxd.io";
int8_t TimeDisp[] = {0x00,0x00,0x00,0x00};
 
TM1637 tm1637(18,19);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int alarmValue = 0;
 
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);
 
    WiFi.begin(SSID, PSK);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Received message [");
    Serial.print(topic);
    Serial.print("] ");
    char msg[length+1];
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        msg[i] = (char)payload[i];
    }
    Serial.println();
 
    msg[length] = '\0';
    Serial.println(msg);
 
    String command = String(msg).substring(0, 6);
    String value = String(msg).substring(6);


    Serial.println("COMMAND:");
    Serial.println(command);

    Serial.println("VALUE");
    Serial.println(value);

    Serial.println("LENGTH:");
    String dunno = String((int) value.length());
    Serial.println(dunno.c_str());

    if(value.length()==4){
        TimeDisp[0] = 0;
        TimeDisp[1] = value.substring(0, 1).toInt();
        TimeDisp[2] = value.substring(2, 3).toInt();
        TimeDisp[3] = value.substring(3, 4).toInt();
    } else if(value.length()==5){
        TimeDisp[0] = value.substring(0, 1).toInt();
        TimeDisp[1] = value.substring(1, 2).toInt();
        TimeDisp[2] = value.substring(3, 4).toInt();
        TimeDisp[3] = value.substring(4, 5).toInt();
    } else {
        TimeDisp[0] = 9;
        TimeDisp[1] = 9;
        TimeDisp[2] = 9;
        TimeDisp[3] = 9;
    }

    if(strcmp(command.c_str(),"ALARM;")==0 && alarmValue==0){
        alarmValue = 5;
    }
}

void setup() {
    pinMode(16, OUTPUT);
    pinMode(14, OUTPUT);

    pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);

    tm1637.set();
    tm1637.init();

    Serial.begin(115200);
    setup_wifi();
    client.setServer(MQTT_BROKER, 1883);
    client.setCallback(callback);
}
 
void reconnect() {
    while (!client.connected()) {
        Serial.print("Reconnecting...");
        if (!client.connect("AlarmClient")) {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
    
    client.subscribe("kollisions_alarm");
    Serial.println("MQTT Connected...");
}
 
void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    if(alarmValue>0){

        Serial.write("Test");
        digitalWrite(16, HIGH);
        digitalWrite(14, HIGH);
        tm1637.display(TimeDisp);

        delay(500);

        digitalWrite(16, LOW);
        digitalWrite(14, LOW);
        tm1637.clearDisplay();
        delay(500);

        alarmValue--;
    }
}