#pragma once
#ifndef BUZZER_H
#define BUZZER_H
#include <Arduino.h>

#define ALARM_PIN 17

// pass param by:
// int second = 5;
// ... (void *) &second 
TaskHandle_t buzzer_task_handle;
int param_play_second = 3;

void sound_alarm(void* param) {
    int* play_second = (int*) param;
    pinMode(ALARM_PIN, OUTPUT);
    digitalWrite(ALARM_PIN, HIGH);
    vTaskDelay((*play_second) / portTICK_PERIOD_MS);
    digitalWrite(ALARM_PIN, LOW);
    vTaskDelete(NULL);
}

bool state_on = false;

void sound() {
    pinMode(ALARM_PIN, OUTPUT);
    digitalWrite(ALARM_PIN, HIGH);
}

void unsound() {
    digitalWrite(ALARM_PIN, LOW);
}

void toggle() {
    if (state_on) unsound();
    else sound();

 
}



#endif