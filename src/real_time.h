#pragma once
#ifndef REAL_TIME_H
#define REAL_TIME_H
#include <Arduino.h>
#include <time.h>


const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 36000;
const int   daylightOffset_sec = 3600;

configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

String get_local_time() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "";
    }

    
    
}

#endif