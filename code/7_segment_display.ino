#include <Wire.h>
#include <ezTime.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <FastLED.h>
#include <FS.h>  // Please read the instructions on http://arduino.esp8266.com/Arduino/versions/2.3.0/doc/filesystem.html#uploading-files-to-file-system
#include <ArduinoOTA.h>
#include <OneWire.h>            // needed for one wire sensors
#include <DallasTemperature.h>  // needed for DS18x20 sensors (which is a one wire sensor)
#define TEMPERATURE_PRECISION 12
#define TEMP_SENSOR_PIN 0  // temperature sensors are on D3, which is pin 0
#define countof(a) (sizeof(a) / sizeof(a[0]))
#define NUM_LEDS 130  // Total of 86 LED's. Need 2*3*7 + 2 extra for seconds
#define DATA_PIN D6   // Change this if you are using another type of ESP board than a WeMos D1 Mini
#define MILLI_AMPS 2400
#define COUNTDOWN_OUTPUT D5
#define WIFIMODE 1        // 0 = Only Soft Access Point, 1 = Only connect to local WiFi network with UN/PW, 2 = Both
#include "Credentials.h"  // Create this file in the same directory as the .ino file and add your credentials (#define SID YOURSSID and on the second line #define PW YOURPASSWORD)
const char *ssid = SID;
const char *password = PW;

int oneWireCount = 0;
OneWire oneWire(TEMP_SENSOR_PIN);         // Set up a OneWire instance to communicate with OneWire devices
DallasTemperature tempSensors(&oneWire);  // Create an instance of the temperature sensor class
boolean TemperatureSensor = false;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdateServer;
CRGB LEDs[NUM_LEDS];

// Settings
Timezone myTZ;
boolean WifiConnected = false;
unsigned long millis_since_start;  // millis() is used to count the time. This value contains the start value (t = 0 on the stopwatch)
unsigned long prevSec = 0;
byte r_val = 255;
byte g_val = 0;
byte b_val = 0;
bool dotsOn = true;
byte brightness = 255;
float temperatureCorrection = -3.0;
byte temperatureSymbol = 12;  // 12=Celcius, 13=Fahrenheit check 'numbers'
byte clockMode = 0;           // Clock modes: 0=Clock, 1=Countdown, 2=Temperature, 3=Scoreboard
unsigned long countdownMilliSeconds;
unsigned long endCountDownMillis;
byte hourFormat = 24;  // Change this to 12 if you want default 12 hours format instead of 24
CRGB countdownColor = CRGB::Green;
byte scoreboardLeft = 0;
byte scoreboardRight = 0;
CRGB scoreboardColorLeft = CRGB::Green;
CRGB scoreboardColorRight = CRGB::Red;
CRGB alternateColor = CRGB::Black;
long numbers[] = {
  0b000111111111111111111,  // [0] 0
  0b000111000000000000111,  // [1] 1
  0b111111111000111111000,  // [2] 2
  0b111111111000000111111,  // [3] 3
  0b111111000111000000111,  // [4] 4
  0b111000111111000111111,  // [5] 5
  0b111000111111111111111,  // [6] 6
  0b000111111000000000111,  // [7] 7
  0b111111111111111111111,  // [8] 8
  0b111111111111000111111,  // [9] 9
  0b000000000000000000000,  // [10] off
  0b111111111111000000000,  // [11] degrees symbol
  0b000000111111111111000,  // [12] C(elsius)
  0b111000111111111000000,  // [13] F(ahrenheit)
};

void setup() {
  pinMode(COUNTDOWN_OUTPUT, OUTPUT);
  Serial.begin(115200);
  delay(200);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  delay(200);
  Serial.setDebugOutput(true);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(LEDs, NUM_LEDS);
  FastLED.setDither(false);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  //fill_solid(LEDs, NUM_LEDS, CRGB::Green);
  FillLEDsFromPaletteColors(0);
  FastLED.show();
  FastLED.show();

  byte count = 0;
  WiFi.begin(ssid, password);
  delay(200);
  while (WiFi.status() != WL_CONNECTED) {
    // Stop if cannot connect
    if (count >= 60) {
      Serial.println("Could not connect to local WiFi.");
      return;
    }

    delay(500);
    Serial.print(".");
    LEDs[count] = CRGB::Green;
    FastLED.show();
    count++;
  }
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  WifiConnected = true;
  //IPAddress ip = WiFi.localIP();

  if (WifiConnected) {
    Serial.println("Wait for time sync");  // for accurate clock:
    millis_since_start = millis();
    while (!waitForSync(20)) Serial.println("Reconnecting...");
    Serial.println("time synced");
    Serial.print("it took: ");
    unsigned long delta = millis() - millis_since_start;
    Serial.print(delta);
    Serial.println(" ms");
    Serial.println(myTZ.dateTime("H:i:s"));
    setServer("nl.pool.ntp.org");
    if (!myTZ.setCache(0)) myTZ.setLocation(F("nl"));  // set to Dutch time incl DST
  }
  httpUpdateServer.setup(&server);

  // Handlers
  server.on("/color", HTTP_POST, []() {
    r_val = server.arg("r").toInt();
    g_val = server.arg("g").toInt();
    b_val = server.arg("b").toInt();
    server.send(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/setdate", HTTP_POST, []() {
    // Sample input: date = "Dec 06 2009", time = "12:34:56"
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    String datearg = server.arg("date");
    String timearg = server.arg("time");
    Serial.println(datearg);
    Serial.println(timearg);
    char d[12];
    char t[9];
    datearg.toCharArray(d, 12);
    timearg.toCharArray(t, 9);
    clockMode = 0;
    server.send(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/brightness", HTTP_POST, []() {
    brightness = server.arg("brightness").toInt();
    server.send(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/countdown", HTTP_POST, []() {
    countdownMilliSeconds = server.arg("ms").toInt();
    byte cd_r_val = server.arg("r").toInt();
    byte cd_g_val = server.arg("g").toInt();
    byte cd_b_val = server.arg("b").toInt();
    digitalWrite(COUNTDOWN_OUTPUT, LOW);
    countdownColor = CRGB(cd_r_val, cd_g_val, cd_b_val);
    endCountDownMillis = millis() + countdownMilliSeconds;
    allBlank();
    clockMode = 1;
    server.send(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/temperature", HTTP_POST, []() {
    temperatureCorrection = server.arg("correction").toInt();
    temperatureSymbol = server.arg("symbol").toInt();
    clockMode = 2;
    server.send(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/scoreboard", HTTP_POST, []() {
    scoreboardLeft = server.arg("left").toInt();
    scoreboardRight = server.arg("right").toInt();
    scoreboardColorLeft = CRGB(server.arg("rl").toInt(), server.arg("gl").toInt(), server.arg("bl").toInt());
    scoreboardColorRight = CRGB(server.arg("rr").toInt(), server.arg("gr").toInt(), server.arg("br").toInt());
    clockMode = 3;
    server.send(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/hourformat", HTTP_POST, []() {
    hourFormat = server.arg("hourformat").toInt();
    clockMode = 0;
    server.send(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/clock", HTTP_POST, []() {
    clockMode = 0;
    server.send(200, "text/json", "{\"result\":\"ok\"}");
  });

  // Before uploading the files with the "ESP8266 Sketch Data Upload" tool, zip the files with the command "gzip -r ./data/" (on Windows I do this with a Git Bash)
  // *.gz files are automatically unpacked and served from your ESP (so you don't need to create a handler for each file).
  server.serveStatic("/", SPIFFS, "/", "max-age=86400");
  server.begin();

  SPIFFS.begin();
  Serial.println("SPIFFS contents:");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
  }
  Serial.println();
  ArduinoOTA.setHostname("RGB_7Segment");
  ArduinoOTA.begin();
  digitalWrite(COUNTDOWN_OUTPUT, LOW);
  tempSensors.begin();                                                                  // start temperature sensor
  oneWireCount = tempSensors.getDeviceCount();                                          // find the number of connected sensors
  Serial.println(" There are: " + String(oneWireCount) + " temperaturesensors found");  // check whether a temperature sensor has been detected
  TemperatureSensor = (oneWireCount != 0) ? true : false;
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  int currentSec = myTZ.second();
  if (currentSec != prevSec) {
    prevSec = currentSec;
    if (clockMode == 0) {
      updateClock();
    } else if (clockMode == 1) {
      updateCountdown();
    } else if (clockMode == 2) {
    // do nothing
    } else if (clockMode == 3) {
      updateScoreboard();
    }
    FastLED.setBrightness(brightness);
    FastLED.show();
  }
}
