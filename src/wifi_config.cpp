//
// Created by Dani on 9/19/2023.
//

#include <WiFi.h>
#include "wifi_config.h"

const char* ssid = "LoRaChat";
const char* password = "12345";

void setWifi(){

    // Configura el ESP32 en modo AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
}