#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

void setup() {
  // Baud rate harus sama dengan Serial2 pada ESP32
  Serial.begin(115200); 
  delay(500);
  
  // Pesan inisialisasi singkat untuk memberi tahu ESP32 bahwa ESP01 sudah siap
  Serial.println("READY");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.startsWith("connect")) {
      handleConnect(input);
    } 
    else if (input.startsWith("send") || input.startsWith("get")) {
      handleHttpRequest(input);
    }
    else if (input.equalsIgnoreCase("disconnect")) {
      WiFi.disconnect();
      Serial.println("DISCONNECTED");
    } 
    else if (input.equalsIgnoreCase("check")) {
      if(WiFi.status() == WL_CONNECTED) {
        Serial.println("CONNECTED," + WiFi.SSID() + "," + WiFi.localIP().toString());
      } else {
        Serial.println("NOT_CONNECTED");
      }
    }
  }
}

void handleConnect(String data) {
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);
  if (firstComma == -1 || secondComma == -1) {
    Serial.println("ERROR:Format connect salah");
    return;
  }

  String ssid = data.substring(firstComma + 1, secondComma);
  String password = data.substring(secondComma + 1);

  WiFi.begin(ssid.c_str(), password.c_str());

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("CONNECTED_OK," + WiFi.localIP().toString());
  } else {
    Serial.println("CONNECTED_FAILED");
  }
}

void handleHttpRequest(String input) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("ERROR:No WiFi");
    return;
  }

  int firstComma = input.indexOf(',');
  if (firstComma == -1) return;

  String url = input.substring(firstComma + 1);
  url.trim();

  WiFiClient client;
  HTTPClient http;

  if (http.begin(client, url)) {
    int httpCode = http.GET();
    // Di ESP-01, modifikasi bagian ini:
    // GANTI CARA PENGIRIMAN DI ESP-01 MENJADI SEPERTI INI:
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      payload.trim();
      payload.replace("\n", ""); // Hapus semua enter
      payload.replace("\r", ""); // Hapus semua carriage return
      Serial.print("PAYLOAD:");
      Serial.println(payload); // Sekarang ini pasti satu baris utuh!
      Serial.println(payload); // Ini harusnya sudah satu baris jika payload-nya satu baris
    }
    http.end();
  } else {
    Serial.println("ERROR:Connect Host Failed");
  }
}
