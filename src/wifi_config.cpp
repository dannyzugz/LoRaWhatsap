//
// Created by Dani on 9/19/2023.
//

#include <WiFi.h>
#include "wifi_config.h"

#define SSID "LoRaChat1"
#define PASSWORD "12345678"

void setWifi() {
    // Configura el ESP32 en modo AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASSWORD);
}