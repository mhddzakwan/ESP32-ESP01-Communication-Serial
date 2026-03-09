// connect,KelilingDunia,12julham12
// get,http://ummuhafidzah.sch.id/iot/get_sensor.php
// send,http://ummuhafidzah.sch.id/iot/update_sensor.php?suhu=4&kelembapan=10
// disconnect
// check

#include <ArduinoJson.h> // Library untuk memparsing JSON

#define RX2 16
#define TX2 17

// Variabel Global untuk menyimpan data dari ESP01
float suhuSekarang = 0;
float lembapSekarang = 0;
String statusWiFi = "TIDAK TERHUBUNG";
String jsonBuffer = ""; // Tambahkan variabel global ini di atas setup()
bool isCollectingJson = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  
  delay(2000);
  Serial.println("--- ESP32 Master Ready ---");
}
void loop() {
  if (Serial.available()) {
    Serial2.println(Serial.readStringUntil('\n'));
  }

  if (Serial2.available()) {
    String response = Serial2.readStringUntil('\n');
    response.trim();

    if (response.startsWith("PAYLOAD:")) {
      isCollectingJson = true;
      jsonBuffer = response.substring(8); // Ambil sisa baris setelah "PAYLOAD:"
    } 
    else if (isCollectingJson) {
      jsonBuffer += response; // Gabungkan baris
      jsonBuffer += " ";      // TAMBAHKAN SPASI agar antar baris tidak menempel
      
      // Jika baris terakhir mengandung '}', berarti JSON sudah lengkap
      if (response.endsWith("}")) {
        isCollectingJson = false;
        
        // --- DEBUG: Lihat hasil penggabungan sebelum diparsing ---
        Serial.print("DEBUG - Hasil akhir buffer: ");
        Serial.println(jsonBuffer);
        
        parseDataSensor(jsonBuffer);
        jsonBuffer = ""; // Reset buffer
      }
    }
    else {
      Serial.print("[ESP01]: ");
      Serial.println(response);
    }
  }
}

// Fungsi untuk memparsing JSON dan menyimpan ke variabel float
void parseDataSensor(String json) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print("Parsing Gagal: ");
    Serial.println(error.f_str());
    return;
  }

  // Ambil data dari struktur JSON: {"status":"success","data":{"suhu":4,"kelembapan":10...}}
  suhuSekarang = doc["data"]["suhu"];
  lembapSekarang = doc["data"]["kelembapan"];

  // Sekarang data sudah ada di variabel dan bisa digunakan!
  Serial.println("--- Data Berhasil Disimpan di Variabel ESP32 ---");
  Serial.print("Suhu: "); Serial.println(suhuSekarang);
  Serial.print("Kelembapan: "); Serial.println(lembapSekarang);
  
  // Contoh penggunaan variabel:
  if (suhuSekarang > 30.0) {
    Serial.println("Peringatan: Suhu terlalu panas!");
  }
}
