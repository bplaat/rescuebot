#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

#define FACTOR 100
#define TRIG_PIN 2
#define ECHO_PIN_LOW 3
#define ECHO_PIN_HIGH 4
#define IR_PIN_LEFT 5
#define IR_PIN_RIGHT 6
#define SERVO_PIN 9

Servo myservo;

void setup()
{
  Serial.begin(115200);
  Wire.begin(10);
  myservo.attach(SERVO_PIN);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN_LOW, INPUT);
  pinMode(ECHO_PIN_HIGH, INPUT);
}

void loop()
{
  int pos;

  for (pos = 0; pos <= 180; pos += 1)
  { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos); // tell servo to go to position in variable 'pos'
    delay(15);          // waits 15ms for the servo to reach the position
    ReadSensors();
  }
  for (pos = 180; pos >= 0; pos -= 1)
  {                     // goes from 180 degrees to 0 degrees
    myservo.write(pos); // tell servo to go to position in variable 'pos'
    delay(15);          // waits 15ms for the servo to reach the position
    ReadSensors();
  }
}

uint16_t getPulsDuration(int echoPin)
{
  long duration;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(echoPin, HIGH);
  return (uint16_t)duration;
}

void ReadSensors()
{
  int16_t duration = getPulsDuration(ECHO_PIN_LOW);
  Serial.print("duration SensorLow: ");
  Serial.println(duration);
  Wire.write(duration/FACTOR);

  duration = getPulsDuration(ECHO_PIN_HIGH);
  Serial.print("duration SensorLow: ");
  Serial.println(duration);
  Wire.write(duration/FACTOR);

  Serial.print("duration IRSensorLeft: ");
  uint8_t b = digitalRead(IR_PIN_LEFT);
  Serial.println(b);
  Wire.write(b);


  Serial.print("duration IRSensorRight: ");
  b = digitalRead(IR_PIN_RIGHT);
  Serial.println(b);
  Wire.write(b);
}
