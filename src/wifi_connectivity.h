#pragma once
#ifndef WIFI_CONNECTIVITY_H
#define WIFI_CONNECTIVITY_H
#include <WiFi.h>
#include "config.h"


const char* device_wifi_config_file = "/wifi.json";
StaticJsonDocument<512> device_wifi_config;
bool wifi_config_loaded = false;


char wifi_ssid[50];
char wifi_psw[20];


#ifdef USE_STATIC_IP
char gateway[16];
char static_ip[16];
char dns[16];
char subnet[16];
#endif

bool wifi_connected() {
    return WiFi.status() == WL_CONNECTED;
}

void wifi_check_reconnect_loop() {

    delay(1000);
    while (1) {
        if (!wifi_connected()) {
            WiFi.disconnect();
            WiFi.reconnect();
            delay(4000);

            if (!wifi_connected()) {
                ESP.restart();
            }
        }
    }
    
}

bool wifi_creds_setup() {

    if (!wifi_config_loaded) {
        wifi_config_loaded = load_config(device_wifi_config, device_wifi_config_file);
    }
    if (wifi_config_loaded) {
        if (device_wifi_config.containsKey("ssid") 
            && device_wifi_config.containsKey("password")
#ifdef USE_STATIC_IP
            && device_wifi_config.containsKey("ip")
            && device_wifi_config.containsKey("gateway")
            && device_wifi_config.containsKey("dns")
            && device_wifi_config.containsKey("subnet")
#endif      
            ) {

            strcpy(wifi_ssid, device_wifi_config["ssid"]);
            strcpy(wifi_psw, device_wifi_config["password"]);

#ifdef USE_STATIC_IP
            strcpy(static_ip, device_wifi_config["ip"]);
            strcpy(gateway, device_wifi_config["gateway"]);
            strcpy(dns, device_wifi_config["dns"]);
            strcpy(subnet, device_wifi_config["subnet"]);
#endif
            return true;

        } else {
#ifdef SERIAL_DEBUG
            initialize_serial();
            Serial.println("Missing Wifi Config, refer to function wifi_creds_setup");
#endif    
            return false;
        }

    }
#ifdef SERIAL_DEBUG
    else {
        Serial.println("Wifi Config Not Loaded");
    }
#endif  
    return false;
}

#ifdef USE_STATIC_IP
bool parse_ip_addr(char* ip_str, IPAddress& out_ip) {
    if (out_ip.fromString(ip_str)) {
        return true;
    } else {
        return false;
    }

}
#endif


void wifi_setup(void*) {
    WiFi.disconnect();
    
    
    if (wifi_creds_setup()) {
        WiFi.mode(WIFI_STA);
#ifdef USE_STATIC_IP
        IPAddress ip_static_ip, ip_subnet, ip_dns, ip_gateway;
        if (parse_ip_addr(static_ip, ip_static_ip) && parse_ip_addr(subnet, ip_subnet) 
            && parse_ip_addr(dns, ip_dns) && parse_ip_addr(gateway, ip_gateway)) {
            WiFi.config(ip_static_ip, ip_gateway, ip_subnet, ip_dns);
        }
#endif
        WiFi.begin(wifi_ssid, wifi_psw);
    }
#ifdef SERIAL_DEBUG
    else {
        initialize_serial();
        Serial.println("Cannot connect to wifi due to supplied config file issue, proceed without wifi");
    }           
#endif  
    while (!wifi_connected()) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    wifi_check_reconnect_loop();
}



#endif