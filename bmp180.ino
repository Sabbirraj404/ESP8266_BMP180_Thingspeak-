#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <ESP8266HTTPClient.h>

Adafruit_BMP085 bmp;

const char* ssid = "********";
const char* password = "********";
const char* thingSpeakAddress = "api.thingspeak.com";
const String apiKey = "****************";

void setup() {
    Serial.begin(9600);
    delay(1000);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    if (!bmp.begin()) {
        Serial.println("Could not find BMP180 sensor, check wiring!");
        while (1) {}
    }
}

void loop() {
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure();
    float altitude = bmp.readAltitude();

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println(" Pa");

    Serial.print("Altitude: ");
    Serial.print(altitude);
    Serial.println(" meters");

    sendToThingSpeak(temperature, pressure, altitude);

    delay(10000); // Send data every 10 seconds
}

void sendToThingSpeak(float temperature, float pressure, float altitude) {
    WiFiClient client;
    HTTPClient http;

    // Construct the URL with the data and API key
    String url = "http://" + String(thingSpeakAddress) + "/update?api_key=" + apiKey +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(pressure) +
                 "&field3=" + String(altitude);

    Serial.print("Sending data to ThingSpeak: ");
    Serial.println(url);

    if (http.begin(client, url)) {
        int httpCode = http.GET();
        if (httpCode > 0) {
            if (httpCode == HTTP_CODE_OK) {
                Serial.println("Data sent successfully!");
            } else {
                Serial.print("HTTP error code: ");
                Serial.println(httpCode);
            }
        } else {
            Serial.println("Failed to connect to ThingSpeak");
        }
        http.end();
    } else {
        Serial.println("Failed to begin HTTP connection");
    }
}
