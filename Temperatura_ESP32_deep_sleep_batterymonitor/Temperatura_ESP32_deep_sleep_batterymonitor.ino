#include <DHTesp.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

DHTesp dht;

const char* ssid = "UNO_2EDA3B";
const char* password = "550DA49101";
const char* mqtt_server = "192.168.34.111";

long lastMsg = 0;

#define TOPIC "Ambiente"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */

WiFiClient espClient;
PubSubClient client(espClient);

void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      //delay(5000);
    }
  }
}


void setup(){
  Serial.begin(9600);

  
  //dht.setup(17, DHTesp::DHT11);

  WiFi.begin(ssid, password);

  client.setServer(mqtt_server, 1883);
  }

  
void loop(){
  //ComfortState cf;
  
  //TempAndHumidity newValues = dht.getTempAndHumidity();
  //float heatIndex = dht.computeHeatIndex(newValues.temperature, newValues.humidity);
  //float dewPoint = dht.computeDewPoint(newValues.temperature, newValues.humidity);
  //float cr = dht.getComfortRatio(cf, newValues.temperature, newValues.humidity);

  /*String comfortStatus;
  switch(cf) {
    case Comfort_OK:
      comfortStatus = "OK";
      break;
    case Comfort_TooHot:
      comfortStatus = "Troppo caldo";
      break;
    case Comfort_TooCold:
      comfortStatus = "Troppo freddo";
      break;
    case Comfort_TooDry:
      comfortStatus = "Troppo secco";
      break;
    case Comfort_TooHumid:
      comfortStatus = "Troppo umido";
      break;
    case Comfort_HotAndHumid:
      comfortStatus = "Troppo caldo e umido";
      break;
    case Comfort_HotAndDry:
      comfortStatus = "Troppo caldo e secco";
      break;
    case Comfort_ColdAndHumid:
      comfortStatus = "Troppo freddo e umido";
      break;
    case Comfort_ColdAndDry:
      comfortStatus = "Troppo freddo e secco";
      break;
    default:
      comfortStatus = "Unknown:";
      break;
  };*/

  //float temperatura = newValues.temperature;
  //float umidita = newValues.humidity;

  float temperatura = 25;
  float umidita = 85;
  int batteryStatus = analogRead(34);
  //4095)*2*3.3*1.1;

  

  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
 
  JSONencoder["Temperatura"] = temperatura;
  JSONencoder["Umidita"] = umidita;
  //JSONencoder["Stato"] = comfortStatus;
  JSONencoder["Batteria"] = batteryStatus;
 
  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" Umidita: ");
  //Serial.println(newValues.humidity);
  Serial.print(umidita);
  Serial.print("  Batteria: ");
  Serial.println(batteryStatus);
  if (!client.connected()) {
    mqttconnect();
  }

  client.loop();


//    snprintf (dati, 20, "%lf", Dati);
    client.publish(TOPIC, JSONmessageBuffer); 
    Serial.println("Mandato");
  
  
  //delay(20000);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
