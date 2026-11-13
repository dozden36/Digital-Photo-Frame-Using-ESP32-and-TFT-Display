#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <LittleFS.h>
#include <TJpg_Decoder.h>
#include <esp_sleep.h>
#include <driver/rtc_io.h>

// --- WiFi Configuration ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);
TFT_eSPI tft = TFT_eSPI();


unsigned long sleepTimerDuration = 0; 
unsigned long lastActionTime = 0;
bool isTimerActive = false;
File fsUploadFile;

// --- Wake-Up Pin Configuration ---
// GPIO 4 is labeled as D2 on Seeed XIAO ESP32-C3
#define WAKEUP_PIN GPIO_NUM_4 


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Photo Keychain</title>
  <style>
    body { background-color: #191c20; color: #e1e3e1; font-family: 'Segoe UI', Roboto, sans-serif; text-align: center; padding: 20px; }
    .container { max-width: 400px; margin: auto; background: #282a2f; padding: 30px; border-radius: 16px; box-shadow: 0 4px 15px rgba(0,0,0,0.5); }
    h2 { color: #a8c7fa; }
    select, input[type="file"] { width: 100%; padding: 12px; margin: 15px 0; border-radius: 8px; border: 1px solid #44474e; background: #191c20; color: #fff; box-sizing: border-box; }
    .btn { background: #a8c7fa; color: #000; padding: 14px 20px; border: none; border-radius: 8px; cursor: pointer; font-weight: bold; width: 100%; font-size: 16px; transition: 0.3s; }
    .btn:hover { background: #82b0f4; }
  </style>
</head>
<body>
  <div class="container">
    <h2>Photo Keychain</h2>
    <form action="/upload" method="POST" enctype="multipart/form-data">
      <label>Select Image (JPEG only):</label>
      <input type="file" name="image" accept="image/jpeg" required>
      <label>Sleep Timer:</label>
      <select name="timer">
        <option value="10">10 Minutes</option>
        <option value="20">20 Minutes</option>
        <option value="30">30 Minutes</option>
        <option value="40">40 Minutes</option>
        <option value="50">50 Minutes</option>
        <option value="60">1 Hour</option>
      </select>
      <button type="submit" class="btn">Upload & Display</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

// --- TJpg_Decoder Callback Function ---
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

// --- Web Server Handlers ---
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleUpload() {
  HTTPUpload& upload = server.upload();
  
  if (server.hasArg("timer")) {
    int minutes = server.arg("timer").toInt();
    sleepTimerDuration = minutes * 60 * 1000; 
    lastActionTime = millis();
    isTimerActive = true;
    Serial.printf("Sleep timer set to: %d minutes\n", minutes);
  }

  if (upload.status == UPLOAD_FILE_START) {
    Serial.println("Upload Started...");
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.println("Uploading image...");
    
    fsUploadFile = LittleFS.open("/photo.jpg", "w");
  } 
  else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } 
  else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
      Serial.println("Upload Finished! Drawing image...");
      server.send(200, "text/html", "<h3 style='color:green; text-align:center;'>Upload successful! Display updated.</h3>");
      
      tft.fillScreen(TFT_BLACK);
      TJpgDec.drawFsJpg(0, 0, "/photo.jpg", LittleFS);
    }
  }
}

// --- Deep Sleep Setup ---
void enterDeepSleep() {
  Serial.println("Entering deep sleep...");
  tft.fillScreen(TFT_BLACK);
  
  rtc_gpio_pullup_en(WAKEUP_PIN);
  rtc_gpio_pulldown_dis(WAKEUP_PIN);


  esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 0); 
  
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);


  pinMode(WAKEUP_PIN, INPUT_PULLUP);

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);

  TJpgDec.setJpgScale(1); 
  TJpgDec.setSwapBytes(true); 
  TJpgDec.setCallback(tft_output);

  
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Woke up from button press! Displaying image...");
    if (LittleFS.exists("/photo.jpg")) {
      TJpgDec.drawFsJpg(0, 0, "/photo.jpg", LittleFS);
    } else {
      tft.setCursor(10, 10);
      tft.println("No image found!");
    }
    
    // Auto-sleep again after 1 minute if no new uploads happen
    sleepTimerDuration = 1 * 60 * 1000; 
    lastActionTime = millis();
    isTimerActive = true;
  } 
  else {
    // Normal Boot - Start WiFi Setup
    tft.setCursor(10, 10);
    tft.println("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.println("System Ready!");
    tft.println("IP Address:");
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println(WiFi.localIP());

    server.on("/", HTTP_GET, handleRoot);
    server.on("/upload", HTTP_POST, []() { server.send(200); }, handleUpload);
    server.begin();
  }
}

void loop() {
  server.handleClient();

  // Handle sleep timer
  if (isTimerActive) {
    if (millis() - lastActionTime >= sleepTimerDuration) {
      enterDeepSleep();
    }
  }
}
