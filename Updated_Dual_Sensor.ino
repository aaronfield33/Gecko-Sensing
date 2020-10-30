// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

DHT dht1(D2, DHT22);
DHT dht2(D3, DHT22);

float t1 = 0.0;
float h1 = 0.0;
float t2 = 0.0;
float h2 = 0.0;

void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);
  dht1.begin();
  dht2.begin();
  
  // Connect to Wi-Fi
  WiFi.begin("ASUS", "12345679");
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
}
 
void loop(){
  float t1 = dht1.readTemperature(true);
  float h1 = dht1.readHumidity();
  float t2 = dht2.readTemperature(true);
  float h2 = dht2.readHumidity();

  Serial.print("DHT Sensor 1   ");
  Serial.print(t1); Serial.print(" F  ");
  Serial.print(h1); Serial.print("%RH");
  Serial.println();
  Serial.print("DHT Sensor 2   ");
  Serial.print(t2); Serial.print(" F  ");
  Serial.print(h2); Serial.print("%RH");  
  Serial.println();

delay(2000);
}


  
