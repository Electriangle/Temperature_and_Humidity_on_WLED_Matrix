/*
 * Title: Temperature_and_Humidity_on_WLED_Matrix
 * Author: Electriangle
 * Description: This sketch is a template for displaying temperature & humidity data from a DHT11/DHT22 sensor onto a WLED matrix panel.
 *
 *              NOTE: Enter values for your WiFi SSID, PASSWORD, WLED_SERVER, and sensor PIN # before uploading sketch.
 *
 * Author: Electriangle
 *      - Channel: https://www.youtube.com/@Electriangle
 *      - Video: https://www.youtube.com/watch?v=X95eUaPup1Q
 *
 * License: MIT License
 *      - Copyright (c) 2025 Electriangle
 *
 * Date Created: 1/24/25
 * Version: 1.0
*/

// ---------- INCLUDE LIBRARIES ----------
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>          // Temperature sensor library

// ---------- NETWORK SETTINGS ----------
const char* SSID = "SpectrumWiFi58";
const char* PASSWORD = "smartcoat031";
const char* WLED_SERVER = "http://192.168.86.115/json/state";

// ---------- PROGRAM PARAMETERS ----------
// Setup temperature sensor
#define DHT_PIN 2         // Enter Pin # for sensor
#define DHT_TYPE DHT11    // Declare sensor type
DHT dht(DHT_PIN, DHT_TYPE);

// Color transition thresholds
#define MAX_TEMP 90       // Color is fully Red above this value
#define MID_TEMP 75       // Color transitions from Blue to Red at this value
#define MIN_TEMP 60       // Color is fully Blue below this value

// Initialize color variables
int redValue = 255;
int blueValue = 255;
int greenValue = 255;

// ---------- PROGRAM FUNCTIONS ----------
void send_WLED_API_Command(String json_cmd) {
  // Confirm WiFi connection
  if (WiFi.status() == WL_CONNECTED) {

    // Initialize HTTP client
    WiFiClient client;
    HTTPClient http;
    http.begin(client, WLED_SERVER);
    http.addHeader("Content-Type", "application/json");
    
    // Send API command to server
    int httpResponseCode = http.POST(json_cmd);

    // Check response
    if (httpResponseCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.printf("POST request failed with code: %d\n", httpResponseCode);
    }

    // End HTTP client
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  dht.begin();

  // Connect to WiFi
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// ---------- LOOP ----------
void loop() {
  int temp = dht.readTemperature(true);   // For temp readings in Celcius, delete "true"
  int humidity = dht.readHumidity();

  int temp_color_value = constrain(temp, MIN_TEMP, MAX_TEMP);   // Constrain temp readings for map functions from 65°F to 90°F

  if (temp_color_value <= MID_TEMP) {   // Cold temperatures
    redValue = 0;
    blueValue = 255;
    greenValue = map(temp_color_value, MIN_TEMP, MID_TEMP, 128, 0);
  }
  else if (temp_color_value > MID_TEMP) {   // Hot temperatures
    redValue = 255;
    blueValue = 0;
    greenValue = map(temp_color_value, MID_TEMP, MAX_TEMP, 128, 0);
  }

  // Format color String
  String color_data = "[" + String(redValue) + "," + String(greenValue) + "," + String(blueValue) + "]";

  // WLED JSON API Message
  String json = R"(ENTER_JSON_CMD_HERE)";
  
  // Example
  // String json = R"( {"on":true,"bri":128,"transition":7,"mainseg":0,"seg":[{"id":0,"start":0,"stop":32,"startY":0,"stopY":32,"grp":1,"spc":0,"of":0,"on":true,"frz":false,"bri":255,"cct":127,"set":0,"n":" )" + String(temp) + R"( F","col":[ )" + color_data + R"( ,[0,0,0],[0,0,0]],"fx":122,"sx":128,"ix":100,"pal":0,"c1":0,"c2":128,"c3":16,"sel":true,"rev":false,"mi":false,"rY":false,"mY":false,"tp":false,"o1":false,"o2":false,"o3":false,"si":0,"m12":0},{"id":1,"start":0,"stop":32,"startY":0,"stopY":32,"grp":1,"spc":0,"of":0,"on":true,"frz":false,"bri":255,"cct":127,"set":0,"n":" )" + String(humidity) + R"( %","col":[[255,255,255],[0,0,0],[0,0,0]],"fx":122,"sx":128,"ix":170,"pal":0,"c1":0,"c2":128,"c3":16,"sel":true,"rev":false,"mi":false,"rY":false,"mY":false,"tp":false,"o1":false,"o2":true,"o3":false,"si":0,"m12":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0},{"stop":0}]} )";

  send_WLED_API_Command(json);

  delay(10000);   // Delay between each message sent
}
