#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "wifi_config.h"
#include "LoRaWan_APP.h"
#include "SPIFFS.h"

#define RF_FREQUENCY                                923000000 // Hz
#define TX_OUTPUT_POWER                             20        // dBm
#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
#define LORA_SPREADING_FACTOR                       12         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define BUFFER_SIZE                                 30 // Define the payload size here

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;

void OnTxDone(void);

void OnTxTimeout(void);

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);


typedef enum {
    LOWPOWER,
    STATE_RX,
    STATE_TX
} States_t;

States_t state;
int16_t Rssi, rxSize;
char my_data[255];

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
                Serial.println(len);
                strncpy(my_data, reinterpret_cast<const char *>(midata), len + 1);
                my_data[len] = '\0';
                Serial.println(String((const char *) my_data));
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

    Mcu.begin();
    Rssi = 0;

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxDone = OnRxDone;

    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, false, 0, LORA_IQ_INVERSION_ON, 3000);

    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                      LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, false, 0, LORA_IQ_INVERSION_ON, true);
    state = STATE_TX;
}


void loop() {
    ws.cleanupClients();

    switch (state) {
        case STATE_TX:
            delay(1000);
            sprintf(txpacket, "%s", my_data);
           Serial.printf("\r\nsending packet \"%s\" , length %d\r\n", txpacket, strlen(txpacket));
            Radio.Send((uint8_t *) txpacket, strlen(txpacket));
            strcpy(my_data, "");
            state = LOWPOWER;
            break;
        case STATE_RX:
            Serial.println("into RX mode");
            Radio.Rx(0);
            state = LOWPOWER;
            break;
        case LOWPOWER:
            Radio.IrqProcess();
            break;
        default:
            break;
    }
    delay(10);
}

void OnTxDone(void) {
    //Serial.print("TX done......");
    state = STATE_RX;
}

void OnTxTimeout(void) {
    Radio.Sleep();
    //Serial.print("TX Timeout......");
    state = STATE_TX;
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
    Rssi = rssi;
    rxSize = size;
    memcpy(rxpacket, payload, size);
    rxpacket[size] = '\0';
    Radio.Sleep();

    Serial.printf("\r\nreceived packet \"%s\" with Rssi %d , length %d\r\n", rxpacket, Rssi, rxSize);
    //Serial.println("wait to send next packet");
    if (rxSize != 0) {
        ws.textAll(String(rxpacket));
    }
    state = STATE_TX;
}