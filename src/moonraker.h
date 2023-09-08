#pragma once
#ifndef MOONRAKER_H
#define MOONRAKER_H
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "config.h"

StaticJsonDocument<2048> device_moonraker_config;
bool moonraker_config_loaded = false;
const char* device_moonraker_config_file = "/moonraker.json";

const char* endpoint = "/printer/emergency_stop";

TaskHandle_t emergency_stop_task_handle;

// TODO: still gonna need some flag here (is_e_stop_issued, is_e_stop_waiting, is_e_stop_succeeded) to prevent from resending request

String get_endpoint() {
    if (!moonraker_config_loaded) {
        moonraker_config_loaded = load_config(device_moonraker_config, device_moonraker_config_file);
    }
    if (moonraker_config_loaded) {
        return device_moonraker_config["host"].as<String>() + String(endpoint);

    }
    else {
        return String("");
    }
}


void emergency_stop(void*) {
    HTTPClient http;
    
    
    String url = get_endpoint();
    http.begin(url.c_str());
    String dummy_payload("");


    while (1) {
        int http_code = http.POST(dummy_payload); // can be replaced with meaningful payload if the endpoint requires
        if (http_code != HTTP_CODE_OK) {
#ifdef SERIAL_DEBUG
            Serial.println("Emergency Stop Request Failed, Retrying");

#endif
        } else {
#ifdef SERIAL_DEBUG
            Serial.println("Emergency Stop Request Succeed");
#endif
            break;

        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
#ifdef SERIAL_DEBUG
    Serial.print("Cleaning up http resource and task running in core: ");
    Serial.println(xPortGetCoreID());
    Serial.print("Priority: ");
    Serial.println(uxTaskPriorityGet(NULL));

#endif

    http.end();
    vTaskDelete(NULL);
}


void start_emergency_stop_task() {
    
    xTaskCreatePinnedToCore(
        &emergency_stop,
        "Emergency Stop",
        10240,
        NULL, 
        20,
        &emergency_stop_task_handle,
        pro_cpu
    );
    
}




#endif