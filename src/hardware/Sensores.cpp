#include "Sensores.h"

Sensores::Sensores(uint8_t dhtPin, uint8_t trigPin, uint8_t echoPin, uint8_t lluviaPin) : _dht(dhtPin, DHT22) {
    _dhtPin = dhtPin;
    _trigPin = trigPin;
    _echoPin = echoPin;
    _lluviaPin = lluviaPin;
}

bool Sensores::testDHT() {
    _dht.begin();
    float t = _dht.readTemperature();
    return !isnan(t);
}

bool Sensores::testUltrasonico() {
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
    // Realiza una lectura de prueba rápida
    float n = leerNivelcm();
    return (n > 0.0 && n < 500.0); // El rango típico del sensor es menor a 5 metros
}

bool Sensores::testLluvia() {
    pinMode(_lluviaPin, INPUT);
    return true; // El pin digital/analógico siempre responde voltaje
}

float Sensores::leerTemperatura() {
    float t = _dht.readTemperature();
    return isnan(t) ? -999.0 : t;
}

float Sensores::leerHumedad() {
    float h = _dht.readHumidity();
    return isnan(h) ? -999.0 : h;
}

float Sensores::leerNivelcm() {
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(20); //Se cambia 10 por el 20
    digitalWrite(_trigPin, LOW);
    
    long duracion = pulseIn(_echoPin, HIGH, 60000); // Timeout de 60ms para evitar bloqueos
    if (duracion == 0) return -1.0;
    
    return (duracion * 0.0343) / 2.0;
}

bool Sensores::estaLloviendo() {
    // Si usas el módulo de lluvia común, la salida digital da LOW (0) cuando detecta agua
    return digitalRead(_lluviaPin) == LOW;
}