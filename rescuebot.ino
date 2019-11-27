#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "config.h"

#define MOTOR_DIRECTION_FORWARD 0
#define MOTOR_DIRECTION_BACKWARDS 1

#define MOTOR_SPEED_STOPPED 0
#define MOTOR_SPEED_FULL 255

#define MOTOR_FRONT_LEFT_DIRECTION_PIN D0
#define MOTOR_FRONT_LEFT_SPEED_PIN D1

#define MOTOR_REAR_LEFT_DIRECTION_PIN D1
#define MOTOR_REAR_LEFT_SPEED_PIN D2

#define MOTOR_FRONT_RIGHT_DIRECTION_PIN D3
#define MOTOR_FRONT_RIGHT_SPEED_PIN D4

#define MOTOR_REAR_RIGHT_DIRECTION_PIN D5
#define MOTOR_REAR_RIGHT_SPEED_PIN D6

#define API_MOTOR_DIRECTION_FORWARD 0
#define API_MOTOR_DIRECTION_LEFT 1
#define API_MOTOR_DIRECTION_RIGHT 2
#define API_MOTOR_DIRECTION_BACKWARDS 3

void motor_move_forward() {
    digitalWrite(MOTOR_FRONT_LEFT_DIRECTION_PIN, MOTOR_DIRECTION_FORWARD);
    analogWrite(MOTOR_FRONT_LEFT_SPEED_PIN, MOTOR_SPEED_FULL);

    digitalWrite(MOTOR_REAR_LEFT_DIRECTION_PIN, MOTOR_DIRECTION_FORWARD);
    analogWrite(MOTOR_REAR_LEFT_SPEED_PIN, MOTOR_SPEED_FULL);

    digitalWrite(MOTOR_FRONT_RIGHT_DIRECTION_PIN, MOTOR_DIRECTION_FORWARD);
    analogWrite(MOTOR_FRONT_RIGHT_SPEED_PIN, MOTOR_SPEED_FULL);

    digitalWrite(MOTOR_REAR_RIGHT_DIRECTION_PIN, MOTOR_DIRECTION_FORWARD);
    analogWrite(MOTOR_REAR_RIGHT_SPEED_PIN, MOTOR_SPEED_FULL);
}

void motor_move_backwards() {
    digitalWrite(MOTOR_FRONT_LEFT_DIRECTION_PIN, MOTOR_DIRECTION_BACKWARDS);
    analogWrite(MOTOR_FRONT_LEFT_SPEED_PIN, MOTOR_SPEED_FULL);

    digitalWrite(MOTOR_REAR_LEFT_DIRECTION_PIN, MOTOR_DIRECTION_BACKWARDS);
    analogWrite(MOTOR_REAR_LEFT_SPEED_PIN, MOTOR_SPEED_FULL);

    digitalWrite(MOTOR_FRONT_RIGHT_DIRECTION_PIN, MOTOR_DIRECTION_BACKWARDS);
    analogWrite(MOTOR_FRONT_RIGHT_SPEED_PIN, MOTOR_SPEED_FULL);

    digitalWrite(MOTOR_REAR_RIGHT_DIRECTION_PIN, MOTOR_DIRECTION_BACKWARDS);
    analogWrite(MOTOR_REAR_RIGHT_SPEED_PIN, MOTOR_SPEED_FULL);
}

void motor_turn_left() {
    analogWrite(MOTOR_FRONT_LEFT_SPEED_PIN, MOTOR_SPEED_STOPPED);
    analogWrite(MOTOR_REAR_LEFT_SPEED_PIN, MOTOR_SPEED_STOPPED);

    digitalWrite(MOTOR_FRONT_RIGHT_DIRECTION_PIN, MOTOR_DIRECTION_FORWARD);
    analogWrite(MOTOR_FRONT_RIGHT_SPEED_PIN, MOTOR_SPEED_FULL);

    digitalWrite(MOTOR_REAR_RIGHT_DIRECTION_PIN, MOTOR_DIRECTION_FORWARD);
    analogWrite(MOTOR_REAR_RIGHT_SPEED_PIN, MOTOR_SPEED_FULL);
}

void motor_turn_right() {
    digitalWrite(MOTOR_FRONT_LEFT_DIRECTION_PIN, MOTOR_DIRECTION_FORWARD);
    analogWrite(MOTOR_FRONT_LEFT_SPEED_PIN, MOTOR_SPEED_FULL);

    digitalWrite(MOTOR_REAR_LEFT_DIRECTION_PIN, MOTOR_DIRECTION_FORWARD);
    analogWrite(MOTOR_REAR_LEFT_SPEED_PIN, MOTOR_SPEED_FULL);

    analogWrite(MOTOR_FRONT_RIGHT_SPEED_PIN, MOTOR_SPEED_STOPPED);
    analogWrite(MOTOR_REAR_RIGHT_SPEED_PIN, MOTOR_SPEED_STOPPED);
}

void motor_stop() {
    analogWrite(MOTOR_FRONT_LEFT_SPEED_PIN, MOTOR_SPEED_STOPPED);
    analogWrite(MOTOR_REAR_LEFT_SPEED_PIN, MOTOR_SPEED_STOPPED);
    analogWrite(MOTOR_FRONT_RIGHT_SPEED_PIN, MOTOR_SPEED_STOPPED);
    analogWrite(MOTOR_REAR_RIGHT_SPEED_PIN, MOTOR_SPEED_STOPPED);
}

String html = "<h1>RescueBot</h1>
<p>
<button id=\"updateMotordirection(0)\">Forward</button>
<button id=\"updateMotordirection(1)\">Left</button>
<button id=\"updateMotordirection(2)\">Right</button>
<button onclick=\"updateMotordirection(3)\">Backwards</button>
</p>
<script>
function updateMotordirection (direction) {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/api/update_motor?direction=' + direction, true);
    xhr.send();
}
</script>
";

ESP8266WebServer server(80);

void setup() {
    Serial.begin(9600);

    pinMode(MOTOR_FRONT_LEFT_DIRECTION_PIN, OUTPUT);
    pinMode(MOTOR_FRONT_LEFT_SPEED_PIN, OUTPUT);

    pinMode(MOTOR_REAR_LEFT_DIRECTION_PIN, OUTPUT);
    pinMode(MOTOR_REAR_LEFT_SPEED_PIN, OUTPUT);

    pinMode(MOTOR_FRONT_RIGHT_DIRECTION_PIN, OUTPUT);
    pinMode(MOTOR_FRONT_RIGHT_SPEED_PIN, OUTPUT);

    pinMode(MOTOR_REAR_RIGHT_DIRECTION_PIN, OUTPUT);
    pinMode(MOTOR_REAR_RIGHT_SPEED_PIN, OUTPUT);

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
        server.send(200, "text/html", html);
    });

    server.on("/api/update_motor", []() {
        int32_t motor_direction = atoi(server.arg("direction"));

        if (motor_direction == API_MOTOR_DIRECTION_FORWARD) {
            motor_move_forward();
        }
        if (motor_direction == API_MOTOR_DIRECTION_LEFT) {
            motor_turn_left();
        }
        if (motor_direction == API_MOTOR_DIRECTION_RIGHT) {
            motor_turn_right();
        }
        if (motor_direction == API_MOTOR_DIRECTION_BACKWARDS) {
            motor_move_backwards();
        }

        server.send(200, "application/json", "{\"message\":\"succesfull\"}");
    });

    server.begin();
}

void loop() {
    server.handleClient();
}
