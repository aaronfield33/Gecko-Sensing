// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <Losant.h>
#include <ArduinoJSON.h>

DHT dht1(D2, DHT22);
DHT dht2(D3, DHT22);

// Losant credentials.
const char* LOSANT_DEVICE_ID = "ESP-12";
const char* LOSANT_ACCESS_KEY = "c388c3b6-a8b3-411d-89f2-837c130083b7";
const char* LOSANT_ACCESS_SECRET = "581ea57169367808bcd1fcc353391424440f4b847ce6e0c05d20ba709a4190f0";

WiFiClientSecure wifiClient;

float t1 = 0.0;
float h1 = 0.0;
float t2 = 0.0;
float h2 = 0.0;

LosantDevice device(LOSANT_DEVICE_ID);

void connect() {

  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println("ASUS");

  // WiFi fix: https://github.com/esp8266/Arduino/issues/2186
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin("ASUS", "12345679");

  unsigned long wifiConnectStart = millis();

  while (WiFi.status() != WL_CONNECTED) {
    // Check to see if
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.println("Failed to connect to WIFI. Please verify credentials: ");
      Serial.println();
      Serial.print("SSID: ASUS ");
      Serial.print("Password: 12345679 ");
      Serial.println();
    }

    delay(500);
    Serial.println("...");
    // Only try for 5 seconds.
    if(millis() - wifiConnectStart > 5000) {
      Serial.println("Failed to connect to WiFi");
      Serial.println("Please attempt to send updated configuration parameters.");
      return;
    }
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.print("Authenticating Device...");
  HTTPClient http;
  http.begin("http://api.losant.com/auth/device");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");

  StaticJsonDocument<500> doc;
  doc["deviceId"] = LOSANT_DEVICE_ID;
  doc["key"] = LOSANT_ACCESS_KEY;
  doc["secret"] = LOSANT_ACCESS_SECRET;
  String buffer
  serializeJson(doc, buffer);

  int httpCode = http.POST(buffer);

  if(httpCode > 0) {
      if(httpCode == HTTP_CODE_OK) {
          Serial.println("This device is authorized!");
      } else {
        Serial.println("Failed to authorize device to Losant.");
        if(httpCode == 400) {
          Serial.println("Validation error: The device ID, access key, or access secret is not in the proper format.");
        } else if(httpCode == 401) {
          Serial.println("Invalid credentials to Losant: Please double-check the device ID, access key, and access secret.");
        } else {
           Serial.println("Unknown response from API");
        }
      }
    } else {
        Serial.println("Failed to connect to Losant API.");

   }

  http.end();

  // Connect to Losant.
  Serial.println();
  Serial.print("Connecting to Losant...");

  device.connectSecure(wifiClient, LOSANT_ACCESS_KEY, LOSANT_ACCESS_SECRET);

  while(!device.connected()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected!");
  Serial.println();
  Serial.println("This device is now ready for use!");
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);
  dht1.begin();
  dht2.begin();
}

void report(double t1, double h1, double t2, double h2) {
  StaticJsonDocument<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["t1"] = t1;
  root["h1"] = h1;
  root["t2"] = t2;
  root["h2"] = h2;
  device.sendState(root);
  Serial.println("Reported!");
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


  
