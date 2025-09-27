//RX
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

extern "C" {
#include "esp_ieee802154.h"
#include "hal/ieee802154_ll.h"
#include "esp_err.h"
}

#define CHANNEL 15
#define PAN_ID  0x1234
#define SHORT_ADDR 0x4011
#define LED_PIN 8

uint8_t led_rx = 0;
uint8_t led_ed = 0;
uint32_t last_ed = 0;
uint32_t last_fade = 0;
bool ED_ready = true;

Adafruit_NeoPixel strip(1, LED_PIN, NEO_GRB + NEO_KHZ800);

// RX Callback
extern "C" void esp_ieee802154_receive_done(uint8_t *frame, esp_ieee802154_frame_info_t *info) {
    //Destination PAN in Frame
    //uint16_t dst_pan = frame[4] | (frame[5]<<8);
    //Serial.printf("dst_pan=0x%04X\n", dst_pan);

    int len = frame[0];
    Serial.printf("[RX] len=%d, rssi=%d\n", len, info->rssi);

    Serial.print("Raw: ");
    for (int i=0; i<len; i++) Serial.printf("%02X ", frame[i]);
    Serial.println();

    led_rx = 100;
    esp_ieee802154_receive_handle_done(frame);
}

// Energy Detection Callback
extern "C" void esp_ieee802154_energy_detect_done(int8_t power) {
    Serial.printf("[ED] Kanal %d: Energie = %d dBm\n", CHANNEL, power);
    led_ed = 100;
    ED_ready=true;
}

void setup() {
    Serial.begin(115200);
    while(!Serial);

    strip.begin();
    strip.show();

    esp_ieee802154_enable();
    esp_ieee802154_set_channel(CHANNEL);
    esp_ieee802154_set_panid(PAN_ID);
    esp_ieee802154_set_short_address(SHORT_ADDR);
    esp_ieee802154_set_promiscuous(false);
    esp_ieee802154_receive();
    esp_ieee802154_set_rx_when_idle(true);

    Serial.printf("RX Ready - Channel=%d, PAN=0x%04X\n", CHANNEL, PAN_ID);
}

void loop() {
    //Energy Detection
    if (false) { //((millis() - last_ed > 800) & ED_ready) {  // cycle time 0.8 sec
        Serial.print("Starting Energy Detection...");
        Serial.println(millis() - last_ed);
        esp_ieee802154_energy_detect(2);  // 2 Symbols ≈ 32 us
        last_ed = millis();
        ED_ready=false;
    }

    // LED fade
    if (millis() - last_fade > 20) {  // alle 20 ms
        last_fade = millis();

        uint32_t color = strip.ColorHSV(54000, 0xDF, led_rx);
        strip.setPixelColor(0, color);
        //strip.setPixelColor(0, 0, led_rx, 0);
        strip.show();
        if (led_rx >= 5) led_rx -= 5; else led_rx = 0;
    }
}
