#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <time.h>
#include <secrets.h>

#define LED_BUILTIN 2 
#define SET_BUTTON_PIN 18 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define FACE_LENGTH 2

// FACE
int CURRENT_FACE = 0;

// NTP
const char* ntpServer = "time.google.com";
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
  pinMode(SET_BUTTON_PIN, INPUT_PULLUP);

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
  int state = digitalRead(SET_BUTTON_PIN);
  if (state == LOW) {
    CURRENT_FACE = (CURRENT_FACE + 1) % FACE_LENGTH;
    delay(300);
  }


  if (getLocalTime(&timeInfo, 0)) {
    display.clearDisplay();
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);

    display.setTextSize(2);// Size 2 -> 12x16px
    // 12 * 8 => Char Width * Number of Chars
    display.setCursor((SCREEN_WIDTH - 12 * 8) / 2, 8);
    display.printf("%02d:%02d:%02d\n",
      timeInfo.tm_hour,
      timeInfo.tm_min,
      timeInfo.tm_sec
    );

    display.setTextSize(1); // Size 1 -> 6x8px
    // 6 * 10 => Char Width * Number of Chars
    display.setCursor((SCREEN_WIDTH - 6 * 10) / 2, (SCREEN_HEIGHT - 8 * 2));
    display.printf("%02d/%02d/%04d\n",
      timeInfo.tm_mday,
      timeInfo.tm_mon + 1,
      timeInfo.tm_year + 1900
    );
  }
  else {
    const char* message = "...";
    display.clearDisplay();
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.setCursor((SCREEN_WIDTH - strlen(message) * 6) / 2, (SCREEN_HEIGHT - 8) / 2);
    display.printf("%s\n", message);
  }
  display.display();
}