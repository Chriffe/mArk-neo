# Daily Scroll — CrowPanel Advanced 10.1" (ESP32-P4)

A daily task viewer that pulls events from Google Calendar and ICS feeds,
displayed on an Elecrow CrowPanel Advanced 10.1" screen with rotary encoder
navigation and WS2812 LED progress bar. Supports up to 6 family members, each
with their own calendars and completion streaks.

---

## Quick Overview

- Tap tasks on screen to mark them done
- LEDs fill up as you complete tasks
- Switch users via the left sidebar
- Add/edit calendar sources from any phone browser via the built-in web UI
- Refreshes every 5 minutes automatically

---

## New User Setup — Step by Step

### Step 1: Install ESP-IDF

The project requires **ESP-IDF v5.4.2**.

Follow the official installation guide for your OS:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/get-started/

After installation, verify it works:

```bash
idf.py --version
# Should print: ESP-IDF v5.4.2
```

Make sure `IDF_PATH` is set and the `idf.py` command is available in your
terminal before continuing.

---

### Step 2: Get the Elecrow BSP (Board Support Package)

The display and touch drivers are not included in this repo — they come from
Elecrow's official example code.

```bash
# Clone Elecrow's example repo (you only need to do this once)
git clone https://github.com/Elecrow-RD/CrowPanel-Advanced-10.1inch-ESP32-P4-HMI-AI-Display-1024x600-IPS-Touch-Screen.git elecrow-examples

# Copy the peripheral folder into this project
cp -r elecrow-examples/example/V1.0/idf-code/Lesson09-LVGL_Lighting_Control/peripheral esp32-project/
```

The `peripheral/` folder contains:
- `bsp_display/` — MIPI-DSI display driver and LVGL port
- `bsp_illuminate/` — Backlight control
- `bsp_i2c/` — I2C bus (used by touch controller)
- `bsp_extra/` — GT911 touch panel driver

If the screen shows white/blank after flashing, this folder is either missing
or from the wrong Elecrow lesson. Lesson 09 is the correct one.

---

### Step 3: Get a Google Calendar API Key

If you want to fetch **public** Google Calendars using their Calendar ID:

1. Go to https://console.cloud.google.com
2. Create a new project (or use an existing one)
3. Go to **APIs & Services → Library**
4. Search for **Google Calendar API** and click **Enable**
5. Go to **APIs & Services → Credentials**
6. Click **Create Credentials → API key**
7. Copy the key — you will paste it in the next step

> **Note:** The API key only works for calendars that are set to "public" in
> Google Calendar settings. For private calendars, use the ICS/iCal URL
> instead (see the web UI help text for instructions).

---

### Step 4: Configure WiFi and API Key

Copy the example secrets file and fill in your credentials:

```bash
cp main/secrets.h.example main/secrets.h
```

Then open `main/secrets.h` and edit the three lines:

```c
#define WIFI_SSID      "your_wifi_network_name"
#define WIFI_PASSWORD  "your_wifi_password"
#define GCAL_API_KEY   "your_google_api_key_from_step_3"
```

`secrets.h` is listed in `.gitignore` and will never be committed.

> The NTP server is set to `pool.ntp.org` by default — change it in
> `main/calendar_fetch.c` if you need a different one.

---

### Step 5: Build and Flash

Connect the CrowPanel to your computer via USB, then:

```bash
cd esp32-project

# Set the target chip (only needed once per project)
idf.py set-target esp32p4

# Build the firmware
idf.py build

# Flash to the board (replace PORT with your actual port)
# macOS: typically /dev/cu.wchusbserial10 or /dev/cu.usbserial-XXXX
# Linux: typically /dev/ttyUSB0 or /dev/ttyACM0
idf.py -p /dev/cu.wchusbserial10 flash

# Optional: open serial monitor to see logs
idf.py -p /dev/cu.wchusbserial10 monitor
```

To find your port:
```bash
# macOS
ls /dev/cu.*

# Linux
ls /dev/ttyUSB* /dev/ttyACM*
```

The first build takes several minutes — subsequent builds are much faster.

> **Shortcut:** `flash.sh` in the project root builds, flashes, and opens the
> monitor in one command. Edit it if your port differs from `/dev/cu.wchusbserial10`.

---

### Step 6: First Boot

After flashing, the board will reboot and:

1. Connect to your WiFi network
2. Sync time via NTP
3. Show a "Connecting..." screen while fetching calendar events
4. Display today's tasks on the main screen

The board's IP address is printed in the serial monitor on boot. The mDNS
hostname `taskviewer.local` can also be used from devices on the same network.

---

### Step 7: Add Calendars via Web UI

Once the board is running and connected to WiFi, open a browser on any phone
or computer on the same network and go to:

```
http://<board-ip-address>/
```

or

```
http://taskviewer.local/
```

From the web UI you can:

- Add **Google Calendar** sources (requires the Calendar ID and API key from Step 3)
- Add **ICS / iCal** sources (works with Google, Apple, Outlook, and any
  standard ICS feed — best for private calendars)
- Edit calendar names and toggle them on/off
- Configure calendars separately for each user

**How to get an ICS URL from Google Calendar:**
1. Open Google Calendar on desktop
2. Click the three dots next to the calendar → **Settings and sharing**
3. Scroll down to **Secret address in iCal format** (for private) or
   **Public address in iCal format** (for public)
4. Copy the URL and paste it into the web UI

**How to get an ICS URL from Apple Calendar (iCloud):**
1. On iPhone: Settings → [your name] → iCloud → turn on Calendars
2. On Mac: Calendar → right-click calendar → Share Calendar → copy the URL

---

### Step 8: Add More Users (optional)

The app supports up to 6 users (family members), each with their own calendars
and completion streaks.

To add a user:
1. Tap **Settings** in the left sidebar on the device screen
2. Tap **Add user**
3. Enter a name
4. Then go to the web UI to add that user's calendar sources

To switch users: tap the user icons in the left sidebar.

---

## Controls

| Control | Action |
|---|---|
| Touch task row | Toggle task done/undone |
| Rotate encoder | Scroll through task list |
| Press encoder button | Toggle current task done/undone |
| Touch **Refresh** (sidebar) | Fetch latest calendar events |
| Touch **Settings** (sidebar) | Open user management |
| Touch user icons (sidebar) | Switch active user |
| Power button short press | Toggle display + LEDs on/off |
| Power button long press (3 s) | Deep sleep (press again to wake) |

---

## Hardware

See `COMPONENTS.md` for a full bill of materials with quantities and notes.

| Component | Details |
|---|---|
| Display | CrowPanel Advanced 10.1" ESP32-P4 (1024×600, MIPI-DSI IPS) |
| MCU | ESP32-P4 (dual-core RISC-V 400 MHz) |
| WiFi | ESP32-C6 co-processor (WiFi 6 + BLE 5.3, built into CrowPanel) |
| Touch | GT911 capacitive touch (built into CrowPanel) |
| Input | Rotary encoder with push button |
| LEDs | WS2812 LED strip (32 LEDs) |
| Power | Momentary push button for soft power control |

---

## Pin Assignment

| Function | GPIO | Notes |
|---|---|---|
| WS2812 DATA | IO2 | LED strip data, 330Ω series resistor recommended |
| Power button | IO3 | Momentary, active low with internal pull-up |
| Encoder push | IO5 | Active low with internal pull-up |
| Encoder A (CLK) | IO27 | Rotary encoder |
| Encoder B (DT) | IO28 | Rotary encoder |
| SPK-R | IO14 | I2S audio output (right channel) |
| SPK-L | IO16 | I2S audio output (left channel) |

**WS2812 wiring:** DIN → IO2, VCC → 5V, GND → GND

**Power button wiring:** one leg → IO3, other leg → GND

**Encoder wiring:** CLK → IO27, DT → IO28, SW → IO5, VCC → 3.3V, GND → GND

---

## Project Structure

```
esp32-project/
├── CMakeLists.txt              # ESP-IDF project root
├── sdkconfig.defaults          # Default SDK configuration
├── partitions.csv              # Flash partition table
├── flash.sh                    # Shortcut: build + flash + monitor
├── main/
│   ├── idf_component.yml       # Component dependencies (LVGL 9.2, mDNS, etc.)
│   ├── main.c                  # Entry point, init sequence, refresh task
│   ├── calendar_fetch.c/h      # WiFi, NTP, Google Calendar API, ICS fetcher
│   ├── ui_taskviewer.c/h       # LVGL UI — main screen, sidebar, settings
│   ├── web_config.c/h          # HTTP config server (calendar management)
│   ├── user_store.c/h          # NVS-backed multi-user profiles
│   ├── streak_store.c/h        # NVS streak persistence per user
│   ├── ws2812_led.c/h          # WS2812 LED strip (RMT driver)
│   ├── led_strip_encoder.c/h   # RMT encoder for WS2812 protocol
│   ├── power_button.c/h        # Soft power (short=toggle, long=deep sleep)
│   ├── rotary_encoder.c/h      # Encoder scroll + button navigation
│   ├── sound_driver.c/h        # I2S audio feedback
│   └── lv_font_*.c             # Custom fonts (Swedish + UI glyphs)
└── peripheral/                 # Elecrow BSP — copy from their repo (Step 2)
    ├── bsp_display/            # MIPI-DSI display driver + LVGL port
    ├── bsp_illuminate/         # Backlight control
    ├── bsp_i2c/                # I2C bus driver
    └── bsp_extra/              # GT911 touch driver
```

---

## Troubleshooting

**White or blank screen after flashing**
→ The `peripheral/` folder is missing or copied from the wrong Elecrow lesson.
Use Lesson 09 from the CrowPanel Advanced 10.1" repo.

**Build error: `idf.py` not found or wrong version**
→ Re-run the ESP-IDF installer and make sure to run the `export.sh` / `export.bat`
script that activates the environment in your current terminal session.

**No WiFi connection**
→ Double-check `WIFI_SSID` and `WIFI_PASSWORD` in `main/calendar_fetch.c`.
The ESP32-P4 uses an ESP32-C6 co-processor for WiFi — both chips boot together
and this is handled automatically, but make sure you flashed the correct port.

**Calendar not showing events**
→ Check that your Google Calendar is set to **public**, or use an ICS URL
for private calendars. Verify the API key has Google Calendar API enabled in
Google Cloud Console.

**Touch not working**
→ Run the Elecrow Lesson 05 (Touchscreen) example first to confirm the touch
controller is functional on your board.

**Web UI not reachable**
→ Make sure your phone/computer is on the same WiFi network as the board.
Try the IP address shown in the serial monitor instead of `taskviewer.local`.

**LEDs stay on after display turned off**
→ This should not happen in the current firmware. If it does, a hard reboot
(unplug and replug) will reset the state.
