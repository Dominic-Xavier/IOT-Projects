#include <WiFi.h>
#include <Espalexa.h>
#include <IRsend.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// WiFi Credentials
#define WIFI_SSID "Haroon 4G"
#define WIFI_PASS "8754477070"

#define DHTTYPE DHT11

//Initialize with the I2C address of your OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//const int wifiLed = 27;
const int FAN  = 5;
const int light = 26, /*FAN = 13, */FanRelay = 4, AC =  18;
const int pirPin = 34, DHTPIN = 14;
const int IrReceiver = 27;
//const uint16_t FanOnOff = 0xCF8976;

DHT dht(DHTPIN, DHTTYPE);

Espalexa espalexa;

IRsend irsend(FAN);

// device names
String Device_1_Name = "Light";
String Device_2_Name = "Fan";
String Device_4_Name = "FanRelay";
String Device_5_Name = "AC";
String Device_6_Name = "Relay2";

boolean wifiConnected = false;

uint16_t FanOnOff[67] = {8974, 4462,  588, 544,  586, 568,  566, 542,  588, 570,  562, 568,  564, 568,  564, 570,  562, 570,  564, 
1682,  566, 1682,  564, 570,  564, 568,  562, 1686,  562, 1686,  562, 1686,  562, 1684,  564, 1684,  564, 572,  560, 570,  560, 574,  
560, 1684,  562, 570,  564, 570,  560, 1684,  586, 548,  560, 1686,  562, 1688,  560, 1684,  562, 570,  560, 1686,  564, 1684,  560, 574,  560};  // NEC CF8976

uint16_t Fan1[71] = {9050, 4382,  666, 464,  668, 466,  668, 464,  666, 466,  666, 464,  670, 464,  666, 464,  668, 464,  666, 1582,  666, 1582,  
668, 464,  668, 464,  666, 1580,  670, 1578,  668, 1580,  666, 1580,  668, 1580,  668, 1582,  666, 464,  666, 1582,  664, 468,  664, 468,  664, 466,  
668, 1582,  664, 468,  662, 468,  664, 1584,  664, 488,  640, 1586,  662, 1586,  662, 1584,  666, 488,  646, 41864,  9010, 2208,  602};  // NEC CFD12E

uint16_t Fan2[71] = {8978, 4458,  592, 540,  592, 542,  590, 542,  590, 542,  590, 544,  588, 544,  586, 544,  586, 548,  586, 1660,  588, 
1680,  566, 568,  564, 566,  564, 1684,  566, 1682,  566, 1682,  566, 1684,  564, 1684,  564, 1684,  564, 1684,  562, 1684,  562, 572,  562, 
568,  560, 572,  562, 1686,  562, 570,  562, 570,  562, 570,  560, 572,  562, 1686,  560, 1688,  560, 1686,  560, 574,  558, 41976,  8974, 2226,  560};  // NEC CFF10E

uint16_t Fan3[71] = {8980, 4458,  592, 540,  590, 564,  566, 542,  590, 544,  588, 546,  586, 564,  568, 544,  586, 566,  566, 1662,  586, 1684,  
564, 568,  566, 568,  564, 1684,  564, 1684,  562, 1684,  564, 1684,  564, 1686,  564, 1684,  564, 1682,  566, 1684,  562, 570,  560, 570,  562, 
570,  562, 1684,  564, 568,  562, 572,  560, 570,  560, 574,  558, 1686,  562, 1686,  560, 1688,  560, 574,  562, 41960,  8950, 2246,  562};  // NEC CFF10E

uint16_t Fan4[71] = {9054, 4382,  664, 468,  664, 466,  666, 468,  666, 466,  666, 466,  666, 468,  664, 466,  666, 466,  666, 1586,  662, 1582,  
668, 470,  662, 470,  660, 1580,  668, 1582,  664, 1582,  666, 1582,  666, 1584,  664, 1582,  664, 472,  662, 488,  642, 1584,  664, 492,  640, 472,  
660, 1604,  644, 492,  640, 492,  640, 1588,  660, 1606,  646, 486,  644, 1604,  642, 1606,  642, 490,  644, 41916,  8988, 2210,  600};  // NEC CFC936

uint16_t Fan5[71] = {9052, 4380,  666, 464,  666, 466,  666, 466,  666, 464,  666, 464,  666, 464,  668, 466,  664, 466,  668, 1580,  666, 1582,  
666, 466,  664, 466,  664, 1582,  666, 1582,  666, 1580,  668, 1580,  666, 470,  662, 468,  662, 468,  662, 1584,  662, 470,  662, 472,  658, 490,  
642, 1604,  644, 1584,  660, 1608,  640, 1606,  642, 490,  644, 1604,  642, 1604,  646, 1602,  646, 488,  644, 41892,  8988, 2210,  600};  // NEC CF11EE

uint16_t Fan6[71] = {9054, 4380,  668, 464,  668, 464,  668, 466,  666, 468,  664, 466,  666, 466,  666, 464,  666, 470,  660, 1582,  666, 1582,  
666, 472,  660, 472,  660, 1582,  666, 1584,  666, 1580,  664, 1584,  664, 1584,  664, 1582,  664, 1582,  664, 1584,  662, 474,  658, 472,  658, 474,  
658, 1586,  662, 492,  640, 492,  640, 492,  642, 494,  638, 1584,  662, 1584,  662, 1584,  664, 492,  642, 41864,  9004, 2210,  600};  // NEC CFF10E

// connect to wifi – returns true if successful or false if not
boolean connectWifi() {

  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false;
      break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else
    Serial.println("Connection failed.");
  return state;
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }

  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void addDevices() {
  espalexa.addDevice(Device_1_Name, firstLightChanged);  //simplest definition, default state off
  espalexa.addDevice(Device_2_Name, atombergFan);
  espalexa.addDevice(Device_4_Name, fanRelays);
  espalexa.addDevice(Device_5_Name, AirConditioner);
  espalexa.begin();
}

void AirConditioner(uint8_t brightness){
  if (brightness == 255) {
      digitalWrite(AC, HIGH);
      Serial.println("AC ON");
    }
  else {
    digitalWrite(AC, LOW);
    Serial.println("AC OFF");
  }
}

void fanRelays(uint8_t brightness) {
  if (brightness == 255) {
    digitalWrite(FanRelay, HIGH);
    Serial.println("Fan Relay ON");
  } else {
    digitalWrite(FanRelay, LOW);
    Serial.println("Fan Relay OFF");
  }
}

//our callback functions
void firstLightChanged(uint8_t brightness) {
  if (brightness == 255) {
    digitalWrite(light, HIGH);
    Serial.println("Device1 ON");
  } else {
    digitalWrite(light, LOW);
    Serial.println("Device1 OFF");
  }
}

//our callback functions
void atombergFan(uint8_t brightness) {
  //Control the device
  if (brightness == 255) {
    Serial.println("Fan IR ON");    
    irsend.sendRaw(FanOnOff, 67, 38);
    displayData("Fan ON");
  } else {
    Serial.println("Fan IR OFF");
    irsend.sendRaw(FanOnOff, 67, 38);
    displayData("Fan Off");
  }
}

void PIRSensor() {
  if (digitalRead(pirPin) == HIGH) {
    digitalWrite(light, HIGH);
  }
  if (digitalRead(pirPin) == LOW) {
    digitalWrite(light, LOW);
  }
}

void displayData(String data){
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println(data);
  display.display();
  delay(2000);
}

void displayData(double humidity, double temperature) {
  display.clearDisplay();  // Clear the buffer

  // Text settings
  display.setTextSize(1);               // Normal text size
  display.setTextColor(SSD1306_WHITE);  // White text color
  display.setCursor(0, 2);              // Start at top-left corner
  display.println("Temp :");
  display.print(temperature);
  display.println("Humidity :");
  display.print(humidity);
  // Display the text
  display.display();

  delay(2000);  // Pause for 2 seconds
}

void dhtSensor() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    displayData("Error in DHT Sensor");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  displayData(h, hic);
}

void irReceiver() {

}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  irsend.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
   displayData("Devide Turned ON...!");

  pinMode(light, OUTPUT);
  //pinMode(wifiLed, OUTPUT);
  pinMode(FanRelay, OUTPUT);
  pinMode(AC, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(IrReceiver, INPUT);
  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected)
    addDevices();
  else {
    Serial.println("Cannot connect to WiFi. So in Manual Mode");
    delay(1000);
  }
}

void loop() {
  //Serial.print("WiFi Connected  ");

  //Manual Switch Control
  //WiFi Control
  if (wifiConnected) {
    espalexa.loop();
    //digitalWrite(wifiLed, HIGH);
    delay(1);
  } else {
    if (!wifiConnected)
      //digitalWrite(wifiLed, LOW);
      wifiConnected = connectWifi();  // Initialise wifi connection
    if (wifiConnected)
      addDevices();   
  }
  //dhtSensor();
  //PIRSensor();
}
