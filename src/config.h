#pragma once
#ifndef CONFIG_H
#define CONFIG_H
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "system.h"

bool stop_on_flame_detected = false;
bool stop_on_fume_detected = false;
bool stop_on_chamber_overheat = false;
bool chamber_overheat_stop_temperature = 100; //deg C



bool load_config(JsonDocument& json, const char* filename) {
    initialize_spiffs();
    if (SPIFFS.exists(filename)) {
        File config = SPIFFS.open(filename, FILE_READ);
        if (config) {
            DeserializationError error = deserializeJson(json, config);
            if (!error) {
#ifdef SERIAL_DEBUG
                initialize_serial();
                serializeJsonPretty(json, Serial);
                Serial.println();
#endif 
                config.close();
                return true;
                
            } else {
#ifdef SERIAL_DEBUG
                initialize_serial();
                Serial.println("JSON Load Error");
#endif 
                config.close();
                return false;
            }
        }
    } else {
#ifdef SERIAL_DEBUG
        initialize_serial();
        Serial.println("File Not Exist");
#endif        
        return false;
    }
    return false;
}

bool save_config(const JsonDocument& json, const char* filename) {
    initialize_spiffs();
    File config;
    
    config = SPIFFS.open(filename, FILE_WRITE);

    if (config) {
        serializeJson(json, config);
        config.close();
        return false; 
    } else {
        config.close();
        return false; 
    }
}

#endif