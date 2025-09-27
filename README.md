## Minimal 802.15.4 TX and RX on ESP32C6 for Arduino



This sketch works with the usual ESP32C6 boards looking like this:



![](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/_images/esp32-c6-devkitc-1-v1.2-annotated-photo.png)



In Arduino IDE choose ESP32C6 Dev Module, add the NeoPixel library, keep CDC on boot disabled, then connect a USB C cable to the UART Port.

By using this port powering the board from a power bank or a mobile charger will not stop the serial console from the point of view of the ESP.



### TX Sketch

Transmits a dummy broadcast frame every 250 ms.

For every tx package the LED is blinking in red and some status info is printed to the serial console.

No regulatory requirements like LBT or CCA implemented.



### RX Sketch

Receives all frames properly addressed, this incudes broadcast frames of the correct PAN ID.

For every receiced package the LED is blinking in magenta and some status info is printed to the serial console.

The sketch contains provisions for energy detection of the used channel, it is deactivated, as it might lead to crashes if the detection coincides with a received package, no interlocking is currently implemented.



## How this happened

This is a result of an intense trial and error vibe-coding session, as most sources and examples dont work with the current version of ESP IDF as integrated into Arduino, a lot of manual tweaking and debugging was required. 

The version tested is ESP-IDF version: v5.5-1-gb66b5448e0



## Licensing

©2025, freely available under the MIT License
