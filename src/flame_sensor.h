#pragma once
#ifndef FLAME_SENSOR_H
#define FLAME_SENSOR_H
#include <Arduino.h>
// RED type of flame module, which digital read gives high when flame detectec


#define FLAME_ANALOG 35
#define FLAME_DIGITAL 25

#define ANALOG_READING_RANGE_MAX 4095

bool is_flame_sensor_setup = false;

volatile float filtered_flame_level;
float flame_analog_alpha = 0.2f;

int flame_analog_sample_interval = 1000;

void flame_sensor_setup() {
    if (!is_flame_sensor_setup) {
        pinMode(FLAME_ANALOG, INPUT);
        pinMode(FLAME_DIGITAL, INPUT);
        is_flame_sensor_setup = true;
    }
}

bool is_flame_detected_digital() {
    flame_sensor_setup();
    return digitalRead(FLAME_DIGITAL) == HIGH;
}

float get_flame_level_analog() {
    return filtered_flame_level;
}

void analog_flame_sample_loop(void*) {
    flame_sensor_setup();
    while (isnan(analogRead(FLAME_ANALOG))) {
        vTaskDelay(flame_analog_sample_interval / portTICK_PERIOD_MS);
    }

    filtered_flame_level = analogRead(FLAME_ANALOG);
    

    while (1) {
        float flame = analogRead(FLAME_ANALOG);
        if (!isnan(flame)) {
            filtered_flame_level += (flame - filtered_flame_level) * flame_analog_alpha;
        }
        vTaskDelay(flame_analog_sample_interval / portTICK_PERIOD_MS);
    }
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