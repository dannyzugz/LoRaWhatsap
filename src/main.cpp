#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "wifi_config.h"
#include "lora_config.h"
#include "LoRaWan_APP.h"
#include "SPIFFS.h"
#include "HT_SSD1306Wire.h"

void displayMcuInit();
void displaySendReceive();
SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

char my_data[255];
String incomingString;
int16_t myRssi = 0 ;
unsigned long tiempoAnterior = 0;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onEvent(AsyncWebSocket *myserver, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *midata,
             size_t len) {

    switch (type) {
        case WS_EVT_CONNECT:
//            Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
//                          client->remoteIP().toString().c_str());
// fulanito conectado con este Ip, mandar mensaje en broadcast para que el resto lo sepa
            break;
        case WS_EVT_DISCONNECT:
//            Serial.printf("WebSocket client #%u disconnected\n", client->id());

// fulanito con este Ip desconectado, mandar mensaje en broadcast para que el resto lo sepa
            break;
        case WS_EVT_DATA:
            // WebSocket data received
            if (len > 0) {
                // Print the received message to the Serial Monitor
//                char *data = (char *) malloc(sizeof(char) * (len + 1));
//                strncpy(data, (const char *) midata, len);
//                Serial.println(data);
//                free(data);
//                Serial.println(String((const char *) midata));
//                Serial.println(len);
                strncpy(my_data, reinterpret_cast<const char *>(midata), len + 1);
                my_data[len] = '\0';
//                Serial.println(String((const char *) my_data));
                //String((const char *) midata) = "";
              //  midata = nullptr;
                // Additional processing of the message
                // ...
            }
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            // WebSocket event types to handle if needed
            break;
    }
}


void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Mcu.begin();

    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    setWifi();

    ws.onEvent(onEvent);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.serveStatic("/", SPIFFS, "/");

    // Start the server
    server.begin();

    setLora();

    display.init();
    display.setFont(ArialMT_Plain_24);

    delay(500);
    display.clear();
//    display.drawString(0, 0, "Heltec.LoRa Initial success!");
    display.display();
}


void loop() {

    if (Serial.available() > 0) {
        incomingString = Serial.readString();
        strcpy(my_data, incomingString.c_str());
        incomingString = "";
    }

    unsigned long tiempoActual = millis();
    ws.cleanupClients();
    String myrss ;
    if (myRssi == 0){
        myrss = " -- ";
    } else{
        myrss = String(myRssi);
    }

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);

    display.drawString(0, 0, "RSSI: ");
    display.drawString(80, 25, myrss);

    display.display();

    if (tiempoActual - tiempoAnterior >= 5000) {

        myRssi = 0;
        tiempoAnterior = tiempoActual;
    }

    myRssi = loopLora(my_data);
    String rxpacket = sendPacket();
    ws.textAll(rxpacket);
}

