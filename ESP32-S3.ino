#include <ArduinoJson.h>
#include "HWCDC.h"

// Inisialisasi Native USB Serial
HWCDC USBSerial;

// Pin untuk komunikasi ke ESP-01 (Uah sesuai dengan wiring Anda)
#define RX2 44
#define TX2 43

// Variabel Global
float suhuSekarang = 0;
float lembapSekarang = 0;
String jsonBuffer = "";
bool isCollectingJson = false;

void setup() {
  // Mulai komunikasi USB native
  USBSerial.begin(115200);
  
  // Mulai komunikasi UART ke ESP-01
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  
  // Tunggu agar USBSerial siap (khusus S3)
  while (!USBSerial) { delay(100); }
  
  USBSerial.println("--- ESP32-S3 Master Ready ---");
}

void loop() {
  // 1. Kirim perintah dari PC ke ESP-01
  if (USBSerial.available()) {
    String cmd = USBSerial.readStringUntil('\n');
    Serial2.println(cmd);
  }

  // 2. Baca Respon dari ESP-01 melalui Serial2
  if (Serial2.available()) {
    String response = Serial2.readStringUntil('\n');
    response.trim();

    if (response.startsWith("PAYLOAD:")) {
      isCollectingJson = true;
      jsonBuffer = response.substring(8);
    } 
    else if (isCollectingJson) {
      jsonBuffer += response;
      jsonBuffer += " "; 
      
      if (response.endsWith("}")) {
        isCollectingJson = false;
        USBSerial.print("DEBUG - Buffer Lengkap: ");
        USBSerial.println(jsonBuffer);
        
        parseDataSensor(jsonBuffer);
        jsonBuffer = "";
      }
    }
    else {
      USBSerial.print("[ESP01]: ");
      USBSerial.println(response);
    }
  }
}

void parseDataSensor(String json) {
  StaticJsonDocument<256> doc; // Ditingkatkan ukurannya sedikit agar lebih aman
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    USBSerial.print("Parsing Gagal: ");
    USBSerial.println(error.f_str());
    return;
  }

  suhuSekarang = doc["data"]["suhu"];
  lembapSekarang = doc["data"]["kelembapan"];

  USBSerial.println("--- Data Berhasil Disimpan ---");
  USBSerial.print("Suhu: "); USBSerial.println(suhuSekarang);
  USBSerial.print("Kelembapan: "); USBSerial.println(lembapSekarang);
}
