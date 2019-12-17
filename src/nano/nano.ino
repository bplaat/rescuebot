#include <Arduino.h>
#include <Servo.h>

#define trigPin 2
#define echoPinLow 3
#define echoPinHigh 4
#define IR_PIN_LEFT 5
#define IR_PIN_RIGHT 6
#define SERVO_PIN 9

Servo myservo;

void setup()
{
  Serial.begin(115200);
  myservo.attach(SERVO_PIN);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPinLow, INPUT);
  pinMode(echoPinHigh, INPUT);
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

long getPulsDuration(int echoPin)
{
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration;
}

void ReadSensors()
{
  long duration = getPulsDuration(echoPinLow);
  Serial.print("duration SensorLow: ");
  Serial.println(duration);

  duration = getPulsDuration(echoPinHigh);
  Serial.print("duration SensorLow: ");
  Serial.println(duration);

  Serial.print("duration IRSensorLeft: ");
  Serial.println(digitalRead(IR_PIN_LEFT));

  Serial.print("duration IRSensorRight: ");
  Serial.println(digitalRead(IR_PIN_RIGHT));
}
