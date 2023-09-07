#pragma once
#ifndef TWO_COLOR_LED_H
#define TWO_COLOR_LED_H

#include <Arduino.h>
#include "flame_sensor.h"

#define RED_PIN 18
#define GREEN_PIN 19

bool is_led_setup = false;

void setup_light_pins() {
    if (!is_led_setup) {
        pinMode (RED_PIN, OUTPUT);
        pinMode (GREEN_PIN, OUTPUT);
        is_led_setup = true;
    }
}

void led_off() {
    setup_light_pins();
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
}

void led_red_on() {
    setup_light_pins();
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
}

void led_green_on() {
    setup_light_pins();
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
}

void led_alert_loop(void*) {
    setup_light_pins();
    while (1) {
        if (is_flame_detected_digital()) {
            led_red_on();
        } else {
            led_green_on(); 
        }
    }
}

#endif