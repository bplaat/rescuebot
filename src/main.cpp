#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "config.hpp"
#include "website.hpp"

#define I2C_ADDRESS 10
#define PROTOCOL_REQUEST_MESSAGE_LENGTH 4

#define MOTOR_LEFT_FORWARD_PIN D0
#define MOTOR_LEFT_BACKWARD_PIN D1
#define MOTOR_LEFT_ENABLE_PIN D2

#define MOTOR_RIGHT_FORWARD_PIN D3
#define MOTOR_RIGHT_BACKWARD_PIN D4
#define MOTOR_RIGHT_ENABLE_PIN D5

uint8_t distance_to_ground;
uint8_t distance_to_object;
uint8_t ir_left;
uint8_t ir_right;


void motor_move_forward() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

void motor_move_backward() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

void motor_turn_left() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, LOW);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

void motor_turn_right() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, LOW);
}

void motor_stop() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, LOW);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, LOW);
}

ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(MOTOR_LEFT_FORWARD_PIN, OUTPUT);
    pinMode(MOTOR_LEFT_BACKWARD_PIN, OUTPUT);
    pinMode(MOTOR_LEFT_ENABLE_PIN, OUTPUT);

    pinMode(MOTOR_RIGHT_FORWARD_PIN, OUTPUT);
    pinMode(MOTOR_RIGHT_BACKWARD_PIN, OUTPUT);
    pinMode(MOTOR_RIGHT_ENABLE_PIN, OUTPUT);

    Serial.print("\nConnecting to ");
    Serial.print(wifi_ssid);
    Serial.println("...");
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", []() {
        server.send(200, "text/html", website_html, website_html_len);
    });

    server.on("/api/update_motor_state", []() {
        String motor_state = server.arg("motor_state");

        if (motor_state == "forward") {
            motor_move_forward();
        }
        if (motor_state == "left") {
            motor_turn_left();
        }
        if (motor_state == "right") {
            motor_turn_right();
        }
        if (motor_state == "backward") {
            motor_move_backward();
        }
        if (motor_state == "stop") {
            motor_stop();
        }

        server.send(200, "application/json", "{\"message\":\"succesfull\"}");
    });

    server.begin();
}

void loop() {
    server.handleClient();
    if (Wire.requestFrom(I2C_ADDRESS, PROTOCOL_REQUEST_MESSAGE_LENGTH) == PROTOCOL_REQUEST_MESSAGE_LENGTH) {
        distance_to_ground = Wire.read();
        distance_to_object = Wire.read();
        ir_left = Wire.read();
        ir_right = Wire.read();
    }
}
