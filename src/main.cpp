#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <time.h>
#include <secrets.h>

#define LED_BUILTIN 2  
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = GMT_OFFSET_SEC;
const int daylightOffset_sec = DAYLGHT_OFFSET_SEC;
struct tm timeInfo;

// OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// NETWORK
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
void initWiFi() {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.onEvent(
    [](WiFiEvent_t event) {
      switch (event)
      {
      case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("WiFi Connected!");
        break;
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.println(WiFi.localIP());
        digitalWrite(LED_BUILTIN, HIGH);
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        Serial.println("Time synced!");
        break;
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("WiFi Disconnected");
        break;
      }
    }
  );

  WiFi.begin(ssid, password);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  initWiFi();

  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("SSD1306 init failed");
    while (true)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.println("Hello World!");
  display.display();
}

void loop()
{
  if (getLocalTime(&timeInfo)) {
    Serial.printf("%02d:%02d:%02d  %02d/%02d/%04d\n",
      timeInfo.tm_hour,
      timeInfo.tm_min,
      timeInfo.tm_sec,
      timeInfo.tm_mday,
      timeInfo.tm_mon + 1,   // months start from 0!
      timeInfo.tm_year + 1900 // years from 1900!
    );
  }
  else {
    Serial.println("Time not available yet...");
  }
}