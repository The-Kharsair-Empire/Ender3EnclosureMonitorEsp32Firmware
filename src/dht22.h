#pragma once
#ifndef DHT_22_H
#define DHT_22_H

#include <DHT.h>

#define DHT_DATA 16
#define DHT_TYPE DHT22

volatile double filtered_dht_temperature;
volatile double filtered_dht_humidity;

DHT dht22(DHT_DATA, DHT_TYPE);

float dht_temperature_alpha = 0.2f; // low pass constant for temperature, default 0.2.
float dht_humidity_alpha = 0.2f;

unsigned int dht_sample_interval = 1000; // 1s

// void update_settings() {} // changing alphas and interval from mqtt


double get_dht_temperature() {
    return filtered_dht_temperature;
}

double get_dht_humidity() {
    return filtered_dht_humidity;
}

void dht_sample_loop(void*) {

    dht22.begin();
    while (isnan(dht22.readTemperature())) {
        vTaskDelay(dht_sample_interval / portTICK_PERIOD_MS);
    }
    while (isnan(dht22.readHumidity())) {
        vTaskDelay(dht_sample_interval / portTICK_PERIOD_MS);
    }
    // initial read otherwise history will be 0
    filtered_dht_humidity = dht22.readHumidity();
    filtered_dht_temperature = dht22.readTemperature();

    while (1) {
        float humidity = dht22.readHumidity();
        float temperature = dht22.readTemperature();
        if (!isnan(humidity)) {
            filtered_dht_humidity += (humidity - filtered_dht_humidity) * dht_humidity_alpha;
        }
        if (!isnan(temperature)) {
            filtered_dht_temperature += (temperature - filtered_dht_temperature) * dht_temperature_alpha;
        }
        
        vTaskDelay(dht_sample_interval / portTICK_PERIOD_MS);
    }
}

#endif