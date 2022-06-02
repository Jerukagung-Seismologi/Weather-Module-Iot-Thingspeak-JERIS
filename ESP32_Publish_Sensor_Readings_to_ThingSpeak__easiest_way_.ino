
#include <WiFi.h>
#include <WiFiClient.h>
#include "ThingSpeak.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
//#in

const char* ssid      = "Jerukagung Seismologi";   // your network SSID (name) 
const char* password  = "seiscalogi";   // your network password

WiFiClient  client;

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "S1VN6AN4R1QT4QMD";

// Timer variables
unsigned long lastTime = 0;
/*Jeda dalam milidetik
  2000 = 2 detik (30 data dalam 1 menit)
  5000 = 5 detik (12 data dalam 1 menit)
  15000 = 15 detik (4 data dalam 1 menit) WAJIB BILA LISENSI GR
*/
unsigned long timerDelay = 15000; 

// Variable to hold temperature readings
float temperature;
//float humidity; BME280
float pressure;


// Create a sensor object
Adafruit_BMP280 bmp; //BMP280 connect to ESP32 I2C (GPIO 21 = SDA, GPIO 22 = SCL)
//Adafruit_BME280 bme;

void initBMP(){
  if (!bmp.begin(0x76))
  //if (!bme.begin(0x76))
  {
    Serial.println("Gagal Menemukan BMP280/BME280, Mohon Cek Sambungan Kabel I2C");
    while (1);
  }
}

void setup() {
  Serial.begin(115200);  //Initialize serial
  initBMP();
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Melakukan Penyambungan Internet...");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nTersambung.");
    }

    // Get a new temperature reading
    temperature = bmp.readTemperature();
    Serial.print("Temperature (ºC): ");
    Serial.println(temperature);
    /*humidity = bme.readHumidity();
    Serial.print("Humidity (%): ");
    Serial.println(humidity);*/
    pressure = bmp.readPressure() / 100.0F;
    Serial.print("Pressure (hPa): ");
    Serial.println(pressure);
    
    // set the fields with the values
    ThingSpeak.setField(1, temperature);
    //ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, pressure);
    digitalWrite(LED_BUILTIN, HIGH);
    
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel Berhasil Diperbaharui.");
    }
    else{
      Serial.println("Terjadi Masalah Pembaharuan Channel. Kode Error HTTP " + String(x));
    }
    lastTime = millis();
    digitalWrite(LED_BUILTIN, LOW);
  }
}
