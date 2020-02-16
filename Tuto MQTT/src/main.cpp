#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>  //mqtt
#include <ArduinoJson.h>

//WiFi Connection configuration
char ssid[] = "ssidname";     //  your network SSID (name)
char password[] = "password";  // your network password
//mqtt server
char mqtt_server[] = "x.x.x.x;
WiFiClient espClient;
PubSubClient MQTTclient(espClient);

#define MQTT_USER ""    // si user existe
#define MQTT_PASS ""

//mqtt
void MQTTsend() {

  char buffer[512];
  DynamicJsonDocument docMqtt(512);
  docMqtt["temperature"] = "23";
  docMqtt["humidity"]= "40";
  docMqtt["notification"]= "il fait chaud !!!";
  size_t n = serializeJson(docMqtt, buffer);
  MQTTclient.publish("byfeel/sensor/state", buffer, n);

}

void MQTTconnect() {

  while (!MQTTclient.connected()) {
      Serial.print("Attente  MQTT connection...");
      String clientId = "NotifheureClient-";
      clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (MQTTclient.connect(clientId.c_str(),MQTT_USER,MQTT_PASS)) {
      Serial.println("connected");

      MQTTclient.subscribe("byfeel/mqtt/RX");

    } else {
      Serial.print("ECHEC, rc=");
      Serial.print(MQTTclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void MQTTcallback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message MQTT [");
  Serial.print(topic);
  Serial.print("] ");

//  payload[length] = '\0';
//  String s = String((char*)payload);

DynamicJsonDocument docMqtt(512);
deserializeJson(docMqtt, payload, length);
String msg , title;
msg=docMqtt["msg"]|"";
title=docMqtt["title"]|"";
Serial.println("message reçu : "+msg +" et titre recu "+title);

}

void setup() {
Serial.begin(115200);
  // Conexion WIFI
   WiFi.begin(ssid, password);
   Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   Serial.println("Connected");
   MQTTclient.setServer(mqtt_server, 1883);
   MQTTclient.setCallback(MQTTcallback);
}

void loop() {
  static uint32_t  lastTimeMqtt= 0;
  // connecte serveur MQTT
  if (!MQTTclient.connected()) {
    MQTTconnect();
  }

  MQTTclient.loop();
  if (millis() - lastTimeMqtt >= 10000)  // toutes les 20 secondes
   {
     lastTimeMqtt = millis();
     MQTTsend();
   }
}
