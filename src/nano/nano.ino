// The arduino, servo and wire library
#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

// Define some constansts and variables
#define DEBUG
#define I2C_ADDRESS 10

uint8_t distance_to_object;
uint8_t distance_to_left;
uint8_t distance_to_right;
uint8_t distance_to_ground;
uint8_t ir_left;
uint8_t ir_right;

// The pins
#define GROUND_ECHO_PIN 2
#define OBJECT_ECHO_PIN 3
#define TRIGGER_PIN 4
#define LEFT_IR_PIN A0
#define RIGHT_IR_PIN A1
#define SERVO_PIN 9

// The servo object
Servo servo;

// Function the reads a ultra soon echo pin and returns the distance in cm
int32_t echo_get_distance(uint8_t echo_pin) {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    return pulseIn(echo_pin, HIGH) / 29 / 2;
}

// Request event handler that writes the data to protocol
void request_event() {
    Wire.write(distance_to_object);
    Wire.write(distance_to_left);
    Wire.write(distance_to_right);
    Wire.write(distance_to_ground);
    Wire.write(ir_left);
    Wire.write(ir_right);
}

// Init the serial, wire and pins
void setup() {
    #ifdef DEBUG
        Serial.begin(115200);
    #endif

    Wire.begin(I2C_ADDRESS);
    Wire.onRequest(request_event);

    pinMode(OBJECT_ECHO_PIN, INPUT);
    pinMode(GROUND_ECHO_PIN, INPUT);
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(LEFT_IR_PIN, INPUT);
    pinMode(RIGHT_IR_PIN, INPUT);

    servo.attach(SERVO_PIN);
}

// The servo positions
uint8_t servo_position[] = { 5, 50, 95, 150, 180, 150, 95, 50 };
uint8_t servo_position_index = 0;

void loop() {
    // Write the current servo position
    servo.write(servo_position[servo_position_index]);

    // Read the ultra soon sensors
    distance_to_object = echo_get_distance(OBJECT_ECHO_PIN);
    distance_to_ground = echo_get_distance(GROUND_ECHO_PIN);

    // Read the ir sensors
    ir_left = analogRead(LEFT_IR_PIN) < 200;
    ir_right = analogRead(RIGHT_IR_PIN) < 200;

    // When servo is on the edge write distance to the left and right variable
    if (servo_position_index == 0) {
        distance_to_left = distance_to_object;
    }
    if (servo_position_index == 4) {
        distance_to_right = distance_to_object;
    }

    // Reset index when on the end else increment
    if (servo_position_index == 7) {
        servo_position_index = 0;
    } else {
        servo_position_index++;
    }

    delay(250);

    // When debuging print all the variables
    #ifdef DEBUG
        Serial.print("distance_to_object = ");
        Serial.println(distance_to_object);
        Serial.print("distance_to_left = ");
        Serial.println(distance_to_left);
        Serial.print("distance_to_right = ");
        Serial.println(distance_to_right);
        Serial.print("distance_to_ground = ");
        Serial.println(distance_to_ground);
        Serial.print("ir_left = ");
        Serial.println(ir_left);
        Serial.print("ir_right = ");
        Serial.println(ir_right);
    #endif
}
