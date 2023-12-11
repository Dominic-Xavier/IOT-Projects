/*
 * Example of a basic FreeRTOS queue
 * https://www.freertos.org/Embedded-RTOS-Queues.html
 */

// Include Arduino FreeRTOS library
#include <Arduino.h>

#include <WiFi.h>
#include <Espalexa.h>
#include <IRsend.h>
#include <IRrecv.h>

#include <IRremoteESP8266.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Function Declaration
void pir(void *pvParameters);
void dhtMethod(void *pvParameters);
void alexa(void *pvParameters);
void iRreceiver(void *pvParameters);

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
const int light = 12, /*FAN = 13, */FanRelay = 18, AC =  4;
const int pirPin = 13, DHTPIN = 14;
const int IrReceivers = 15;
const int ldr = 32;
int readLDRValue=0, count = 0;
//const uint16_t FanOnOff = 0xCF8976;

DHT dht(DHTPIN, DHTTYPE);

Espalexa espalexa;

IRsend irsend(FAN);
IRrecv irrecv(IrReceivers);
decode_results results;

// device names
String Device_1_Name = "Light";
String Device_2_Name = "Fan";
String Device_4_Name = "FanRelay";
String Device_5_Name = "AC";

boolean wifiConnected = false;

uint16_t FanOnOff[67] = {8974, 4462,  588, 544,  586, 568,  566, 542,  588, 570,  562, 568,  564, 568,  564, 570,  562, 570,  564, 
1682,  566, 1682,  564, 570,  564, 568,  562, 1686,  562, 1686,  562, 1686,  562, 1684,  564, 1684,  564, 572,  560, 570,  560, 574,  
560, 1684,  562, 570,  564, 570,  560, 1684,  586, 548,  560, 1686,  562, 1688,  560, 1684,  562, 570,  560, 1686,  564, 1684,  560, 574,  560};  // NEC CF8976

uint16_t Fan1[71] = {9060, 4374,  668, 462,  672, 460,  672, 460,  670, 460,  670, 460,  672, 460,  672, 460,  668, 464,  668, 1578,  670, 
1578,  668, 462,  668, 466,  666, 1580,  666, 1582,  666, 1582,  664, 1586,  662, 1586,  662, 1586,  660, 472,  658, 1590,  656, 476,  656, 476,  
654, 478,  656, 1590,  656, 476,  658, 474,  654, 1592,  628, 506,  622, 1624,  622, 1626,  600, 1648,  622, 512,  622, 41916,  9006, 2194,  616};  // NEC CFD12E

uint16_t Fan2[71] = {9050, 4384,  666, 464,  666, 468,  664, 468,  666, 466,  666, 464,  668, 464,  668, 464,  666, 466,  666, 1582,  666, 1582,  
668, 472,  660, 472,  662, 1582,  664, 1584,  664, 1584,  664, 1584,  664, 474,  658, 472,  658, 474,  660, 474,  660, 1584,  662, 494,  638, 472,  658, 1586,  660, 1610,  638, 1606,  640, 1608,  640, 1606,  644, 486,  644, 1602,  646, 1602,  644, 490,  644, 41918,  8988, 2208,  600};  // NEC CF09F6

uint16_t Fan3[71] = {9050, 4380,  670, 464,  666, 468,  666, 466,  666, 466,  666, 464,  670, 464,  668, 464,  666, 466,  666, 1582,  666, 1582,  
666, 464,  666, 472,  660, 1582,  664, 1584,  666, 1582,  664, 1582,  666, 474,  660, 1582,  664, 470,  660, 1584,  664, 472,  660, 472,  658, 494,  
638, 1588,  662, 1586,  662, 492,  640, 1586,  662, 496,  638, 1606,  644, 1584,  660, 1608,  642, 494,  640, 41894,  9008, 2208,  600};  // NEC CF51AE

uint16_t Fan4[71] = {9050, 4384,  666, 468,  666, 468,  664, 468,  662, 470,  664, 468,  664, 474,  658, 472,  660, 474,  658, 1586,  662, 1586,  664, 472,  
658, 474,  658, 1584,  662, 1586,  662, 1584,  662, 1584,  662, 1588,  660, 1588,  660, 478,  652, 494,  638, 1588,  660, 492,  638, 494,  638, 1608,  640, 494,  
636, 494,  638, 1608,  640, 1606,  642, 488,  646, 1602,  646, 1600,  646, 488,  644, 41904,  8988, 2208,  600};  // NEC CFC936

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
  int count = 0;
  //Control the device
    switch(brightness){
      Serial.println(brightness);
      //Off
      case 0:
        Serial.println("Fan Off");
        irsend.sendRaw(FanOnOff, 71, 38);
        displayData("Fan Off");
      break;

      //On
      case 255:
        Serial.println("Fan On");
        irsend.sendRaw(FanOnOff, 71, 38);
        displayData("Fan On");
      break;

      //Speed 1
      case 5:
        Serial.println("Fan Speed 1");
        irsend.sendRaw(Fan1, 71, 38);
        displayData("Fan Speed 1");
      break;

      //Speed 2
      case 7:
        Serial.println("Fan Speed 2");
        irsend.sendRaw(Fan2, 71, 38);
        displayData("Fan Speed 2");
      break;

      //Speed 3
      case 10:
        Serial.println("Fan Speed 3");
        irsend.sendRaw(Fan3, 71, 38);
        displayData("Fan Speed 3");
      break;

      //Speed 4
      case 12:
        Serial.println("Fan Speed 4");
        irsend.sendRaw(Fan4, 71, 38);
        displayData("Fan Speed 4");
      break;

      //Speed 5
      case 15:
        Serial.println("Fan Speed 5");
        irsend.sendRaw(Fan5, 71, 38);
        displayData("Fan Speed 5");
      break;

      //Speed 6
      case 17:
        Serial.println("Fan Speed 6");
        irsend.sendRaw(Fan6, 71, 38);
        displayData("Fan Speed 6");
      break;
    
  }
}

void PIRSensor() {
  readLDRValue = analogRead(ldr);
  Serial.println("LDR value");
  Serial.println(readLDRValue);
  if (digitalRead(pirPin) == HIGH) {
    if(readLDRValue>800){
      digitalWrite(light, HIGH);
      //irsend.sendRaw(FanOnOff, 71, 38);
      Serial.println("Pir On");
    }
    else{
      digitalWrite(light, LOW);
      Serial.println("Pir On But Light is off");
    }
    Serial.println(readLDRValue);
  }
  if (digitalRead(pirPin) == LOW) {
    digitalWrite(light, LOW);
    //irsend.sendRaw(FanOnOff, 71, 38);
    Serial.println("Pir Off");
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
  display.setTextSize(2);               // Normal text size
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

void irReceive() {
  if (irrecv.decode(&results)) {
    Serial.println("ir sIGNAL received...!");
    irrecv.resume();  // Receive the next value
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}


void setup() {
  Serial.begin(115200);
  irsend.begin();
  irrecv.enableIRIn();  // Start the receiver
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
  //pinMode(IrReceivers, INPUT);
  pinMode(ldr, INPUT);
  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected)
    addDevices();
  else {
    Serial.println("Cannot connect to WiFi. So in Manual Mode");
    delay(1000);
  }

  xTaskCreatePinnedToCore(//Use xTaskCreate in Venilla RTOS
    dhtMethod,             //Function name to be called
    "DHT",                //Name of the Task
    1500,                 //Stack Size (bytes in Esp32 , words in freeRTOS)
    NULL,                 //Parameter
    1,                    //Task Priority
    NULL,                 //Task Handle           
    0);                   //Run on core ID

    xTaskCreatePinnedToCore( //Use xTaskCreate in Venilla RTOS
    iRreceiver,              //Function name to be called
    "IR Sensor",             //Name of the Task
    1500,                    //Stack Size (bytes in Esp32 , words in freeRTOS)
    NULL,                    //Parameter
    1,                       //Task Priority
    NULL,                    //Task Handle           
    1);                      //Run on core ID

    xTaskCreatePinnedToCore(    //Use xTaskCreate in Venilla RTOS
    pir,                        //Function name to be called
    "PIR Sensor",               //Name of the Task
    1028,                       //Stack Size (bytes in Esp32 , words in freeRTOS)
    NULL,                       //Parameter
    3,                          //Task Priority
    NULL,                       //Task Handle           
    0);                         //Run on core ID

    // xTaskCreatePinnedToCore(//Use xTaskCreate in Venilla RTOS
    // alexa,                  //Function name to be called
    // "Alexa",                //Name of the Task
    // 3000,                 //Stack Size (bytes in Esp32 , words in freeRTOS)
    // NULL,                 //Parameter
    // 2,                    //Task Priority
    // NULL,                 //Task Handle           
    // 1);                   //Run on core ID

}

void alexa(void *pvParameters){
  // if (wifiConnected)
  //   addDevices();
  // else {
  //   Serial.println("Cannot connect to WiFi. So in Manual Mode");
  //   delay(1000);
  //}
  
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
}

void alexa(){
  // if (wifiConnected)
  //   addDevices();
  // else {
  //   Serial.println("Cannot connect to WiFi. So in Manual Mode");
  //   delay(1000);
  //}
  if(WiFi.status() != WL_CONNECTED){
    wifiConnected = connectWifi();
  }
  if (wifiConnected) {
    if(count<1){
      displayData("WiFi Connected....!");
      displayData(WIFI_SSID);
      count++;
    }
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
}

void loop() { 
  alexa();
}

/**
 * Analog read task for Pin A0
 * Reads an analog input on pin 0 and send the readed value through the queue.
 * See Blink_AnalogRead example.
 */
void dhtMethod(void *pvParameters){
  while(1){
    //PIRSensor();
    dhtSensor();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
  
}

void iRreceiver(void *pvParameters){
  while(1){
    //PIRSensor();
    irReceive();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }  
}

/**
 * Analog read task for Pin A1
 * Reads an analog input on pin 1 and send the readed value through the queue.
 * See Blink_AnalogRead example.
 */
void pir(void *pvParameters){
  while(1){
    PIRSensor();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
    
}