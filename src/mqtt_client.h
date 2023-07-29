#pragma once
#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <PubSubClient.h>
// #include <WiFiClientSecure.h>
#include "config.h"
#include "wifi_connectivity.h"
#include <ArduinoJson.h>

// WiFiClientSecure client; // May depends on Server, Bambulab X1C might need secure client
WiFiClient client;
PubSubClient mqtt_client(client);

StaticJsonDocument<1024> device_mqtt_config;
bool mqtt_config_loaded = false;
const char* device_mqtt_config_file = "/mqtt.json";

// const size_t CAPACITY = JSON_ARRAY_SIZE(10);

char host_ip[16];
char device_id[50];
char device_name[50];
char user[50];
char password[50];
int32_t port;

StaticJsonDocument<2048> payload_buffer; // payload buffer;
String payload_string;

// StaticJsonDocument<CAPACITY> cmds;
// StaticJsonDocument<CAPACITY> stats;
JsonObject cmd_topics;
JsonObject stat_topics;

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

            cmd_topics = device_mqtt_config["cmd"].to<JsonObject>();
            stat_topics = device_mqtt_config["stat"].to<JsonObject>();

            String constructed_topic = "";
            for (JsonPair kv: cmd_topics) {

            }

            for (JsonPair kv: stat_topics) {
                
            }
            // load topics
            // topic should be: device/<id>/<category>(cmd/stat/other)/topic, construct it here
        }
#ifdef SERIAL_DEBUG
        
        else {
            initialize_serial();
            Serial.println("Missing MQTT Config, refer to function load_mqtt_config");
        }
        initialize_serial();
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

//some test code:
bool should_publish = false;
const char* publish_test_topic = "state/test_publish";
unsigned long prev = 0, now = 0;

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.println(topic);
    // Serial.println((char*) payload);
    payload_buffer.clear();
    DeserializationError error = deserializeJson(payload_buffer, payload, length);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    if (payload_buffer.containsKey("publish")) {
        if (strcmp(payload_buffer["publish"], "yes") == 0) {
            should_publish = true;
        } else if (strcmp(payload_buffer["publish"], "no") == 0) {
            should_publish = false;
        } else {
            Serial.println("unknowned command");
        }
        
    }

    serializeJsonPretty(payload_buffer, Serial);
    Serial.println();
}

void subscribe() {
    mqtt_client.subscribe("state/test_json");
    Serial.println("Subscribing");
}

void reconnect() {
    // while(! mqtt_client.connected()) {
    //     if (mqtt_client.connect(device_name, user, password)) {
    //         subscribe();
    //     }
    //     else delay(500);
    //     Serial.println("try");
    // }
    Serial.println("connected");
    if (mqtt_client.connect(device_name, user, password)) {
        subscribe();
        Serial.println("connected");
    }
    // delay(500);
    
}

void mqtt_setup() {
    load_mqtt_config();
    // client.setInsecure();
    mqtt_client.setBufferSize(2048);
    mqtt_client.setServer(host_ip, port);
    reconnect();
    mqtt_client.setCallback(callback);
}

void mqtt_loop() {
    if (wifi_connected() && !mqtt_client.connected()) {
        reconnect();
        Serial.println("retry mqtt connection");
    }
    mqtt_client.loop();

    //test code:
    now = millis();
    if (should_publish) {
        if (now - prev > 5000) {
            prev = now;
            payload_buffer.clear();
            payload_buffer["time"] = now;
            // payload_buffer["no"] = "no";
            payload_string.clear();
            serializeJson(payload_buffer, payload_string);
            mqtt_client.publish(publish_test_topic, payload_string.c_str());
        }
    }
}

// publishing strategy, every time published, keep the published sensor value, then compare new value to it
// if new value is different, then publish.
// also set minimum interval.

#endif