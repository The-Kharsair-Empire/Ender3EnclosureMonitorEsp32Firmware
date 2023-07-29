#include <Arduino.h>
#include "wifi_connectivity.h"
#include "mqtt_client.h"

void setup() {
    wifi_setup();
    mqtt_setup();
}

void loop() {
    wifi_check_reconnect();
    mqtt_loop();

    // if (wifi_connected()) {
    //     Serial.println("Wifi Good");
    // } else {
    //     Serial.println("Wifi Not Good");
    // }

    // if (mqtt_client.connected()) {
    //     Serial.println("MQTT Good");
    // } else {
    //     Serial.println("MQTT Not Good");
    // }
}