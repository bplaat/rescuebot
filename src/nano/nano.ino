#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

uint8_t distanceLow;
uint8_t distanceHigh;
uint8_t irPinLeft;
uint8_t irPinRight;

#define FACTOR 100
#define TRIG_PIN 2
#define ECHO_PIN_LOW 3
#define ECHO_PIN_HIGH 4
#define IR_PIN_LEFT 5
#define IR_PIN_RIGHT 6
#define MAGNET_PIN 7
#define SERVO_PIN 9
#define I2C_ADDRESS 10

int16_t duration = 0;

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
  Serial.begin(115200);
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(sendValues);
  myservo.attach(SERVO_PIN);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN_LOW, INPUT);
  pinMode(ECHO_PIN_HIGH, INPUT);
}

void loop()
{
  int pos;
  readSensors();
  for (pos = 0; pos <= 180; pos += 2)
  { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos); // tell servo to go to position in variable 'pos'
    delayMicroseconds(1000);
  }
  for (pos = 180; pos >= 0; pos -= 2)
  {                     // goes from 180 degrees to 0 degrees
    myservo.write(pos); // tell servo to go to position in variable 'pos'
    delayMicroseconds(1000);
  }
}

long getPulsDuration(int echoPin)
{
  long duration;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration;
}

void readSensors()
{
  long durationLow = getPulsDuration(ECHO_PIN_LOW);
  Serial.print("duration SensorLow: ");
  Serial.println(duration);
  distanceLow = duration / FACTOR;

  long durationHigh = getPulsDuration(ECHO_PIN_HIGH);
  Serial.print("duration SensorLow: ");
  Serial.println(duration);
  distanceHigh = duration / FACTOR;

  Serial.print("duration IRSensorLeft: ");
  irPinLeft = digitalRead(IR_PIN_LEFT);
  Serial.println(irPinLeft);

  Serial.print("duration IRSensorRight: ");
  irPinRight = digitalRead(IR_PIN_RIGHT);
  Serial.println(irPinRight);
}
