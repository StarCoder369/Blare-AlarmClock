#include <WiFi.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_SCLK 0
#define TFT_MOSI 1
#define TFT_RST 2
#define TFT_DC 3
#define TFT_CS 4
#define TFT_BL 5

// I'm not doing the mistake of writing my wifi password and commiting it to Github so everyone can see it.
// Definitely not speaking from experience here...
const char* ssid = "MY_WIFI_MINE_THIS_IS_MY_WIFI_NOT_YOURS_STOP_LOOKING_HERE";
const char* password = "MY_PASSWORD_WHY_ARE_YOU_LOOKING_HERE_STOP_TRYING_TO_GET_MY_PASSWORD";
const char* timezone = "PST8PDT,M3.2.0,M11.1.0"; // PST/PDT Because of daylight saving. Not sure how this calculates it, but I'll figure it out. 

class MyST7789 : public Adafruit_ST7789 {
public:
  MyST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst) : Adafruit_ST7789(cs, dc, mosi, sclk, rst) {}

  void setOffsets(uint8_t col, uint8_t row) {
    _colstart = _colstart2 = col;
    _rowstart = _rowstart2 = row;
  }
};

MyST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// I think I covered every single possibility of the wifi. So there's a few
// 1. Connecting to Wifi
// 2. The Wifi is not connected
// 3. When the time can't sync even with Wifi connected
// I don't think I'm missing anything. I think there might be things like a time mismatch or something, but idk. I'll have to check.

void showMessage(const char* mesage) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(0, 20);
  tft.print(message);
}

void setupDisplay() {
  tft.init(76, 284);
  tft.setOffsets(82, 18);
  tft.invertDisplay(false);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  // I don't know how many characters it supports.
  showMessage("Please be patient. The clock is trying to connect to the WIFI. ");
}

void setup() {
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);

  setupDisplay();

  WiFi.begin(ssid, password);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(250);
  }

  if (WiFi.status() != WL_CONNECTED) {
    showMessage("WIFI HAS FAILED! GET SOMETHING BETTER please. ");
    return;
  }

  configTzTime(timezone, "pool.ntp.org", "time.nist.gov");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    showMessage("WiFi has been lost. Please try to find it...");
    delay(1000);
    return;
  }

  struct tm timeInfo;

  if (getLocalTime(&timeInfo, 1000)) {
    char timeString[9];
    // Not sure if MS is actually a thing in this, I'll have to check. 
    strftime(timeString, sizeof(timeString), "%H:%M:%S:%MS", &timeInfo);

    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(3);
    tft.setCursor(0, 20);
    //I'm not sure how this will acually show up, so for now I'll make do with this. 
    tft.print("Time: " + timeString);
  } else {
    showMessage("Time cannot sync. It is a failure!");
  }

  delay(200);
  // Delay because otherwise it would be too much.
}