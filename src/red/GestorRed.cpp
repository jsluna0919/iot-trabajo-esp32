#include "GestorRed.h"
#include <WiFi.h>
#include <WebServer.h>

// Declaramos el servidor web interno en el puerto 80
WebServer servidor(80);

GestorRed::GestorRed() {
    _nivelBase = 0.0;
    _modoConfig = false;
}

void GestorRed::inicializar() {
    _prefs.begin("config_estacion", false);
    
    // Leer datos guardados (si no existen, devuelve vacío o 0.0)
    _ssid = _prefs.getString("wifi_ssid", "");
    _pass = _prefs.getString("wifi_pass", "");
    _nivelBase = _prefs.getFloat("nivel_base", 0.0);

    // Si no hay Wi-Fi guardado o el nivel base es 0, entramos en Modo Configuración
    if (_ssid == "" || _nivelBase == 0.0) {
        _modoConfig = true;
        iniciarPortalConfig();
    } else {
        Serial.println("[SYSTEM] Datos cargados de la memoria. Iniciando en modo Normal.");
        Serial.printf("-> Wi-Fi: %s | Nivel Base: %.2f cm\n", _ssid.c_str(), _nivelBase);
        _prefs.end();
    }
}

void GestorRed::iniciarPortalConfig() {
    Serial.println("[SYSTEM] Entrando en MODO CONFIGURACIÓN...");
    
    // 1. Crear la red Wi-Fi propia del ESP32
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32_S3_QUEBRADA", "12345678"); // Nombre de red y clave de acceso
    
    Serial.println("-> Red Wi-Fi creada: ESP32_S3_QUEBRADA");
    Serial.print("-> Conéctate con tu celular e ingresa a: ");
    Serial.println(WiFi.softAPIP()); // Normalmente es 192.168.4.1

    // 2. Definir la página web que verá el celular
    servidor.on("/", [this]() {
        String html = "<html><head><meta charset='UTF-8'><title>Configuración Estación</title>";
        html += "<style>body{font-family:Arial; margin:40px;} input{display:block; margin:10px 0; padding:10px; width:100%;}</style></head><body>";
        html += "<h2>Configuración de la Estación Meteorológica</h2>";
        html += "<form action='/guardar' method='POST'>";
        html += "Nombre del Wi-Fi (SSID):<input type='text' name='ssid' required>";
        html += "Contraseña Wi-Fi:<input type='password' name='pass' required>";
        html += "Nivel Base de Referencia del Río (cm):<input type='number' step='0.1' name='nivel' required>";
        html += "<input type='submit' value='Guardar Configuración' style='background:green; color:white;'>";
        html += "</form></body></html>";
        
        servidor.send(200, "text/html", html);
    });

    // 3. Procesar los datos cuando el usuario le dé al botón "Guardar"
    servidor.on("/guardar", [this]() {
        if (servidor.hasArg("ssid") && servidor.hasArg("pass") && servidor.hasArg("nivel")) {
            _ssid = servidor.arg("ssid");
            _pass = servidor.arg("pass");
            _nivelBase = servidor.arg("nivel").toFloat();

            // Guardar permanentemente en la memoria Flash (Preferences)
            _prefs.begin("config_estacion", false);
            _prefs.putString("wifi_ssid", _ssid);
            _prefs.putString("wifi_pass", _pass);
            _prefs.putFloat("nivel_base", _nivelBase);
            _prefs.end();

            String htmlExito = "<html><body><h2>¡Configuración Guardada!</h2><p>El ESP32 se reiniciará en 3 segundos en modo normal...</p></body></html>";
            servidor.send(200, "text/html", htmlExito);
            
            delay(3000);
            ESP.restart(); // Reinicia el chip físicamente
        }
    });

    servidor.begin();
}

void GestorRed::manejarPortal() {
    if (_modoConfig) {
        servidor.handleClient(); // Atiende las peticiones web del celular
    }
}