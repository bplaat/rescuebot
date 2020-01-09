#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

uint8_t distanceLow;
uint8_t distanceHigh;
uint8_t irPinLeft;
uint8_t irPinRight;

bool PulsHighCaptured = true;
bool PulsLowCaptured = true;

#define TRIG_PIN 4
#define ECHO_PIN_LOW 2
#define ECHO_PIN_HIGH 3
#define IR_PIN_LEFT 5
#define IR_PIN_RIGHT 6
#define MAGNET_PIN 7
#define SERVO_PIN 9
#define I2C_ADDRESS 10
#define SERIAL_CON

Servo myservo;

void sendValues()
{
  Wire.write(distanceLow);
  Wire.write(distanceHigh);
  Wire.write(irPinLeft);
  Wire.write(irPinRight);
}

void setup()
{
  #ifdef SERIAL_CON
  Serial.begin(115200);
  #endif
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(sendValues);
  myservo.attach(SERVO_PIN);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN_LOW, INPUT);
  pinMode(ECHO_PIN_HIGH, INPUT);
}

void loop()
{
  for (uint8_t i = 0; i < 180; i += 90)
  {
    distanceLow = getDistanceFromPosition(i, ECHO_PIN_HIGH);
    distanceHigh = getDistanceFromPosition(i, ECHO_PIN_LOW);
    irPinLeft = digitalRead(IR_PIN_LEFT);
    irPinRight = digitalRead(IR_PIN_RIGHT);
    delay(500);
  }
  

#ifdef SERIAL_CON
  Serial.println("distanceHigh");
  Serial.println(distanceHigh);
  Serial.println("distanceLow");
  Serial.println(distanceLow);
  Serial.println("irPinLeft");
  Serial.println(irPinLeft);
  Serial.println("irPinRight");
  Serial.println(irPinRight);
#endif
}

int getDistanceFromPosition(int position, uint8_t echoPin)
{
  myservo.write(position);
  return getDistance(echoPin);
}

int getDistance(uint8_t echoPin)
{
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW); 
  delayMicroseconds(2);       
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10); 
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  return distance;
}
 