
#include <WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

const char* ssid      = "XXXXXXXXXX"; // Nama SSID Jaringan
const char* password  = "XXXXXXXXXX"; // Password SSID Jaringan

WiFiClient  client;

unsigned long myChannelNumber = X; //Nomor Channel
const char * myWriteAPIKey = "XXXXXXXXXXXX"; //Write API Channel

// Timer variables
unsigned long lastTime = 0;
/*Jeda dalam milidetik
  2000 = 2 detik (30 data dalam 1 menit)
  5000 = 5 detik (12 data dalam 1 menit)
  15000 = 15 detik (4 data dalam 1 menit) WAJIB BILA LISENSI GRATIS UNTUK SAMPLING MAKS
  30000 = 30 detik (2 data dalam 1 menit)
*/
unsigned long timerDelay = 15000; 

// Variable untuk menampilkan tipe data
float temperature;
float humidity;
float pressure;


// Membuat objek data
Adafruit_BME280 bme; //BME280 sambungkan ESP32 I2C (GPIO 21 = SDA, GPIO 22 = SCL)

// Inisialisasi Sensor BME280
void initBME(){
  if (!bme.begin(0x76))
  {
    Serial.println("Gagal Menemukan BME280, Mohon Cek Sambungan Kabel I2C");
    while (1);
  }
}
// Program yang dimulai
void setup() {
  Serial.begin(115200);  //Inisialisasi Serial
  initBME();
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Inisialisasi ThingSpeak
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Menyambungkan WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Melakukan Penyambungan Internet...");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nTersambung.");
    }

    // Mengambil data sensor
    digitalWrite(LED_BUILTIN, HIGH); // Indikator Mengawali Tugas
    temperature = bme.readTemperature();
    Serial.print("Temperature (ºC): ");
    Serial.println(temperature);
    humidity = bme.readHumidity();
    Serial.print("Humidity (Rh%): ");
    Serial.println(humidity);
    pressure = bme.readPressure() / 100.0F;
    Serial.print("Pressure (hPa): ");
    Serial.println(pressure);
    
    // Menampilkan variabel ke ThingSpeak
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, pressure);
    
    // Menulis data ke Thingspeak
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Field Berhasil Diperbaharui.");
    }
    else{
      Serial.println("Terjadi Masalah Pembaharuan Field. Kode Error HTTP " + String(x));
    }
    lastTime = millis();
    digitalWrite(LED_BUILTIN, LOW); // Indikator Menyelesaikan Tugas
  }
}
