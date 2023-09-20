#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "web_page.h"
#include "wifi_config.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onEvent(AsyncWebSocket *myserver, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *midata,
             size_t len) {

    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                          client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            // WebSocket data received
            if (len > 0) {
                // Print the received message to the Serial Monitor
//                char *data = (char *) malloc(sizeof(char) * (len + 1));
//                strncpy(data, (const char *) midata, len);
//                Serial.println(data);
//                free(data);
                  Serial.println(String((const char *) midata));


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

    setWifi();

    ws.onEvent(onEvent);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", html);
    });

    // Start the server
    server.begin();
}

void loop() {
    ws.cleanupClients();

    if (Serial.available()) {
        String mydata = Serial.readStringUntil('\n');
        // TODO: Process the data
        ws.textAll(mydata);
    }
}