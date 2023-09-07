#pragma once
#ifndef DS_18_B20_H
#define DS_18_B20_H

#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20_DATA 26

OneWire oneWire(DS18B20_DATA);
DallasTemperature ds18b20(&oneWire);

volatile double filtered_ds18bb20_temperature;

unsigned int ds18bb20_sample_interval = 1000;
float ds18bb20_temperature_alpha = 0.2f; // low pass constant for temperature, default 0.2.

double get_ds18b20_temperature() {
    return filtered_ds18bb20_temperature;
}


void ds18b20_sample_loop(void*) {
    ds18b20.begin();
    ds18b20.requestTemperatures();
    while (isnan(ds18b20.getTempCByIndex(0))) {
        vTaskDelay(ds18bb20_sample_interval / portTICK_PERIOD_MS);
        ds18b20.requestTemperatures();
    }

    ds18b20.requestTemperatures();
    filtered_ds18bb20_temperature = ds18b20.getTempCByIndex(0) ;

    while (1) {
        ds18b20.requestTemperatures();
        float temperature = ds18b20.getTempCByIndex(0);
        if (!isnan(temperature)) {
            filtered_ds18bb20_temperature += (temperature - filtered_ds18bb20_temperature) * ds18bb20_temperature_alpha;
        }

        
        vTaskDelay(ds18bb20_sample_interval / portTICK_PERIOD_MS);
    }
}

#endif