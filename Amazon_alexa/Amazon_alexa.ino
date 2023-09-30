#include <WiFi.h>
#include <Espalexa.h>
#include <IRsend.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Initialize with the I2C address of your OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//const int wifiLed = 27;
const uint16_t FAN = 5;
const int light = 26, /*FAN = 13, */FanRelay = 4, AC =  18;
const int pirPin = 34, DHTPIN = 14;
const int IrReceiver = 27; 
const uint16_t FanOnOff = 0xCF8976;

Espalexa espalexa;
IRsend irsend(FAN);

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// WiFi Credentials
#define WIFI_SSID "Haroon 4G"
#define WIFI_PASS "8754477070"

// device names
String Device_1_Name = "Light";
String Device_2_Name = "Fan";
String Device_4_Name = "FanRelay";
String Device_5_Name = "AC";
String Device_6_Name = "Relay2";

boolean wifiConnected = false;

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
    irsend.sendNEC(FanOnOff);
    displayData("Fan ON");
  } else {
    Serial.println("Fan IR OFF");
    irsend.sendNEC(FanOnOff);
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
  irsend.begin();

  #if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266

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
  dhtSensor();
  PIRSensor();
}
