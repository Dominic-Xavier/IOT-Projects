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
    tone(buzzer,6000,250);
  }
}