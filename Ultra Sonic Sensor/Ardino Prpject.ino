// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLDTXmAb-U"
#define BLYNK_DEVICE_NAME "Water level Sensor"
#define BLYNK_AUTH_TOKEN "nY0UjonEpJtbh2xYG0n272_lsBpY68_n"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define trig D5   // Trig pin
#define echo D6
#define relaySwitch D7
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Haroon 4G";
char pass[] = "8754477070";
int depth =20;

BlynkTimer timer;
 
void waterlevel() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  long cm = t / 29 / 2;
  Serial.println("Distnce is "+cm);
  long level= depth-cm;
  if (level<0)
    level=0;
  level = map(level,0,depth-3,0,100);
  Blynk.virtualWrite(V0, level);
}

void setup() {
  pinMode(relaySwitch, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1L, waterlevel);  
}

void loop() {
  Blynk.run();
  timer.run();
}