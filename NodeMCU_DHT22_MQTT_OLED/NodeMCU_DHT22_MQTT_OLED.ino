
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<dht.h>
#include <Wire.h>
#include "OLED.h"




dht DHT;
OLED display(14, 12);
// Define NodeMCU D3 pin to as temperature data pin of  DHT22
// per ESP01 mettere solo 2 per ESP12 D2
#define DHT22_PIN D2

// Update these with values suitable for your network.
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Command is : [");
  Serial.print(topic);
  int p =(char)payload[0]-'0';
  int chk = DHT.read22(DHT22_PIN);
  // if MQTT comes a 0 message, show humidity
  if(p==0) 
  {
    Serial.println("to show humidity!]");
    Serial.print(" Humidity is: " );
    Serial.print(DHT.humidity, 1);
    Serial.println('%');
  } 
  // if MQTT comes a 1 message, show temperature
  if(p==1)
  {
  // digitalWrite(BUILTIN_LED, HIGH);
    Serial.println(" is to show temperature!] ");
  int chk = DHT.read22(DHT22_PIN);
   Serial.print(" Temp is: " );
   Serial.print(DHT.temperature, 1);
   Serial.println(' C');
  }
  Serial.println();
} //end callback

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe("DHT22");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 10 seconds");
      // Wait 10 seconds before retrying
      delay(10000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
 // setup_wifi();
    // Initialize display
  display.begin();
    // Test display ON
  //display.on();
  delay(3*1000);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  int chk = DHT.read22(DHT22_PIN);
  Serial.print(" Starting Humidity: " );
  Serial.print(DHT.humidity, 1);
  Serial.println('%');
  Serial.print(" Starting Temparature ");
  Serial.print(DHT.temperature, 1);
  Serial.println('C');
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  // read DHT11 sensor every 10 seconds
  if (now - lastMsg > 10000) {
     lastMsg = now;
     int chk = DHT.read22(DHT22_PIN);
 //    String msg="real time temperature: ";
     String msg="";
     msg= msg+ DHT.humidity;
    // msg = msg+" C ;real time Humidity: " ;
    // msg=msg+DHT.humidity ;
   //  msg=msg+"%";
     char message[58];
     msg.toCharArray(message,58);
     Serial.print("Umidità= ");
     Serial.print(message);
     //publish sensor data to MQTT broker
   client.publish("DHT22HUMI02", message);
           String msgT=" ";
        msgT=msgT+ DHT.temperature;
             char messageT[58];
     msgT.toCharArray(messageT,58);
     Serial.print("  Temperatura= ");
     Serial.println(messageT);
    client.publish("DHT22TEMP02", messageT);
  
  display.clear();
  //display.print("Temp");
  delay(500);
 display.print(messageT,3);
  //display.print("Hum=", 5);
display.print(message,4);
  }
}
