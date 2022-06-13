#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "ThingSpeak.h"

const char* ssid      = "Replace with your network name";   
const char* password  = "Replace with your network password";

WiFiClient client;

unsigned long myChannelNumber = 1;
const char* myWriteAPIKey = "REPLACE WITH YOUR WRITE KEY API";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 14000; //MINIMAL 14 DETIK BIAR EFEKTIF
const int ledStat = 2;
const int ledWifi = 4;
const int ledGood = 16;
const int ledFail = 17;

// Variable to hold temperature readings
float temperature;
float pressure;

// Create a sensor object
Adafruit_BMP280 bmp;

void initBMP(){
  if (!bmp.begin(0x76))
  {
    Serial.println("Gagal Menemukan BMP280/BME280, Mohon Cek Sambungan Kabel I2C");
    while (1);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X1,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X1,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_OFF,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}


void setup() {
  Serial.begin(115200);  //Initialize serial
  initBMP();
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  pinMode(ledStat, OUTPUT);
  pinMode(ledWifi, OUTPUT);
  pinMode(ledGood, OUTPUT);
  pinMode(ledFail, OUTPUT);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED) {
      digitalWrite(ledWifi, LOW);
      Serial.print("Melakukan Penyambungan Internet...");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      }
      digitalWrite(ledWifi, HIGH);
      Serial.println("\nTersambung.");
    }
    // Get a new temperature reading
    digitalWrite(ledStat, HIGH);
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
    ThingSpeak.setField(3, pressure);
    
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      digitalWrite(ledGood, HIGH);
      digitalWrite(ledFail, LOW);
      Serial.println("Channel Berhasil Diperbaharui. Kode HTTP" + String(x));
    }
    else{
      digitalWrite(ledFail, HIGH);
      digitalWrite(ledGood, LOW);
      Serial.println("Terjadi Masalah Pembaruan Channel. Kode HTTP" + String(x));
    }
    lastTime = millis();
    digitalWrite(ledStat, LOW);
  }
}
