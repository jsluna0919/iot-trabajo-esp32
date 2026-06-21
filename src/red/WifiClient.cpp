#include "WifiClient.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WifiClient::WifiClient(const char* ssid, const char* password, const char* apiUrl) {
    _ssid = ssid;
    _password = password;
    _apiUrl = apiUrl;
}

bool WifiClient::conectar() {
    if (WiFi.status() == WL_CONNECTED) return true;
    
    WiFi.begin(_ssid, _password);
    unsigned long startAttemptTime = millis();
    
    // Intenta conectar máximo por 10 segundos para no congelar el ESP32
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(500);
    }
    return (WiFi.status() == WL_CONNECTED);
}

bool WifiClient::enviarDatos(float temp, float hum, float nivel, bool lluvia) { //Pendiente organizar el json como se requiere para la base de datos...
    if (!conectar()) return false;
    
    HTTPClient http;
    http.begin(_apiUrl);
    http.addHeader("Content-Type", "application/json");

    JsonDocument doc;
    doc["temperatura"] = temp;
    doc["humedad"] = hum;
    doc["nivel_quebrada"] = nivel;
    doc["lloviendo"] = lluvia;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpCode = http.POST(jsonString);
    http.end();
    
    return (httpCode > 0);
}
