#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

uint8_t distanceLow;
uint8_t distanceHigh;
uint8_t irPinLeft;
uint8_t irPinRight;
uint8_t distanceLeft;
uint8_t distanceRight;


bool PulsHighCaptured = true;
bool PulsLowCaptured = true;

#define ECHO_PIN_LOW 2
#define ECHO_PIN_HIGH 3
#define TRIG_PIN 4
#define IR_PIN_LEFT A1
#define IR_PIN_RIGHT A2
#define MAGNET_PIN 7
#define SERVO_PIN 9
#define I2C_ADDRESS 10
//#define SERIAL_CON

    Servo myservo;

void sendValues()
{
  Wire.write(distanceLow);
  Wire.write(distanceHigh);
  Wire.write(irPinLeft);
  Wire.write(irPinRight);
  Wire.write(distanceRight);
  Wire.write(distanceLeft);
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

uint8_t servoPos[] = {5, 50, 95, 150, 180, 150, 95, 50};
uint8_t pos = 0;
void loop()
{
  distanceLow = getDistanceFromPosition(servoPos[pos], ECHO_PIN_HIGH);
  distanceHigh = getDistanceFromPosition(servoPos[pos], ECHO_PIN_LOW);
  irPinLeft = analogRead(IR_PIN_LEFT) > 200;
  irPinRight = analogRead(IR_PIN_RIGHT) > 200;
  if(pos == 4 ){
    distanceRight = distanceHigh;
  }
    if(pos == 0 ){
    distanceLeft = distanceHigh;
  }
  if (++pos >= 8)
  {
    pos = 0;
  }
  delay(500);

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
