#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <time.h>
#include <secrets.h>
#include <map>

#define LED_BUILTIN 2 
#define SET_BUTTON_PIN 18 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

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

// FACES
enum FACE_TYPE {
  FACE_DEEFAULT,
  FACE_CLOCK,
  FACE_BINARY_CLOCK,
  FACE_SIN_WAVE,
  FACE_LISSAJOUS_CURVE,
  FACE_DVD,
  FACE_COUNT
};
FACE_TYPE CURRENT_FACE = FACE_DEEFAULT;
class Face {
public:
  virtual void  show() = 0;
  virtual void reset() = 0;
  virtual ~Face() {}
};
std::map<FACE_TYPE, Face*> FACES;

class DefaultFace : public Face {
public:
  void show() {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor((SCREEN_WIDTH - 18 * 3) / 2, (SCREEN_HEIGHT - 16) / 2);
    display.println("128");
    display.display();
  }
  void reset() {}
};
DefaultFace* DEFAULT_FACE = new DefaultFace();

class Clock : public Face {
public:
  void show() {
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
      display.setTextSize(1);
      display.setCursor((SCREEN_WIDTH - strlen(message) * 6) / 2, (SCREEN_HEIGHT - 8) / 2);
      display.printf("%s\n", message);
    }
    display.display();
  }

  void reset() {}
};

class BinaryClock : public Face {
public:
  void show() {
    if (getLocalTime(&timeInfo, 0)) {
      display.clearDisplay();
      display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);

      int hour =
        timeInfo.tm_hour;
      for (int i = 5; i >= 0; i--) {
        if ((hour >> i) & 1) {
          display.fillRect((5 - i) * 21 + 2, 2, 19, 19, SSD1306_WHITE);
        }
        else {
          display.drawRect((5 - i) * 21 + 2, 2, 19, 19, SSD1306_WHITE);
        }
      }


      int minute =
        timeInfo.tm_min;
      for (int i = 5; i >= 0; i--) {
        if ((minute >> i) & 1) {
          display.fillRect((5 - i) * 21 + 2, 22, 19, 19, SSD1306_WHITE);
        }
        else {
          display.drawRect((5 - i) * 21 + 2, 22, 19, 19, SSD1306_WHITE);
        }
      }


      int second =
        timeInfo.tm_sec;
      for (int i = 5; i >= 0; i--) {
        if ((second >> i) & 1) {
          display.fillRect((5 - i) * 21 + 2, 42, 19, 19, SSD1306_WHITE);
        }
        else {
          display.drawRect((5 - i) * 21 + 2, 42, 19, 19, SSD1306_WHITE);
        }
      }

    }
    else {
      const char* message = "...";
      display.clearDisplay();
      display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
      display.setTextSize(1);
      display.setCursor((SCREEN_WIDTH - strlen(message) * 6) / 2, (SCREEN_HEIGHT - 8) / 2);
      display.printf("%s\n", message);
    }
    display.display();
  }

  void reset() {}
};

class SinWave : public Face {
  int SIN_AMP = 24;
  int SIN_DIR = 1;
  float SIN_T = 0;
  float SIN_BALL_X = 0;

public:
  void show() {
    display.clearDisplay();

    for (int i = 0; i < 127; i++)
    {
      float angle = i * 0.1;
      int y = 32 + SIN_AMP * sin(angle + SIN_T);
      display.drawPixel(i, y, SSD1306_WHITE);
    }

    float angle = 
    SIN_BALL_X * 0.1;
    int y = 32 + SIN_AMP * sin(angle + SIN_T);
    display.drawCircle(SIN_BALL_X, y, 5, SSD1306_WHITE);

    display.display();

    SIN_T += 0.1;

    SIN_BALL_X += 1;
    if (SIN_BALL_X >= 128)
    {
      SIN_BALL_X = 0;
    }
  }

  void reset() {
    SIN_AMP = 24;
    SIN_DIR = 1;
    SIN_T = 0;
    SIN_BALL_X = 0;
  }

};
class DVD : public Face {
  float x = 0;
  float y = 0;

  float boxHeight = 20;
  float boxWidth = 30;

  float xDir = 1;
  float yDir = 1;



public:
  void show() {
    display.clearDisplay();

    display.drawRect(x, y, boxWidth, boxHeight, SSD1306_WHITE);

    const char* text = "DVD";
    int16_t tx, ty;
    uint16_t tw, th;
    display.getTextBounds(text, 0, 0, &tx, &ty, &tw, &th);

    display.setCursor(x + (boxWidth - tw) / 2, y + (boxHeight - th) / 2);
    display.setTextColor(SSD1306_WHITE);
    display.print(text);

    display.display();

    if (x < 0 || x + boxWidth > 128) {
      xDir *= -1;
    }
    if (y < 0 || y + boxHeight > 64) {
      yDir *= -1;
    }

    x += 1 * xDir;
    y += 1 * yDir;
  }

  void reset() {
  }
};

class LissajousCurve : public Face {
  float a = 1.0;
  float b = 2.0;
  float delta = PI / 2;
  int dir = 1;

public:
  void show() {
    display.clearDisplay();
    for (int i = 0; i <= 4 * 360; i++) {
      float theta = 0.25 * i * PI / 180;
      int x = (SCREEN_WIDTH / 2) + ((SCREEN_WIDTH / 2) * sin(a * theta + delta));
      int y = (SCREEN_HEIGHT / 2) + ((SCREEN_HEIGHT / 2) * sin(b * theta));
      display.drawPixel(x, y, SSD1306_WHITE);
    }

    display.display();

    delta += 0.01;
    a += 0.01 * dir;
    if (a >= 4.0 || a <= 1.0) {
      dir *= -1;
    }
  }

  void reset() {
    a = 1;
    b = 2;
    delta = PI / 2;
  }
};

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

  display.setTextColor(SSD1306_WHITE);

  FACES[FACE_TYPE::FACE_DEEFAULT] = new DefaultFace();
  FACES[FACE_TYPE::FACE_CLOCK] = new Clock();
  FACES[FACE_TYPE::FACE_BINARY_CLOCK] = new BinaryClock();
  FACES[FACE_TYPE::FACE_SIN_WAVE] = new SinWave();
  FACES[FACE_TYPE::FACE_LISSAJOUS_CURVE] = new LissajousCurve();
  FACES[FACE_TYPE::FACE_DVD] = new DVD();
}

void loop()
{
  int state = digitalRead(SET_BUTTON_PIN);
  if (state == LOW) {
    CURRENT_FACE = static_cast<FACE_TYPE>(
      (static_cast<int>(CURRENT_FACE) + 1) % FACES.size()
      );
    if (FACES[CURRENT_FACE]) {
      FACES.at(CURRENT_FACE)->reset();
    }
    delay(300);
  }
  if (FACES[CURRENT_FACE]) {
    FACES[CURRENT_FACE]->show();
  }
  else {
    DEFAULT_FACE->show();
  }

}
