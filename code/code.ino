```#include <PubSubClient.h>

#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTPIN 5     // Digital pin  #2 on ESP8266
#define DHTTYPE DHT22 // Sensor type
#define mqtt_server "192.168.1.4"
#define mqtt_user "pi"
#define mqtt_password "raspberry"
#define TOPIC "ESIEA/grp1"

DHT dht(DHTPIN, DHTTYPE);

float humidity,temperature_C;

WiFiClient espClient;
PubSubClient client(espClient);

// Network credentials
const char* ssid = "TheLabIOT";
const char* password = "Yaay!ICanTalkNow";

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Configuration de la connexion au serveur MQTT
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);  //La fonction de callback qui est executée à chaque réception de message

  dht.begin();
}

//Reconnexion
void reconnect() {
  //Boucle jusqu'à obtenir une reconnexion
  while (!client.connected()) {
    Serial.print("Connexion au serveur MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("KO, erreur : ");
      Serial.print(client.state());
      Serial.println(" On attend 5 secondes avant de recommencer");
      delay(5000);
    }
  }
}

void loop() {
  delay(2000);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  temperature_C = dht.readTemperature();  
  humidity = dht.readHumidity();

  // Check for error reading
  if (isnan(humidity) || isnan(temperature_C)) {
    Serial.println(" DHT reading failed ");
    return;
  }

  Serial.print("Temperature : ");
  Serial.print(temperature_C);
  Serial.print(" | Humidite : ");
  Serial.println(humidity);

  String data = "{\"temperature\":\"" + String(temperature_C) +"\",\"humidity\":\"" + String(humidity) + "\"}";

  client.publish(TOPIC, String(data).c_str(), true);
}```