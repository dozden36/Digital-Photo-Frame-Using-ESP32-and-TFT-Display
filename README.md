# Digital-Photo-Frame-Using-ESP32-and-TFT-Display

A minimalist, low-power digital photo keychain powered by an ESP32. It creates a local Wi-Fi web server, allowing you to upload and display JPEG images directly from your phone, and enters Deep Sleep mode after a selected timer to preserve battery.

## 📚 Required Libraries

Install these libraries via the Arduino IDE Library Manager:
* **TFT_eSPI** (by Bodmer)
* **TJpg_Decoder** (by Bodmer)

---

## 🔌 Pin Connections & Setup

### 1. Display (1.3" 240x240 IPS - ST7789, No CS Pin)
Connect your display to your microcontroller:
* **VCC & BLK:** 3.3V
* **GND:** GND
* **MOSI & SCLK:** Defined in `User_Setup.h` (see below)
* **DC & RST:** Defined in `User_Setup.h` (see below)

### 2. Wake-Up Button
* Connect one side of the push button to **GND**.
* Connect the other side to **GPIO 4** (Labeled as **D2** on Seeed Studio XIAO ESP32-C3).

---

## ⚙️ Where to Edit (Configuration)

### 1. Wi-Fi Credentials
In `main.ino`, update your Wi-Fi SSID and password:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
