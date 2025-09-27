//TX
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

extern "C" {
#include "esp_ieee802154.h"
#include "esp_err.h"
}

#define CHANNEL    15
#define PAN_ID     0x1234
#define SHORT_ADDR 0x0001
#define REMOTE_SHORT_ADDR 0xFFFF // Broadcast
#define LED_PIN    8
#define TX_PWR     10
//#define CCA_mode ESP_IEEE802154_CCA_MODE_ED_CS  // ED + CS
//#define CCA_mode ESP_IEEE802154_CCA_MODE_ED // Energy Detection
//#define CCA_mode ESP_IEEE802154_CCA_MODE_CARRIER // Carrier Sense)
//#define CCA_time    2 // Symbol time 16us, min CCA assessment 18us --> 2 symbols

uint8_t led_tx = 0;
uint8_t led_err = 0;
uint32_t last_fade = 0;
uint32_t last_tx = 0;
//int8_t threshold_dbm = -67; // Channel free assessment threshold for CCA


Adafruit_NeoPixel strip(1, LED_PIN, NEO_GRB + NEO_KHZ800);
static uint8_t seq = 0;

// Minimal 802.15.4 data frame
int build_frame(uint8_t *buf, size_t maxlen) {
    size_t idx = 1;
    uint16_t fcf = 0x8841; // Data frame, no ACK, short address, no PAN ID compression
    buf[idx++] = fcf & 0xFF;
    buf[idx++] = fcf >> 8;
    buf[idx++] = seq++; // Sequence number

    // Destination PAN + Short address
    buf[idx++] = PAN_ID & 0xFF;
    buf[idx++] = PAN_ID >> 8;
    buf[idx++] = REMOTE_SHORT_ADDR & 0xFF;
    buf[idx++] = REMOTE_SHORT_ADDR  >> 8;

    // Source PAN + Short address
    buf[idx++] = PAN_ID & 0xFF;
    buf[idx++] = PAN_ID >> 8;
    buf[idx++] = SHORT_ADDR & 0xFF;
    buf[idx++] = SHORT_ADDR >> 8;

    // Payload
    const char *msg = "Hello802154";
    memcpy(&buf[idx], msg, strlen(msg));
    idx += strlen(msg);

    buf[0] = idx - 1; // PHY Länge
    return idx;
}

void setup() {
    Serial.begin(115200);
    while(!Serial);

    strip.begin();
    strip.show();

    esp_ieee802154_enable();
    esp_ieee802154_set_channel(CHANNEL);

    Serial.printf("TX Ready - Channel=%d, PAN=0x%04X, Short=0x%04X\n",
                  CHANNEL, PAN_ID, SHORT_ADDR);

    esp_err_t err = esp_ieee802154_set_txpower(TX_PWR);
    if (err == ESP_OK) {
        Serial.printf("TX power set to %d dBm\n", TX_PWR);
    } else {
        Serial.printf("Error setting TX power: 0x%X\n", err);
    }
//    esp_ieee802154_set_cca_mode(CCA_mode);
//    esp_ieee802154_set_cca_threshold(threshold_dbm);
}

void loop() {

    //TX Frame
    if (millis() - last_tx > 250) {  // cycle time 250 ms
        last_tx = millis();
        uint8_t frame[127];
        int len = build_frame(frame, sizeof(frame));
        esp_err_t err = esp_ieee802154_transmit(frame, false); // CCA=false

        if (err == ESP_OK) {
            Serial.printf("TX OK, len=%d, seq=%d\n", len, seq-1);
            led_tx = 10;
        } else {
            Serial.printf("TX failed, err=0x%X\n", err);
            led_err = 10;
        }
    }

    // LED fade
    if (millis() - last_fade > 20) {  // alle 20 ms
        last_fade = millis();
        strip.setPixelColor(0, led_tx, led_err, 0);
        strip.show();
        if (led_tx >= 5) led_tx -= 5; else led_tx = 0;
    }
}
