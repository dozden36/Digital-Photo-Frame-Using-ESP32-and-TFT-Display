#define USER_SETUP_INFO "User_Setup"

#define ST7789_DRIVER // 1.3 inch 240x240 IPS (No CS pin)
#define TFT_SDA_READ
#define TFT_RGB_ORDER_TFT_RGB
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// ==========================================
// OPTION 1: ESP32-S3 PINS
// ==========================================
#define TFT_MOSI 11 
#define TFT_SCLK 12 
#define TFT_DC   10
#define TFT_RST  9
#define TFT_BL   -1  // Connect Display BL/LED pin directly to 3.3V

// ==========================================
// OPTION 2: Seeed Studio XIAO ESP32-C3
// ==========================================
// #define TFT_MOSI 10 // XIAO pin D10
// #define TFT_SCLK 8  // XIAO pin D8
// #define TFT_DC   3  // XIAO pin D1 (GPIO 3)
// #define TFT_RST  5  // XIAO pin D3 (GPIO 5)
// #define TFT_BL   -1 // Connect Display BL/LED pin directly to 3.3V

// ==========================================
// DISPLAY TWEAKS (Only uncomment if screen looks weird)
// ==========================================
// If colors are inverted (looks like a negative photo), uncomment below:
// #define TFT_INVERSION_ON

// If the image is shifted off-center, uncomment below:
// #define CGRAM_OFFSET

// ==========================================
// FONTS & SPI SPEED
// ==========================================
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define SPI_FREQUENCY  27000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000
#define USE_HSPI_PORT
