// Include the libraries, the config and the website headers
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "config.hpp"
#include "website.hpp"

#define DEBUG

// #############################################################################
// ############################# I2C COMMUNICATION #############################
// #############################################################################

// The i2c variables
#define I2C_ADDRESS 10
#define PROTOCOL_REQUEST_MESSAGE_LENGTH 6
#define PROTOCOL_BORDER_NOT_FOUND 0
#define PROTOCOL_BORDER_FOUND 1

uint8_t distance_to_object;
uint8_t distance_to_left;
uint8_t distance_to_right;
uint8_t distance_to_ground;
uint8_t ir_left;
uint8_t ir_right;

// A function that request the data and updates some variables
void update_data_from_slave() {
    if (Wire.requestFrom(I2C_ADDRESS, PROTOCOL_REQUEST_MESSAGE_LENGTH) == PROTOCOL_REQUEST_MESSAGE_LENGTH) {
        distance_to_object = Wire.read();
        distance_to_left = Wire.read();
        distance_to_right = Wire.read();
        distance_to_ground = Wire.read();
        ir_left = Wire.read();
        ir_right = Wire.read();

        #ifdef DEBUG
            printf("distance_to_object: %d, distance_to_left: %d, distance_to_right: %d, distance_to_ground: %d, ir_left: %d, ir_right: %d\n", distance_to_object, distance_to_left, distance_to_right, distance_to_ground, ir_left, ir_right);
        #endif
    }
}

// A function that ints the i2c and gets the data
void i2c_init() {
    Wire.begin();
    update_data_from_slave();
}

// #############################################################################
// #################################### STATE ##################################
// #############################################################################

// The motor pins
#define MOTOR_LEFT_FORWARD_PIN D0
#define MOTOR_LEFT_BACKWARD_PIN D6
#define MOTOR_LEFT_ENABLE_PIN D7

#define MOTOR_RIGHT_FORWARD_PIN D3
#define MOTOR_RIGHT_BACKWARD_PIN D4
#define MOTOR_RIGHT_ENABLE_PIN D5

// Inits the motor pins
void motor_init() {
    pinMode(MOTOR_LEFT_FORWARD_PIN, OUTPUT);
    pinMode(MOTOR_LEFT_BACKWARD_PIN, OUTPUT);
    pinMode(MOTOR_LEFT_ENABLE_PIN, OUTPUT);

    pinMode(MOTOR_RIGHT_FORWARD_PIN, OUTPUT);
    pinMode(MOTOR_RIGHT_BACKWARD_PIN, OUTPUT);
    pinMode(MOTOR_RIGHT_ENABLE_PIN, OUTPUT);
}

// Moves the car forward
void motor_move_forward() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

// Turns the car to the left
void motor_turn_left() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

// Turns the cars to the right
void motor_turn_right() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

// Moves the car backwards
void motor_move_backward() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, HIGH);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, HIGH);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, HIGH);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, HIGH);
}

// Stops all motors
void motor_stop() {
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_LEFT_ENABLE_PIN, LOW);

    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, LOW);
    digitalWrite(MOTOR_RIGHT_ENABLE_PIN, LOW);
}

// #############################################################################
// #################################### STATE ##################################
// #############################################################################

// Variables to hold the last border position
#define BORDER_UNKOWN 0
#define BORDER_LEFT 1
#define BORDER_RIGHT 2

uint8_t last_border_position = BORDER_UNKOWN;

// The different states
#define STATE_STILL 0
#define STATE_MOVE_FORWARD 1

#define STATE_AVOID_LEFT_BORDER 2
#define STATE_AVOID_RIGHT_BORDER 3

#define STATE_AVOID_OBJECT_MOVE_BACKWARD 4
#define STATE_AVOID_OBJECT_TURN_LEFT_FIRST 5
#define STATE_AVOID_OBJECT_TURN_RIGHT_FIRST 6
#define STATE_AVOID_OBJECT_MOVE_FORWARD 7
#define STATE_AVOID_OBJECT_TURN_LEFT_SECOND 8
#define STATE_AVOID_OBJECT_TURN_RIGHT_SECOND 9

#define STATE_AVOID_CLIFF_MOVE_BACKWARD 10
#define STATE_AVOID_CLIFF_TURN_LEFT 11
#define STATE_AVOID_CLIFF_TURN_RIGHT 12

#define STATE_OVERIDE_MOVE_FORWARD 13
#define STATE_OVERIDE_TURN_LEFT 14
#define STATE_OVERIDE_TURN_RIGHT 15
#define STATE_OVERIDE_MOVE_BACKWARD 16

#define STATE_TUNNEL_FORWARD 17

// The state variables
bool auto_control = false;
uint8_t state = STATE_STILL;
uint32_t state_time = millis();

// A function that sets the state and reflects it to the motors
void set_state(uint8_t new_state) {
    state = new_state;
    state_time = millis();

    // Standard states
    if (state == STATE_STILL) {
        motor_stop();
    }

    if (state == STATE_MOVE_FORWARD) {
        motor_move_forward();
    }

    // Avoid left border
    if (state == STATE_AVOID_LEFT_BORDER) {
        last_border_position = BORDER_LEFT;
        motor_turn_right();
    }

    // Avoid right border
    if (state == STATE_AVOID_RIGHT_BORDER) {
        last_border_position = BORDER_RIGHT;
        motor_turn_left();
    }

    // Avoid objects
    if (state == STATE_AVOID_OBJECT_MOVE_BACKWARD) {
        motor_move_backward();
    }

    if (state == STATE_AVOID_OBJECT_TURN_LEFT_FIRST) {
        motor_turn_left();
    }

    if (state == STATE_AVOID_OBJECT_TURN_RIGHT_FIRST) {
        motor_turn_right();
    }

    if (state == STATE_AVOID_OBJECT_MOVE_FORWARD) {
        motor_move_forward();
    }

    if (state == STATE_AVOID_OBJECT_TURN_LEFT_SECOND) {
        motor_turn_left();
    }

    if (state == STATE_AVOID_OBJECT_TURN_RIGHT_SECOND) {
        motor_turn_right();
    }

    // Avoid cliffs
    if (state == STATE_AVOID_CLIFF_MOVE_BACKWARD) {
        motor_move_backward();
    }

    if (state == STATE_AVOID_CLIFF_TURN_LEFT) {
        motor_turn_left();
    }

    if (state == STATE_AVOID_CLIFF_TURN_RIGHT) {
        motor_turn_right();
    }

    // Overide motor
    if (state == STATE_OVERIDE_MOVE_FORWARD) {
        motor_move_forward();
    }

    if (state == STATE_OVERIDE_TURN_LEFT) {
        motor_turn_left();
    }

    if (state == STATE_OVERIDE_TURN_RIGHT) {
        motor_turn_right();
    }

    if (state == STATE_OVERIDE_MOVE_BACKWARD) {
        motor_move_backward();
    }

    // Tunnel protocol
    if (state == STATE_TUNNEL_FORWARD) {
        motor_move_forward();
    }
}

// A function that updates the state with the data
void update_state() {
    uint32_t time_passed = millis() - state_time;

    // // Tunnel protocol
    // if (distance_to_left < 20 && distance_to_right < 20) {
    //     set_state(STATE_TUNNEL_FORWARD);
    //     return;
    // }

    // Avoid left border
    if (ir_left == PROTOCOL_BORDER_FOUND) {
        set_state(STATE_AVOID_LEFT_BORDER);
    }

    if (state == STATE_AVOID_LEFT_BORDER && ir_left == HIGH) {
        set_state(STATE_MOVE_FORWARD);
    }

    // Avoid right border
    if (ir_right == PROTOCOL_BORDER_FOUND) {
        set_state(STATE_AVOID_RIGHT_BORDER);
    }

    if (state == STATE_AVOID_RIGHT_BORDER && ir_right == HIGH) {
        set_state(STATE_MOVE_FORWARD);
    }

    // // Avoid objects
    // if (distance_to_object < 25) {
    //     set_state(STATE_AVOID_OBJECT_MOVE_BACKWARD);
    // }

    // if (state == STATE_AVOID_OBJECT_MOVE_BACKWARD && time_passed > 1000) {
    //     if (last_border_position == BORDER_LEFT) {
    //         set_state(STATE_AVOID_OBJECT_TURN_RIGHT_FIRST);
    //     }
    //     if (last_border_position == BORDER_RIGHT) {
    //         set_state(STATE_AVOID_OBJECT_TURN_LEFT_FIRST);
    //     }
    // }

    // if (state == STATE_AVOID_OBJECT_TURN_LEFT_FIRST && time_passed > 1000) {
    //     set_state(STATE_AVOID_OBJECT_MOVE_FORWARD);
    // }

    // if (state == STATE_AVOID_OBJECT_TURN_RIGHT_FIRST && time_passed > 1000) {
    //     set_state(STATE_AVOID_OBJECT_MOVE_FORWARD);
    // }

    // if (state == STATE_AVOID_OBJECT_MOVE_FORWARD && time_passed > 1000) {
    //     if (last_border_position == BORDER_LEFT) {
    //         set_state(STATE_AVOID_OBJECT_TURN_LEFT_SECOND);
    //     }
    //     if (last_border_position == BORDER_RIGHT) {
    //         set_state(STATE_AVOID_OBJECT_TURN_RIGHT_SECOND);
    //     }
    // }

    // if (state == STATE_AVOID_OBJECT_TURN_LEFT_SECOND && time_passed > 1000) {
    //     set_state(STATE_MOVE_FORWARD);
    // }

    // if (state == STATE_AVOID_OBJECT_TURN_RIGHT_SECOND && time_passed > 1000) {
    //     set_state(STATE_MOVE_FORWARD);
    // }

    // // Avoid cliffs
    // if (distance_to_ground > 2) {
    //     set_state(STATE_AVOID_CLIFF_MOVE_BACKWARD);
    // }

    // if (state == STATE_AVOID_CLIFF_MOVE_BACKWARD && time_passed > 1000) {
    //     if (last_border_position == BORDER_LEFT) {
    //         set_state(STATE_AVOID_CLIFF_TURN_RIGHT);
    //     }
    //     if (last_border_position == BORDER_RIGHT) {
    //         set_state(STATE_AVOID_CLIFF_TURN_RIGHT);
    //     }
    // }

    // if (state == STATE_AVOID_CLIFF_TURN_LEFT && time_passed > 1000) {
    //     set_state(STATE_MOVE_FORWARD);
    // }

    // if (state == STATE_AVOID_CLIFF_TURN_RIGHT && time_passed > 1000) {
    //     set_state(STATE_MOVE_FORWARD);
    // }
}

// #############################################################################
// ############################# WIFI / WEB SERVER #############################
// #############################################################################

// A function that connects to wifi via the config settings
void wifi_connect() {
    Serial.print("\nConnecting to ");
    Serial.print(wifi_ssid);
    Serial.println("...");

    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();

    Serial.print("RescueBot ");
    Serial.print(rescuebot_name);
    Serial.println(" connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

// The web server object
ESP8266WebServer webserver(80);

// A function that inits the webs server
void webserver_init() {
    // Return the web interface when you go to the root path
    webserver.on("/", []() {
        webserver.send(200, "text/html", src_website_html, src_website_html_len);
    });

    // Begin the web server
    webserver.begin();
}

// The web socket server object
WebSocketsServer websocket(81);

// The JSON library data pool
StaticJsonDocument<256> doc;

// The websocket event handler function
void websocket_event(uint8_t num, WStype_t type, uint8_t *payload, size_t length){
    if (type == WStype_CONNECTED) {
        // Send data to new client
        String client_message;
        doc["rescuebot_name"] = rescuebot_name;
        doc["auto_control"] = auto_control;
        doc["state"] = state;
        serializeJson(doc, client_message);
        websocket.sendTXT(num, client_message);
    }

    if (type == WStype_TEXT) {
        // Update data from json message
        deserializeJson(doc, payload, length);
        auto_control = doc["auto_control"];
        state = doc["state"];

        // Send change message to all clients
        String client_message;
        doc["rescuebot_name"] = rescuebot_name;
        doc["auto_control"] = auto_control;
        doc["state"] = state;
        serializeJson(doc, client_message);
        websocket.broadcastTXT(client_message);
    }
}

// Init the websockets server
void websocket_init() {
    websocket.begin();
    websocket.onEvent(websocket_event);
}

// #############################################################################
// ######################## ARDUINO LIBRARY SETUP & LOOP #######################
// #############################################################################

// The start of our program
void setup() {
    // Init the serial output
    Serial.begin(115200);

    // Init all the things
    i2c_init();
    motor_init();
    wifi_connect();
    webserver_init();
    websocket_init();
}

// The program loop
void loop() {
    // Handle websocket clients
    websocket.loop();

    // Handle any http clients
    webserver.handleClient();

    // Get new data from the slave
    update_data_from_slave();

    // Update the state with the data when auto control is on
    if (auto_control) {
        update_state();
    }
}
