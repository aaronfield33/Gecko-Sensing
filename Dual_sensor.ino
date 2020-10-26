/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide/
*********/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "ASUS";
const char* password = "12345679";

#define DHTPIN 5     // Digital pin connected to the DHT sensor
#define DHTPIN2 4    // Digital pin connected to the second DHT sensor

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

// current temperature & humidity, updated in loop()
float t1 = 0.0;
float h1 = 0.0;
float t2 = 0.0;
float h2 = 0.0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 2 seconds
const long interval = 2000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h { font-size: 5.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h>ESP8266 DHT Server</h>
    <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i>
    <span class="dht-labels">Temperature Tank 1</span> 
    <span id="temperature">%TEMPERATURE1%</span>
    <sup class="units">&deg;F</sup>
    </p>
    <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature Tank 2</span> 
    <span id="temperature">%TEMPERATURE2%</span>
    <sup class="units">&deg;F</sup>
    <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity Tank 1</span>
    <span id="humidity">%HUMIDITY1%</span>
    <sup class="units">%</sup>
    </p>
    <p>
//    <i class="fas fa-tint" style="color:#00add6;"></i> 
//    <span class="dht-labels">Humidity Tank 2</span>
//    <span id="humidity">%HUMIDITY2%</span>
//    <sup class="units">%</sup>
    </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature1").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature1", true);
  xhttp.send();
}, 2000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature2").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature2", true);
  xhttp.send();
}, 2000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity1").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity1", true);
  xhttp.send();
}, 2000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity2").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity2", true);
  xhttp.send();
}, 2000 ) ;

</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE1"){
    return String(t1);
  }
  else if(var == "TEMPERATURE2"){
    return String(t2);
  }
  else if(var == "HUMIDITY1"){
    return String(h1);
  }
  else if(var == "HUMIDITY2"){
    return String(h2);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t1).c_str());
  });
  server.on("/temperature2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t2).c_str());
  });
  server.on("/humidity1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h1).c_str());
  });
  server.on("/humidity2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h2).c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    //float newT1 = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float newT1 = dht.readTemperature(true);
    // if temperature read failed, don't change t1 value
    if (isnan(newT1)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t1 = newT1;
      Serial.println(t1);
    }
    //Find second sensor temp
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float newT2 = dht2.readTemperature(true);
    // if temperature read failed, don't change t1 value
    if (isnan(newT2)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t2 = newT2;
      Serial.println(t2);
    }
    // Read Humidity
    float newH1 = dht.readHumidity();
    // if humidity read failed, don't change h1value 
    if (isnan(newH1)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h1= newH1;
      Serial.println(h1);
    // Read Humidity
    float newH2 = dht2.readHumidity();
    // if humidity read failed, don't change h1value 
    if (isnan(newH2)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h2= newH2;
      Serial.println(h2);
    }
    }
  }
}
