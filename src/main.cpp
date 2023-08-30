#include <Arduino.h>
#include "wifi_connectivity.h"
#include "mqtt_client.h"
#include "config.h"

TaskHandle_t wifi_task;
TaskHandle_t mqtt_task;
TaskHandle_t dht_task;
TaskHandle_t ds18b20_task;

void setup() {
#ifdef SERIAL_DEBUG
    initialize_serial();
    
    Serial.print("running main loop in core: ");
    Serial.println(xPortGetCoreID());
    Serial.print("Priority: ");
    Serial.println(uxTaskPriorityGet(NULL));
#endif
    vTaskDelay(500 / portTICK_PERIOD_MS);

    xTaskCreatePinnedToCore(
        &wifi_setup,
        "WiFi Task",
        10240,
        NULL,
        1,
        &wifi_task,
        app_cpu
    );

    xTaskCreatePinnedToCore(
        &mqtt_setup,
        "MQTT Task",
        10240,
        NULL,
        1,
        &mqtt_task,
        app_cpu
    );

    xTaskCreatePinnedToCore(
        &dht_sample_loop,
        "DHT22 Task",
        3072,
        NULL,
        1,
        &dht_task,
        request_cpu
    );

    xTaskCreatePinnedToCore(
        &ds18b20_sample_loop,
        "DS18B20 Task",
        2048,
        NULL,
        1,
        &ds18b20_task,
        request_cpu
    );

    vTaskDelete(NULL); // setup and loop task should terminate here.
}

void loop() {
#ifdef SERIAL_DEBUG
    Serial.println("Code should never gets here");
#endif
}