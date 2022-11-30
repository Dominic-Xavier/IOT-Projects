#include<Esp.h>
#include<ESP8266WiFi.h>
#include<DNSServer.h>
#include<ESP8266WebServer.h>
#include<WiFiManager.h>

#define PushButton D6
#define buzzer D7

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(PushButton, INPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  
  if(digitalRead(PushButton)==HIGH){
    tone(buzzer, 6000, 250);                
    WiFiManager wifiManager;
    if(!wifiManager.startConfigPortal("Haroon 5G")){
      Serial.println("Failed to connect and timed out...!");
      delay(3000);
      ESP.reset();
      delay(2000);            
    }
  Serial.println("Connected...!!");
  }
}