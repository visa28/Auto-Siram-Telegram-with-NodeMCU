#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <RTClib.h>

// Ganti dengan SSID WiFi Anda
const char* ssid = "DEVFTEAM";
// Ganti dengan password WiFi Anda
const char* password = "D3VFTEAM";
// Ganti dengan token bot Telegram Anda
const char* telegramBotToken = "7040936290:AAEtgIJHGCQmcn6GTXdkNY4zhUQDGI5zlZU";

const int relayPin = 23;
const int buttonPin = 19;

WiFiClientSecure client;
UniversalTelegramBot bot(telegramBotToken, client);

RTC_DS3231 rtc;

bool manualMode = false;
bool pumpState = false;
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

const int maxSchedules = 10;
int scheduleCount = 2;
int scheduleHours[maxSchedules] = {7, 16};
int scheduleMinutes[maxSchedules] = {0, 0};
int scheduleDurations[maxSchedules] = {300, 300}; 

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    if (text == "/start") {
      String welcome = "Selamat datang di Bot Penyiram Tanaman!\n";
      welcome += "/pump_on - Nyalakan pompa\n";
      welcome += "/pump_off - Matikan pompa\n";
      welcome += "/set_schedule - Atur jadwal penyiraman (format: HH:MM,HH:MM,...)\n";
      welcome += "/set_duration - Atur durasi penyiraman (format: detik,detik,...)\n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/pump_on") {
      pumpState = true;
      digitalWrite(relayPin, HIGH);
      bot.sendMessage(chat_id, "Pompa sekarang MENYALA", "");
    }

    if (text == "/pump_off") {
      pumpState = false;
      digitalWrite(relayPin, LOW);
      bot.sendMessage(chat_id, "Pompa sekarang MATI", "");
    }

    if (text.startsWith("/set_schedule ")) {
      String scheduleString = text.substring(14);
      scheduleCount = 0;
      int lastIndex = 0;
      while (lastIndex < scheduleString.length() && scheduleCount < maxSchedules) {
        int commaIndex = scheduleString.indexOf(',', lastIndex);
        if (commaIndex == -1) commaIndex = scheduleString.length();
        
        String timeString = scheduleString.substring(lastIndex, commaIndex);
        int colonIndex = timeString.indexOf(':');
        if (colonIndex != -1) {
          int hour = timeString.substring(0, colonIndex).toInt();
          int minute = timeString.substring(colonIndex + 1).toInt();
          if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60) {
            scheduleHours[scheduleCount] = hour;
            scheduleMinutes[scheduleCount] = minute;
            scheduleCount++;
          }
        }
        lastIndex = commaIndex + 1;
      }
      bot.sendMessage(chat_id, "Jadwal penyiraman diatur ulang", "");
      Serial.print("Jadwal penyiraman baru: ");
      for (int j = 0; j < scheduleCount; j++) {
        Serial.print(scheduleHours[j]);
        Serial.print(":");
        Serial.print(scheduleMinutes[j]);
        if (j < scheduleCount - 1) {
          Serial.print(", ");
        }
      }
      Serial.println();
    }

    if (text.startsWith("/set_duration ")) {
      String durationString = text.substring(14);
      int lastIndex = 0;
      int count = 0;
      while (lastIndex < durationString.length() && count < maxSchedules) {
        int commaIndex = durationString.indexOf(',', lastIndex);
        if (commaIndex == -1) commaIndex = durationString.length();
        
        int duration = durationString.substring(lastIndex, commaIndex).toInt();
        if (duration > 0) {
          scheduleDurations[count] = duration;
          count++;
        }
        lastIndex = commaIndex + 1;
      }
      bot.sendMessage(chat_id, "Durasi penyiraman diatur ulang", "");
      Serial.print("Durasi penyiraman baru: ");
      for (int j = 0; j < count; j++) {
        Serial.print(scheduleDurations[j]);
        Serial.print(" detik");
        if (j < count - 1) {
          Serial.print(", ");
        }
      }
      Serial.println();
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("Memulai...");

  Wire.begin(21, 22);

  if (!rtc.begin()) {
    Serial.println("Tidak dapat menemukan RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC kehilangan daya, mengatur waktu!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke WiFi...");
  }
  Serial.println("Terhubung ke WiFi");

  client.setInsecure();
}

void loop() {
  DateTime now = rtc.now();

  // Cetak waktu saat ini untuk debugging
  Serial.print("Sekarang: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  // Jadwal penyiraman otomatis
  for (int i = 0; i < scheduleCount; i++) {
    if (now.hour() == scheduleHours[i] && now.minute() == scheduleMinutes[i] && now.second() == 0) {
      Serial.println("Waktu penyiraman - menyalakan pompa");
      digitalWrite(relayPin, HIGH);
      unsigned long startTime = millis();
      while (millis() - startTime < scheduleDurations[i] * 1000) {
        delay(1000);
      }
      Serial.println("Mematikan pompa");
      digitalWrite(relayPin, LOW);
    }
  }

  // Kontrol manual dengan tombol
  if (digitalRead(buttonPin) == LOW) {
    delay(50);
    if (digitalRead(buttonPin) == LOW) {
      manualMode = !manualMode;
      if (manualMode) {
        pumpState = !pumpState;
        digitalWrite(relayPin, pumpState);
        Serial.println("Mode manual - mengubah status pompa");
      } else {
        digitalWrite(relayPin, LOW);
        Serial.println("Mode manual - mematikan pompa");
      }
      while (digitalRead(buttonPin) == LOW);
    }
  }

  // Periksa pesan Telegram baru setiap detik
  if (millis() - lastTime > timerDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTime = millis();
  }

  delay(1000);
}
