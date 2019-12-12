#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "config.h"
#include "controls.h"

#define MOTOR_LEFT_FORWARD_PIN D0
#define MOTOR_LEFT_BACKWARD_PIN D1
#define MOTOR_LEFT_ENABLE_PIN D2

#define MOTOR_RIGHT_FORWARD_PIN D3
#define MOTOR_RIGHT_BACKWARD_PIN D4
#define MOTOR_RIGHT_ENABLE_PIN D5

/*#define IR_LEFT_PIN D6
#define IR_RIGHT_PIN D7
#define PULSE_SERVO_PIN D8

#define TRIGGER_PIN D9
#define ECHO_PIN_LOW_MOUNTED 10
#define ECHO_PIN_HIGH_MOUNTED A0
*/

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
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, LOW);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

void motor_turn_right() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, LOW);
}

void motor_stop() {
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, LOW);
}

ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);

    pinMode(MOTOR_LEFT_FORWARD_PIN, OUTPUT);
    pinMode(MOTOR_LEFT_BACKWARD_PIN, OUTPUT);
    pinMode(MOTOR_LEFT_ENABLE_PIN, OUTPUT);

    pinMode(MOTOR_RIGHT_FORWARD_PIN, OUTPUT);
    pinMode(MOTOR_RIGHT_BACKWARD_PIN, OUTPUT);
    pinMode(MOTOR_RIGHT_ENABLE_PIN, OUTPUT);

    /*
    pinMode(PULSE_SERVO_PIN, OUTPUT);

    pinMode(ECHO_PIN_LOW_MOUNTED, INPUT);
    pinMode(ECHO_PIN_HIGH_MOUNTED, INPUT);
    //pinMode(TRIGGER_PIN, OUTPUT);

    pinMode(IR_OUT_RIGHT_PIN, INPUT);
    pinMode(IR_OUT_LEFT_PIN, INPUT);
    */

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
}
