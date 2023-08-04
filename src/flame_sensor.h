#pragma once
#ifndef FLAME_SENSOR_H
#define FLAME_SENSOR_H
#include <Arduino.h>
// RED type of flame module, which digital read gives high when flame detectec


#define FLAME_ANALOG 35
#define FLAME_DIGITAL 25

#define ANALOG_READING_RANGE_MAX 4095

bool is_flame_sensor_setup = false;

float flame_level;
int flame_detected;

void flame_sensor_setup() {
    pinMode(FLAME_ANALOG, INPUT);
    pinMode(FLAME_DIGITAL, INPUT);
    is_flame_sensor_setup = true;
}

bool is_flame_detected_digital() {
    if (!is_flame_sensor_setup) flame_sensor_setup();
    return digitalRead(FLAME_DIGITAL) == HIGH;
}




#endif