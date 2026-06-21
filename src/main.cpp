#include <Arduino.h>
#include "hardware/Sensores.h"
#include "hardware/Alertas.h"
#include "red/WifiClient.h"

// --- Configuración de Pines (Ajusta según tu conexionado físico en el S3) ---
#define PIN_TRIG       4  //A0
#define PIN_ECHO       5  //A1
#define PIN_DHT        6  //A2
#define PIN_LLUVIA     7

#define PIN_LED_R      13 //D11
#define PIN_LED_G      12 //D12
#define PIN_LED_B      14 //D10
#define PIN_BUZZER     18 //D6

// --- Instanciación de Capas ---
Sensores misSensores(PIN_DHT, PIN_TRIG, PIN_ECHO, PIN_LLUVIA);
Alertas misAlertas(PIN_LED_R, PIN_LED_G, PIN_LED_B, PIN_BUZZER);
WifiClient redEstacion("Mi_Wifi_SSID", "Mi_Clave_Password", "https://iot-trabajo.onrender.com/mediciones");

// Variable global para almacenar el nivel inicial de referencia (Línea base sin crecida)
float nivelBaseReferencia = 0.0; 
unsigned long ultimoEnvioAPI = 0;

void setup() {
    Serial.begin(115200);
    delay(2000); // Espera de cortesía para estabilizar el puerto serie del S3
    
    Serial.println("\n=== INICIANDO SISTEMA DE MONITOREO DE QUEBRADA ===");
    
    // 1. Test de Alertas (LED y Buzzer)
    if (misAlertas.testAlertas()) {
        Serial.println("[OK] Sistema de alertas visuales y sonoras funcionando.");
    }

    // 2. Test del DHT22
    if (misSensores.testDHT()) {
        Serial.println("[OK] Sensor temperatura y humedad funcionando.");
    } else {
        Serial.println("[ERROR] Falló el sensor DHT22.");
    }

    // 3. Test del Ultrasónico
    if (misSensores.testUltrasonico()) {
        Serial.println("[OK] Sensor ultrasónico funcionando.");
        // Tomamos la primera lectura como el nivel base normal de la quebrada
        nivelBaseReferencia = misSensores.leerNivelcm();
        Serial.printf("-> Nivel base de referencia establecido en: %.2f cm\n", nivelBaseReferencia);
    } else {
        Serial.println("[ERROR] Falló el sensor ultrasónico.");
    }

    // 4. Test del Sensor de Lluvia
    if (misSensores.testLluvia()) {
        Serial.println("[OK] Sensor de lluvia funcionando.");
    }

    // 5. Test de Conectividad de Red
    Serial.println("Intentando conectar a la red...");
    if (redEstacion.conectar()) {
        Serial.println("[OK] Wi-Fi conectado exitosamente.");
    } else {
        Serial.println("[ADVERTENCIA] No se pudo conectar al Wi-Fi. El sistema operará de forma local transitoriamente.");
    }
    
    Serial.println("=== DIAGNÓSTICO FINALIZADO. INICIANDO CICLO PRINCIPAL ===\n");
}

void loop() {
    // 1. Lectura constante de las variables físicas
    float temp = misSensores.leerTemperatura();
    float hum = misSensores.leerHumedad();
    float nivelActual = misSensores.leerNivelcm();
    bool lloviendo = misSensores.estaLloviendo();

    // 2. Calcular cuánto ha subido el agua respecto a la referencia
    // (A menor distancia medida por el ultrasónico, más alto está el nivel del agua)
    float deltaSubida = nivelBaseReferencia - nivelActual;

    // 3. Determinar el estado de la alerta según tus reglas de negocio
    EstadoAlerta alertaActual = ALERTA_VERDE;

    // La crecida del río tiene prioridad absoluta sobre la lluvia local
    if (deltaSubida >= 60.0) {
        alertaActual = ALERTA_ROJO;
    } else if (deltaSubida >= 40.0) {
        alertaActual = ALERTA_NARANJA;
    } else if (deltaSubida >= 20.0) {
        alertaActual = ALERTA_AMARILLO;
    } else if (lloviendo) {
        // Solo se muestra azul si el nivel está en rangos normales pero llueve
        alertaActual = ALERTA_LLUVIA_AZUL;
    } else {
        alertaActual = ALERTA_VERDE;
    }

    // 4. Actualizar actuadores (Hará parpadear el LED/Buzzer sin congelar el flujo)
    misAlertas.actualizarAlerta(alertaActual);

    // 5. Envío programado de datos a la API cada minuto (sin usar delay para no romper los pitidos)
    if (millis() - ultimoEnvioAPI >= 60000) {
        ultimoEnvioAPI = millis();
        
        Serial.printf("[API] Despachando -> Temp: %.1f°C | Hum: %.1f%% | Subió: %.1fcm | Lluvia: %s\n", 
                      temp, hum, deltaSubida, lloviendo ? "SI" : "NO");
                      
        bool exito = redEstacion.enviarDatos(temp, hum, deltaSubida, lloviendo);
        if (!exito) {
            Serial.println("[API] Error al transmitir el reporte.");
        }
    }
}
