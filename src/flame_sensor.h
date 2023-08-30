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

// reference code from arduino test project below:

// #define FLAME A3
// #define FLAME_DIGITAL 2
// #define ALARM 8
// #define LED 13

// float fire_level;
// int fire;

// int interval = 1000;
// unsigned long prev = 0, now = 0;

// void setup() {
//     Serial.begin(9600);
//     pinMode(FLAME, INPUT);
//     pinMode(FLAME_DIGITAL, INPUT);
//     pinMode(ALARM, OUTPUT);
//     pinMode(LED, OUTPUT);

// }


// void loop() {
//     fire_level = analogRead(FLAME);
//     Serial.println(fire_level);

//     int intensity = map(fire_level, 0, 1023, 0, 7);

//     interval = 2 * 100 * intensity;

//     fire = digitalRead(FLAME_DIGITAL);

//     if (fire == HIGH) {
//         digitalWrite(ALARM, HIGH);
//     } else {
//         digitalWrite(ALARM, LOW);
//     }

//     // now = millis();
//     // if (now - prev > interval) {
//     //     prev = now;
//     //     digitalWrite(ALARM, HIGH);
//     //     delay(100);
//     //     digitalWrite(ALARM, LOW);
//     // }


//     delay(500);

// }



#endif