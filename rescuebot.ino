// start http://145.24.238.50/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "config.h"
#include "controls.h"

#define MOTOR_LEFT_FORWARD_PIN D0
#define MOTOR_LEFT_BACKWARDS_PIN D1
#define MOTOR_LEFT_ENABLE_PIN D2

#define MOTOR_RIGHT_FORWARD_PIN D3
#define MOTOR_RIGHT_BACKWARDS_PIN D4
#define MOTOR_RIGHT_ENABLE_PIN D5

#define API_MOTOR_STATE_STOP 0
#define API_MOTOR_STATE_FORWARD 1
#define API_MOTOR_STATE_LEFT 2
#define API_MOTOR_STATE_RIGHT 3
#define API_MOTOR_STATE_BACKWARDS 4

void motor_move_forward() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_BACKWARDS_PIN, LOW);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_BACKWARDS_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

void motor_move_backwards() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_BACKWARDS_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_BACKWARDS_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

void motor_turn_left() {
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, LOW);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_BACKWARDS_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

void motor_turn_right() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_BACKWARDS_PIN, LOW);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, LOW);
}

void motor_stop() {
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, LOW);
}

ESP8266WebServer server(80);

void setup() {
    Serial.begin(9600);

    pinMode(MOTOR_LEFT_FORWARD_PIN, OUTPUT);
    pinMode(MOTOR_LEFT_BACKWARDS_PIN, OUTPUT);
    pinMode(MOTOR_LEFT_ENABLE_PIN, OUTPUT);

    pinMode(MOTOR_RIGHT_FORWARD_PIN, OUTPUT);
    pinMode(MOTOR_RIGHT_BACKWARDS_PIN, OUTPUT);
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
        server.send(200, "text/html", controls_html);
    });

    server.on("/api/update_motor", []() {
        int32_t motor_state = atoi(server.arg("state").c_str());

        if (motor_state == API_MOTOR_STATE_FORWARD) {
            motor_move_forward();
        }
        if (motor_state == API_MOTOR_STATE_LEFT) {
            motor_turn_left();
        }
        if (motor_state == API_MOTOR_STATE_RIGHT) {
            motor_turn_right();
        }
        if (motor_state == API_MOTOR_STATE_BACKWARDS) {
            motor_move_backwards();
        }
        if (motor_state == API_MOTOR_STATE_STOP) {
            motor_stop();
        }

        server.send(200, "application/json", "{\"message\":\"succesfull\"}");
    });

    server.begin();
}

void loop() {
    server.handleClient();
}
