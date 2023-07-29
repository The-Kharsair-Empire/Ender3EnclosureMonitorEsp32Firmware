#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define SERIAL_DEBUG
// #define ERASE_DEVICE_CONFIG
// #define ERASE_WIFI_CONFIG
#define USE_STATIC_IP

bool __is_on_pairing_mode = true;
bool __is_spiffs_initialized = false;
bool __is_serial_initialized = false;
bool __is_reset_pin_setup = false;

bool is_spiffs_initialized() {
    return __is_spiffs_initialized;
}


bool is_serial_initialized() {
    return __is_serial_initialized;
}


void disable_spiffs() {
    __is_spiffs_initialized = false;
    SPIFFS.end();
}

void disable_serial() {
    __is_serial_initialized = false;
    Serial.end();
}



bool initialize_spiffs() {
    if (!__is_spiffs_initialized) {
        if(SPIFFS.begin()) {
            __is_spiffs_initialized = true;
            return true;
        } else {
            return false;
        }
    }
    return true;
}

void initialize_serial() {
    if (!__is_serial_initialized) {
        Serial.end();
        Serial.begin(115200);
        __is_serial_initialized = true;
    }
    
}





#endif