#pragma once
#ifndef DHT_22_H
#define DHT_22_H

#include <DHT.h>

#define DHT_DATA 16
#define DHT_TYPE DHT22

double filtered_dht_temperature;
double filtered_dht_humidity;

bool is_dht_setup = false;

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
    if (!is_dht_setup) {
        dht22.begin();
    }

    while (1) {
        filtered_dht_humidity += (dht22.readHumidity() - filtered_dht_humidity) * dht_humidity_alpha;
        filtered_dht_temperature += (dht22.readTemperature() - filtered_dht_temperature) * dht_temperature_alpha;
        vTaskDelay(dht_sample_interval / portTICK_PERIOD_MS);
    }
}



#endif