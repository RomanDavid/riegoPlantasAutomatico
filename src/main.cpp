#include "config.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>

#define SENSOR_LUZ D1
#define SENSOR_AGUA D2
#define SENSOR_HUMEDAD D3
#define LED_AMARILLO D4
#define LED_VERDE D5
#define LED_AZUL D6
#define DHTTYPE DHT22
DHT dht(SENSOR_HUMEDAD, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Buscando red WiFi..");
  }
  Serial.println("Conectado a la red WiFi");
  client.setServer(mqttServer, mqttPort);

  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(SENSOR_LUZ, INPUT);
  pinMode(SENSOR_AGUA, INPUT);
  pinMode(SENSOR_HUMEDAD, INPUT);
  dht.begin();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Buscando MQTT...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("Conectado a MQTT");
    } else {
      Serial.print("Fallo al conectar por MQTT: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void sendTopic(String sensor, String valor) {
  if (!client.connected()) {
    reconnect();
  }

  String mqtt_message = ("data,Sensor="+sensor+" Valor=" + valor);
  if (client.publish(database, mqtt_message.c_str(), false )) {
    Serial.println("Publish ok");
  } else {
    Serial.println("Publish failed");
  }
}

void loop() {
  digitalWrite(LED_VERDE, HIGH);

  //Humedad en aire
  float humedad = dht.readHumidity();
  sendTopic("Humedad_Aire",String(humedad, 2));

  //Temperatura aire
  float temperatura = dht.readTemperature();
  sendTopic("Temperatura_Aire",String(temperatura, 2));

  int sensorLuz = digitalRead(SENSOR_LUZ);
  if (sensorLuz == HIGH) {
    digitalWrite(LED_AMARILLO, HIGH);
  } else  if (sensorLuz == LOW) {
    digitalWrite(LED_AMARILLO, LOW);
  }
  sendTopic("LUZ",String(sensorLuz,2));

  int humedadTierra = digitalRead(SENSOR_AGUA);
  String sensor_humedadTierra = "";
  if (humedadTierra == 1) {
    digitalWrite(LED_AZUL, HIGH);
  } else if (humedadTierra == 0) {
    digitalWrite(LED_AZUL, LOW);
  }
  sendTopic("Humedad_Tierra",String(humedadTierra,2));
  

  delay(5000);
  digitalWrite(LED_VERDE, LOW);
  delay(5000);
}
