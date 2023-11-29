//
// Created by Dany on 11/28/2023.
//
#include "LoRaWan_APP.h"

#define RF_FREQUENCY                                923000000 // Hz
#define TX_OUTPUT_POWER                             20        // dBm
#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
#define LORA_SPREADING_FACTOR                       12         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define BUFFER_SIZE                                 300 // Define the payload size here

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
char rxpacket2[BUFFER_SIZE];

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


void setLora(){

    Rssi = 0;

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxDone = OnRxDone;

    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, false, 0, LORA_IQ_INVERSION_ON, 1000);

    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                      LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, false, 0, LORA_IQ_INVERSION_ON, true);
    state = STATE_TX;
}


void OnTxDone(void) {
    state = STATE_RX;
}

void OnTxTimeout(void) {
    Radio.Sleep();
    state = STATE_TX;
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
    Rssi = rssi;
    rxSize = size;
    memcpy(rxpacket, payload, size);
    rxpacket[size] = '\0';
    Radio.Sleep();

    Serial.printf("\r\n%s\r\n", rxpacket);
    if (rxSize != 0) {
        strcpy(rxpacket2, rxpacket);
    }
    state = STATE_TX;
}

int16_t loopLora(char my_data[255]){
    switch (state) {
        case STATE_TX:
            strcpy(txpacket, my_data);
            Radio.Send((uint8_t *) txpacket, strlen(txpacket));
            strcpy(my_data, "");
            state = LOWPOWER;
            break;
        case STATE_RX:
            Radio.Rx(0);
            state = LOWPOWER;
            break;
        case LOWPOWER:
            Radio.IrqProcess();
            break;
        default:
            break;
    }

    return Rssi;
}

String sendPacket(){
    String packet = String(rxpacket2);

    return packet;
}