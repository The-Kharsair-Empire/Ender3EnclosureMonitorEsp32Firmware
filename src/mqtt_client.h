#pragma once
#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <PubSubClient.h>
// #include <WiFiClientSecure.h>
#include "config.h"
#include "wifi_connectivity.h"
#include <ArduinoJson.h>
#include "moonraker.h"
#include "dht22.h"
#include "ds18b20.h"
#include "flame_sensor.h"

// WiFiClientSecure client; // May depends on Server, Bambulab X1C might need secure client
WiFiClient client;
PubSubClient mqtt_client(client);

StaticJsonDocument<2048> device_mqtt_config;
bool mqtt_config_loaded = false;
const char* device_mqtt_config_file = "/mqtt.json";

char host_ip[16];
char device_id[50];
char device_name[50];
char user[50];
char password[50];
int32_t port;

StaticJsonDocument<2048> payload_buffer; // payload buffer;
String payload_string;

JsonObject cmd_topics;
JsonObject stat_topics;

//////// variable updated by mqtt subscription
bool e_stop_on_flame_detected = false;
bool e_stop_on_mq2_fume_detected = false;
bool e_stop_on_mq135_fume_detected = false;
bool e_stop_on_overheat = false;
float e_stop_overheat_temperature = 100.f; // C
bool should_publish = true;
unsigned int publish_interval = 10000; //ms
///////////////

unsigned long prev = 0, now = 0;

void load_mqtt_config() {
    if (!mqtt_config_loaded) {
        mqtt_config_loaded = load_config(device_mqtt_config, device_mqtt_config_file);

    }
    if (mqtt_config_loaded) {
        if (device_mqtt_config.containsKey("port") 
            && device_mqtt_config.containsKey("server")
            && device_mqtt_config.containsKey("id")
            && device_mqtt_config.containsKey("name")
            && device_mqtt_config.containsKey("user")
            && device_mqtt_config.containsKey("psw")
            && device_mqtt_config.containsKey("stat")
            && device_mqtt_config.containsKey("cmd")
            ) {

            port = device_mqtt_config["port"].as<int32_t>();
            strcpy(host_ip, device_mqtt_config["server"]);
            strcpy(device_id, device_mqtt_config["id"]);
            strcpy(device_name, device_mqtt_config["name"]);
            strcpy(user, device_mqtt_config["user"]);
            strcpy(password, device_mqtt_config["psw"]);

            cmd_topics = device_mqtt_config["cmd"].as<JsonObject>();
            stat_topics = device_mqtt_config["stat"].as<JsonObject>();

            String constructed_topic = "";
            for (const JsonPair& kv: cmd_topics) {
                constructed_topic = "device/";
                constructed_topic += device_mqtt_config["id"].as<const char*>();
                constructed_topic += "/cmd/";
                constructed_topic += kv.value().as<const char*>();
                cmd_topics[kv.key().c_str()] = String(constructed_topic.c_str()); // String() make a copy, 
                                                                                // neccessary to prevent memeory leak
#ifdef SERIAL_DEBUG
                Serial.print("Cmd Topic: ");
                Serial.println(cmd_topics[kv.key().c_str()].as<const char*>());
#endif
            }

            for (const JsonPair& kv: stat_topics) {
                constructed_topic = "device/";
                constructed_topic += device_mqtt_config["id"].as<const char*>();
                constructed_topic += "/stat/";
                constructed_topic += kv.value().as<const char*>();
                stat_topics[kv.key().c_str()] = String(constructed_topic.c_str());
#ifdef SERIAL_DEBUG
                Serial.print("Stat Topic: ");
                Serial.println(stat_topics[kv.key().c_str()].as<const char*>());
#endif
            }
        }
#ifdef SERIAL_DEBUG
        
        else {
            Serial.println("Missing MQTT Config, refer to function load_mqtt_config");
        }
        Serial.print("server ip: "); Serial.println(host_ip);
        Serial.print("server port: "); Serial.println(port);
        Serial.print("name: "); Serial.println(device_name);
        Serial.print("user: "); Serial.println(user);
        Serial.print("pass: "); Serial.println(password);
#endif        
    }
#ifdef SERIAL_DEBUG
    else {
        Serial.println("MQTT Config Not Loaded");
    }
#endif  
}

void callback(char* topic, byte* payload, unsigned int length) {

#ifdef SERIAL_DEBUG
    Serial.println(topic);
#endif

    payload_buffer.clear();
    DeserializationError error = deserializeJson(payload_buffer, payload, length);

    if (error) {
#ifdef SERIAL_DEBUG
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
#endif
        return;
    }

//     if (payload_buffer.containsKey("publish")) {
//         if (strcmp(payload_buffer["publish"], "yes") == 0) {
//             should_publish = true;
//         } else if (strcmp(payload_buffer["publish"], "no") == 0) {
//             should_publish = false;
//         } else {
//             Serial.println("unknowned command");
//         }
        
//     }

//     if (payload_buffer.containsKey("stop")) {
//         if (strcmp(payload_buffer["stop"], "yes") == 0) {
// #ifdef SERIAL_DEBUG
//             Serial.println("Emergency Stop Starting");
// #endif
//             start_emergency_stop_task();
//         } else {
//             Serial.println("unknowned command");
//         }
        
//     }


    if (strcmp(topic, cmd_topics["settings"].as<const char*>()) == 0) {
        // {enabled: true, interval: 10000}
        if (payload_buffer.containsKey("publish_interval")) {
            // {interval: 10000}
        }
    }

    // serializeJsonPretty(payload_buffer, Serial);
    // Serial.println();
}

void subscribe() {
    for (const JsonPair& kv: cmd_topics) {
        const char* topic = kv.value().as<const char*>();
        mqtt_client.subscribe(topic);
#ifdef SERIAL_DEBUG
        Serial.print("Subscribing Topic: ");
        Serial.println(topic);
#endif
    }
    // mqtt_client.subscribe("state/test_json");
    // Serial.println("Subscribing to test");
}

void reconnect() {
    while(! mqtt_client.connected()) {
        if (mqtt_client.connect(device_name, user, password)) {
            subscribe();
        }
        else vTaskDelay(500 / portTICK_PERIOD_MS);
    }
#ifdef SERIAL_DEBUG
    Serial.println("mqtt connected");
#endif
    // sending birth message
    payload_buffer.clear();
    payload_string.clear();

    payload_buffer["birth"] = "true";
    payload_buffer["timestamp"] = millis(); 
    
    serializeJson(payload_buffer, payload_string);
    mqtt_client.publish(stat_topics["birth"].as<const char*>(), 
                        payload_string.c_str(), 
                        true);
    
}


void mqtt_loop() {
    delay(1000);
    while (1) {
        if (wifi_connected() && !mqtt_client.connected()) {
            reconnect();
#ifdef SERIAL_DEBUG
            Serial.println("retry mqtt connection");
#endif
        } else if (!wifi_connected()) {
            if (mqtt_client.connected())
                mqtt_client.disconnect();
            continue;
        }
        mqtt_client.loop();

        
        if (should_publish) {
            now = millis();
            if (now - prev > publish_interval) {
                prev = now;
                
                for (const JsonPair& kv: stat_topics) {
                    const char* topic_key = kv.key().c_str();
                    if (strcmp(topic_key, "birth") != 0) {
                        payload_buffer.clear();
                        payload_string.clear();

                        if (strcmp(topic_key, "humidity") == 0) {
                            payload_buffer["timestamp"] = millis();

                            double humidity = get_dht_humidity();
                            payload_buffer["humidity"] = humidity;
        
                        } else if (strcmp(topic_key, "temperature") == 0) {
                            payload_buffer["timestamp"] = millis();
                            double temperature = get_dht_temperature();
                            payload_buffer["temperature_dht"] = temperature;
                            temperature = get_ds18b20_temperature();
                            payload_buffer["temperature_ds18b20"] = temperature;
                            // temperature = get_cpu_temp_in_c();
                            // payload_buffer["cpu_temperature"] = temperature;
                        } else if (strcmp(topic_key, "flame") == 0) {
                            payload_buffer["timestamp"] = millis();
                            payload_buffer["flame_analog"] = get_flame_level_analog();
                            payload_buffer["flame_digital_detected"] = is_flame_detected_digital();
                        } else if (strcmp(topic_key, "fume") == 0) {
                            payload_buffer["timestamp"] = millis();
                            payload_buffer["fume_mq2"] = 0;
                            payload_buffer["fume_mq135"] = 0;
                        }
                    
                        serializeJson(payload_buffer, payload_string);
                        mqtt_client.publish(kv.value().as<const char*>(), 
                            payload_string.c_str(), 
                            true);
                  
                    }
                    
                }
                
            }
        }
    }
}

// publishing strategy, every time published, keep the published sensor value, then compare new value to it
// if new value is different, then publish.
// also set minimum interval.


void mqtt_setup(void*) {
    load_mqtt_config();
    // client.setInsecure();
    while (!wifi_connected()) { // wait for wifi to connect first
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    mqtt_client.setBufferSize(2048);
    mqtt_client.setServer(host_ip, port);
    reconnect(); // it will crash if wifi is not connected
    mqtt_client.setCallback(callback);

    mqtt_loop();
}


#endif