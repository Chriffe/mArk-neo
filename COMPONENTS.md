# Bill of Materials — Daily Scroll

All components needed to build the Daily Scroll task viewer.

---

## Required

| # | Component | Notes |
|---|---|---|
| 1 | **Elecrow CrowPanel Advanced 10.1"** | The main board. Includes ESP32-P4 MCU, ESP32-C6 WiFi co-processor, MIPI-DSI 1024×600 IPS display, GT911 capacitive touch, and a built-in speaker amplifier. Search: "CrowPanel Advanced 10.1 ESP32-P4" |
| 1 | **USB-C cable** | For programming and power during development. Must support data (not charge-only). |
| 1 | **5 V power supply (2 A or more)** | For permanent installation. The CrowPanel can be powered via USB-C or its screw terminal/barrel connector depending on board revision. |

---

## Optional Add-ons

| # | Component | GPIO | Notes |
|---|---|---|---|
| 1 | **Rotary encoder with push button** | CLK→IO27, DT→IO28, SW→IO5 | Any standard EC11-style encoder. Used to scroll the task list. |
| 4 | **WS2812B LED** (or compatible NeoPixel) | IO2 | 5 V addressable RGB LEDs. Used as a progress bar — 4 LEDs fill up as tasks are completed. Wire them in series with a 330Ω resistor on the data line. |
| 1 | **Momentary push button** (normally open) | IO3 | Soft power button. Short press toggles the display; long press (3 s) puts the board into deep sleep. Any standard 6 mm or 12 mm panel-mount button works. |

---

## Wiring Notes

### WS2812 LEDs
```
CrowPanel IO2 → 330Ω resistor → LED 1 DIN
LED 1 DOUT → LED 2 DIN → LED 3 DIN → LED 4 DIN
All LEDs share 5V and GND
```
A 100µF capacitor across the 5 V rail near the LEDs helps prevent power glitches.

### Rotary Encoder
```
CLK → IO27
DT  → IO28
SW  → IO5
+   → 3.3V
GND → GND
```
The firmware uses internal pull-ups on IO5, IO27, and IO28 — no external
resistors needed for the encoder or button.

### Power Button
```
One leg → IO3
Other leg → GND
```
The firmware uses an internal pull-up on IO3.

---

## No-solder Minimum

If you just want to get started without any soldering, the CrowPanel alone
is enough. The rotary encoder, LEDs, and power button are all optional — the
display is fully touchscreen-operated and the web UI handles all configuration.

---

## Tools Required

| Tool | Purpose |
|---|---|
| Computer (macOS, Linux, or Windows) | Building and flashing firmware |
| USB-C cable (data-capable) | Connecting board to computer |
| Soldering iron (optional) | If adding rotary encoder, LEDs, or power button |
| Breadboard or prototype PCB (optional) | For the external components above |
