// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>
// #include <ArduinoJson.h>
// #include "DHT.h"

// // --- Pins ---
// #define DHTPIN 4
// #define DHTTYPE DHT22
// #define FAN_PIN 25
// #define HUMIDIFIER_PIN 26
// #define LDR_PIN 34         // LDR analog input pin
// #define LIGHT_PIN 27       // Optional grow light output (you can remove if not needed)

// // --- Wi-Fi credentials ---
// const char* ssid = "ABHIJITH 1442";
// const char* password = "P[9574t3";

// // --- Telegram Bot ---
// #define BOT_TOKEN "7911837990:AAEU9pxk6lWIlNAa-kYOQIS5-Pcg1iv54a8"
// #define CHAT_ID "787994958"

// WiFiClientSecure client;
// UniversalTelegramBot bot(BOT_TOKEN, client);
// DHT dht(DHTPIN, DHTTYPE);

// // --- Timing control ---
// unsigned long lastStatusTime = 0;
// const unsigned long statusInterval = 300000; // 5 minutes

// // --- Alert flags ---
// bool tempAlertSent = false;
// bool humidAlertSent = false;
// bool lightAlertSent = false;

// void setup() {
//   Serial.begin(115200);
//   dht.begin();

//   pinMode(FAN_PIN, OUTPUT);
//   pinMode(HUMIDIFIER_PIN, OUTPUT);
//   pinMode(LIGHT_PIN, OUTPUT);
//   digitalWrite(FAN_PIN, LOW);
//   digitalWrite(HUMIDIFIER_PIN, LOW);
//   digitalWrite(LIGHT_PIN, LOW);

//   // --- Connect to Wi-Fi ---
//   Serial.print("Connecting to WiFi");
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\n✅ WiFi Connected!");
//   Serial.println(WiFi.localIP());

//   client.setInsecure();  // skip SSL certificate check
//   bot.sendMessage(CHAT_ID, "🍄 Mushroom Climate Controller + Light Monitor Started!", "");
// }

// void loop() {
//   float humidity = dht.readHumidity();
//   float temperature = dht.readTemperature();
//   int ldrValue = analogRead(LDR_PIN);   // LDR value (0–4095)
//   float lightPercent = (ldrValue / 4095.0) * 100; // optional normalization

//   if (isnan(humidity) || isnan(temperature)) {
//     Serial.println("❌ DHT22 sensor error!");
//     delay(2000);
//     return;
//   }

//   // --- Control Logic ---
//   bool fanState = LOW;
//   bool humidifierState = LOW;
//   bool lightState = LOW;

//   // 🌡️ Temperature & Humidity Control
//   if (temperature > 30 && humidity < 80) {
//     fanState = HIGH;
//     humidifierState = HIGH;
//   } else if (temperature > 30 && humidity >= 80) {
//     fanState = HIGH;
//     humidifierState = LOW;
//   } else if (temperature <= 30 && humidity < 80) {
//     fanState = LOW;
//     humidifierState = HIGH;
//   } else if (humidity > 95) {
//     fanState = HIGH;
//     humidifierState = LOW;
//   } else {
//     fanState = LOW;
//     humidifierState = LOW;
//   }

//   // 💡 Light Control (optional)
//   if (ldrValue < 1000) { // too dark → turn ON light
//     lightState = HIGH;
//   } else {
//     lightState = LOW;
//   }

//   // --- Apply States ---
//   digitalWrite(FAN_PIN, fanState);
//   digitalWrite(HUMIDIFIER_PIN, humidifierState);
//   digitalWrite(LIGHT_PIN, lightState);

//   // --- Serial Log ---
//   Serial.printf("Temp: %.2f°C | Hum: %.2f%% | Light: %.0f%% | Fan: %s | Humidifier: %s | Light: %s\n",
//                 temperature, humidity, lightPercent,
//                 fanState ? "ON" : "OFF",
//                 humidifierState ? "ON" : "OFF",
//                 lightState ? "ON" : "OFF");

//   // --- Telegram alerts ---
//   if (temperature > 32 && !tempAlertSent) {
//     bot.sendMessage(CHAT_ID, "⚠️ Alert: Temperature is above 32°C!", "");
//     tempAlertSent = true;
//   } else if (temperature <= 32) tempAlertSent = false;

//   if (humidity < 80 && !humidAlertSent) {
//     bot.sendMessage(CHAT_ID, "⚠️ Alert: Humidity is below 80%!", "");
//     humidAlertSent = true;
//   } else if (humidity >= 80) humidAlertSent = false;

//   if (ldrValue < 800 && !lightAlertSent) {
//     bot.sendMessage(CHAT_ID, "💡 Alert: Light level is low!", "");
//     lightAlertSent = true;
//   } else if (ldrValue >= 800) lightAlertSent = false;

//   // --- Telegram command handling ---
//   int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//   while (numNewMessages) {
//     handleNewMessages(numNewMessages);
//     numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//   }

//   // --- Periodic status update ---
//   if (millis() - lastStatusTime > statusInterval) {
//     String msg = "🌡 Temp: " + String(temperature) + "°C\n💧 Humidity: " + String(humidity) + "%\n";
//     msg += "💨 Fan: " + String(fanState ? "ON" : "OFF") + "\n";
//     msg += "💦 Humidifier: " + String(humidifierState ? "ON" : "OFF") + "\n";
//     msg += "💡 Light: " + String(lightPercent, 0) + "% (" + (lightState ? "ON" : "OFF") + ")";
//     bot.sendMessage(CHAT_ID, msg, "");
//     lastStatusTime = millis();
//   }

//   delay(2000);
// }

// // --- Handle Telegram commands ---
// void handleNewMessages(int numNewMessages) {
//   for (int i = 0; i < numNewMessages; i++) {
//     String chat_id = String(bot.messages[i].chat_id);
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "DHT.h"

// --- Pins ---
#define DHTPIN 4
#define DHTTYPE DHT22
#define FAN_PIN 25
#define HUMIDIFIER_PIN 26

// --- Wi-Fi credentials ---
const char* ssid = "ABHIJITH 1442";
const char* password = "P[9574t3";

// --- Telegram Bot ---
#define BOT_TOKEN "7911837990:AAEU9pxk6lWIlNAa-kYOQIS5-Pcg1iv54a8"
#define CHAT_ID "787994958"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
DHT dht(DHTPIN, DHTTYPE);

// --- Timing control ---
unsigned long lastStatusTime = 0;
const unsigned long statusInterval = 300000; // 5 minutes

// --- Alert flags ---
bool tempAlertSent = false;
bool humidAlertSent = false;

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(FAN_PIN, OUTPUT);
  pinMode(HUMIDIFIER_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(HUMIDIFIER_PIN, LOW);

  // --- Connect to Wi-Fi ---
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected!");
  Serial.println(WiFi.localIP());

  client.setInsecure();  // skip SSL certificate check
  bot.sendMessage(CHAT_ID, "🍄 Mushroom Climate Controller Started!", "");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("❌ DHT22 sensor error!");
    delay(2000);
    return;
  }

  // --- Control Logic ---
  bool fanState = LOW;
  bool humidifierState = LOW;

if (temperature > 30 && humidity < 80) {
  // Hot and dry → cool & humidify
  fanState = HIGH;
  humidifierState = HIGH;

} else if (temperature > 30 && humidity >= 80) {
  // Hot but humid enough → only cool
  fanState = HIGH;
  humidifierState = LOW;

} else if (temperature <= 30 && humidity < 80) {
  // Cool enough but still dry → only humidify
  fanState = LOW;
  humidifierState = HIGH;

} else if (humidity > 95) {
  // Too humid → stop humidifier, maybe ventilate a bit
  fanState = HIGH;
  humidifierState = LOW;

} else {
  // Normal balanced condition
  fanState = LOW;
  humidifierState = LOW;
}

  digitalWrite(FAN_PIN, fanState);
  digitalWrite(HUMIDIFIER_PIN, humidifierState);

  Serial.printf("Temp: %.2f°C | Hum: %.2f%% | Fan: %s | Humidifier: %s\n",
                temperature, humidity, fanState ? "ON" : "OFF", humidifierState ? "ON" : "OFF");

  // --- Telegram alerts ---
  if (temperature > 32 && !tempAlertSent) {
    bot.sendMessage(CHAT_ID, "⚠️ Alert: Temperature is above 32°C!", "");
    tempAlertSent = true;  // prevent repeated alerts
  } 
  else if (temperature <= 32) {
    tempAlertSent = false; // reset when back to normal
  }

  if (humidity < 80 && !humidAlertSent) {
    bot.sendMessage(CHAT_ID, "⚠️ Alert: Humidity is below 80%!", "");
    humidAlertSent = true; // prevent repeated alerts
  } 
  else if (humidity >= 80) {
    humidAlertSent = false; // reset when back to normal
  }

  // --- Telegram message handling ---
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  // --- Periodic status update every 5 minutes ---
  if (millis() - lastStatusTime > statusInterval) {
    String msg = "🌡 Temp: " + String(temperature) + "°C\n💧 Humidity: " + String(humidity) + "%\n";
    msg += "💨 Fan: " + String(fanState ? "ON" : "OFF") + "\n";
    msg += "💦 Humidifier: " + String(humidifierState ? "ON" : "OFF");
    bot.sendMessage(CHAT_ID, msg, "");
    lastStatusTime = millis();
  }

  delay(2000); // small delay
}

// --- Handle Telegram commands ---
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    if (chat_id != CHAT_ID) continue; // ignore messages from others

    if (text == "/start") {
      bot.sendMessage(chat_id, "Welcome to 🍄 Mushroom Climate Controller!\n\n"
                               "Commands:\n"
                               "/status - Show temperature & humidity\n"
                               "/fan_on - Turn ON fan\n"
                               "/fan_off - Turn OFF fan\n"
                               "/humid_on - Turn ON humidifier\n"
                               "/humid_off - Turn OFF humidifier", "");
    }
    else if (text == "/status") {
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      String msg = "🌡 Temp: " + String(t) + "°C\n💧 Humidity: " + String(h) + "%\n";
      msg += "💨 Fan: " + String(digitalRead(FAN_PIN) ? "ON" : "OFF") + "\n";
      msg += "💦 Humidifier: " + String(digitalRead(HUMIDIFIER_PIN) ? "ON" : "OFF");
      bot.sendMessage(chat_id, msg, "");
    }
    else if (text == "/fan_on") {
      digitalWrite(FAN_PIN, HIGH);
      bot.sendMessage(chat_id, "💨 Fan turned ON", "");
    }
    else if (text == "/fan_off") {
      digitalWrite(FAN_PIN, LOW);
      bot.sendMessage(chat_id, "💨 Fan turned OFF", "");
    }
    else if (text == "/humid_on") {
      digitalWrite(HUMIDIFIER_PIN, HIGH);
      bot.sendMessage(chat_id, "💦 Humidifier turned ON", "");
    }
    else if (text == "/humid_off") {
      digitalWrite(HUMIDIFIER_PIN, LOW);
      bot.sendMessage(chat_id, "💦 Humidifier turned OFF", "");
    }
  }
}
