/* Smart Mushroom Controller (Non-blocking)
   Blynk + Telegram + DHT22 + Fan/Humidifier
   Relay is ACTIVE LOW: LOW = ON, HIGH = OFF
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "DHT.h"
#define BLYNK_TEMPLATE_ID "TMPL3kvXnZFlJ"
#define BLYNK_TEMPLATE_NAME "Smart Mushroom Cultivation"
#define BLYNK_AUTH_TOKEN "y2EbNfbNNvBHfInzTLdSDLw_zoAkmW20"
#include <BlynkSimpleEsp32.h>


// --- Pins ---
#define DHTPIN 4
#define DHTTYPE DHT22
#define FAN_PIN 25
#define HUMIDIFIER_PIN 26

// --- NEW LDR/LED PINS ---
#define LDR_PIN 34
#define LED_PIN 27

// --- Wi-Fi credentials ---
const char* ssid = "ABHIJITH 1442";
const char* password = "P[9574t3";

// --- Telegram ---
#define BOT_TOKEN "7911837990:AAEU9pxk6lWIlNAa-kYOQIS5-Pcg1iv54a8"
#define CHAT_ID "787994958"
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// --- Blynk ---
char blynkAuth[] = BLYNK_AUTH_TOKEN;

// --- Objects ---
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// --- States ---
bool fanState = false;
bool humidifierState = false;
bool tempAlertSent = false;
bool humidAlertSent = false;

// --- Timing ---
unsigned long lastStatusTime = 0;
const unsigned long statusInterval = 300000; // 5 minutes

String getStatusString();
void sendSensorsToBlynk();
void sendStatusTelegram();
void checkTelegram();

// ================== Setup ==================
void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(FAN_PIN, OUTPUT);
  pinMode(HUMIDIFIER_PIN, OUTPUT);

  // Relay is ACTIVE LOW: OFF = HIGH
  digitalWrite(FAN_PIN, HIGH);
  digitalWrite(HUMIDIFIER_PIN, HIGH);

  // --- NEW LDR/LED SETUP ---
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // --- Connect Wi-Fi ---
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  // --- Blynk ---
  Blynk.begin(blynkAuth, ssid, password);

  // --- Telegram ---
  client.setInsecure();
  bot.sendMessage(CHAT_ID, "🍄 Mushroom Climate Controller Started!", "");

  // --- Timers ---
  timer.setInterval(2000L, sendSensorsToBlynk);
  timer.setInterval(3000L, checkTelegram);
  timer.setInterval(statusInterval, sendStatusTelegram);

  Serial.println("\n✅ Setup Complete!");
}

// ================== Main Loop ==================
void loop() {
  Blynk.run();
  timer.run();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (!isnan(humidity) && !isnan(temperature)) {

    // --- Auto Climate Logic ---
    if (temperature > 30 && humidity < 80) { fanState = true; humidifierState = true; }
    else if (temperature > 30 && humidity >= 80) { fanState = true; humidifierState = false; }
    else if (temperature <= 30 && humidity < 80) { fanState = false; humidifierState = true; }
    else if (humidity > 95) { fanState = true; humidifierState = false; }
    else { fanState = false; humidifierState = false; }

    digitalWrite(FAN_PIN, fanState ? LOW : HIGH);
    digitalWrite(HUMIDIFIER_PIN, humidifierState ? LOW : HIGH);

    // --- Alerts ---
    if (temperature > 32 && !tempAlertSent) { 
      bot.sendMessage(CHAT_ID, "⚠️ Temperature above 32°C!", "");
      tempAlertSent = true; 
    } else if (temperature <= 32) tempAlertSent = false;

    if (humidity < 80 && !humidAlertSent) { 
      bot.sendMessage(CHAT_ID, "⚠️ Humidity below 80%!", "");
      humidAlertSent = true; 
    } else if (humidity >= 80) humidAlertSent = false;

    // --- NEW LDR READ + LED CONTROL ---
    int ldrValue = analogRead(LDR_PIN);

    if (ldrValue < 1500) {
      digitalWrite(LED_PIN, HIGH);  // dark
    } else {
      digitalWrite(LED_PIN, LOW);   // bright
    }

    // --- Debug ---
    Serial.printf("Temp: %.2f°C | Hum: %.2f%% | Fan: %s | Humidifier: %s | LDR: %d | LED: %s\n",
      temperature, humidity,
      fanState ? "ON" : "OFF",
      humidifierState ? "ON" : "OFF",
      ldrValue,
      digitalRead(LED_PIN) ? "ON" : "OFF"
    );
  }
}

// ================== Blynk Handlers ==================
BLYNK_WRITE(V3) {
  fanState = param.asInt();
  digitalWrite(FAN_PIN, fanState ? LOW : HIGH);
}

BLYNK_WRITE(V4) {
  humidifierState = param.asInt();
  digitalWrite(HUMIDIFIER_PIN, humidifierState ? LOW : HIGH);
}

BLYNK_WRITE(V5) {
  if (param.asInt() == 1) {
    String s = getStatusString();
    Blynk.virtualWrite(V6, s);
    bot.sendMessage(CHAT_ID, s, "");
    Blynk.virtualWrite(V5, 0);
  }
}

// ================== Helper Functions ==================
String getStatusString() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  String msg = "🌡 Temp: " + String(t) + "°C\n";
  msg += "💧 Humidity: " + String(h) + "%\n";
  msg += "💨 Fan: " + String(fanState ? "ON" : "OFF") + "\n";
  msg += "💦 Humidifier: " + String(humidifierState ? "ON" : "OFF");
  return msg;
}

void sendSensorsToBlynk() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) return;

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V3, fanState ? 1 : 0);
  Blynk.virtualWrite(V4, humidifierState ? 1 : 0);
}

void sendStatusTelegram() {
  bot.sendMessage(CHAT_ID, getStatusString(), "");
}

void checkTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    if (chat_id != CHAT_ID) continue;

    if (text == "/start") {
      bot.sendMessage(chat_id, "Commands:\n/status\n/fan_on\n/fan_off\n/humid_on\n/humid_off", "");
    } 
    else if (text == "/status") {
      bot.sendMessage(chat_id, getStatusString(), "");
    }
    else if (text == "/fan_on") {
      fanState = true;
      digitalWrite(FAN_PIN, LOW);
      Blynk.virtualWrite(V3, 1);
    } 
    else if (text == "/fan_off") {
      fanState = false;
      digitalWrite(FAN_PIN, HIGH);
      Blynk.virtualWrite(V3, 0);
    } 
    else if (text == "/humid_on") {
      humidifierState = true;
      digitalWrite(HUMIDIFIER_PIN, LOW);
      Blynk.virtualWrite(V4, 1);
    } 
    else if (text == "/humid_off") {
      humidifierState = false;
      digitalWrite(HUMIDIFIER_PIN, HIGH);
      Blynk.virtualWrite(V4, 0);
    }
  }
}
