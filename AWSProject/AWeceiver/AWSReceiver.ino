#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
#define BUFFER_LEN 256
char msg[BUFFER_LEN];
char mac_Id[18];
byte mac[6];
int FLOATING_SENSOR = 4;

const char SUBSCRIBE_TOPIC[] = "WATER_TANK_STATUS";
const char PUBLISH_TOPIC[] = "iot/WATER_TANK";

//const char SUBSCRIBE_TOPIC[] = "WATER_TANK";
//const char PUBLISH_TOPIC[] = "WATER_TANK_STATUS";

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("WiFi Connected...!");

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  client.subscribe(SUBSCRIBE_TOPIC);

  // Create a message handler
  client.setCallback(messageHandler);
  
  Serial.println("AWS IoT Connected!");
}

void publishMessage() {
  WiFi.macAddress(mac);
  snprintf(mac_Id, sizeof(mac_Id), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print(mac_Id);
  //status="full";
  float t = 36.2;
  float h = 30.2;
  //String waterTankStatus = status;
  String macIdStr = mac_Id;
  String status = "FULL";

  String Temprature = String(t);
  String Humidity = String(h);
  Serial.print("Publish message: ");
  if(digitalRead(FLOATING_SENSOR) == HIGH)
    snprintf (msg, BUFFER_LEN, "{\"mac_Id\" : \"%s\", \"Status\" : \"%s\"}", macIdStr.c_str(), "Full");
  else
    snprintf (msg, BUFFER_LEN, "{\"mac_Id\" : \"%s\", \"Status\" : \"%s\"}", macIdStr.c_str(), "Empty");
  
  Serial.println("Mac ID in c_str format...!");
  Serial.println(msg);
  
  client.publish(PUBLISH_TOPIC, msg);
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //Serial.println(payload);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(PUBLISH_TOPIC, "hello world");
      // ... and resubscribe
      client.subscribe("SUBSCRIBE_TOPIC");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(FLOATING_SENSOR, INPUT);
  connectAWS();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  //publishMessage();
  client.loop();
  delay(5000);
}
