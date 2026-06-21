#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>
#include <DHT.h>

class Sensores {
private:
    uint8_t _dhtPin;
    uint8_t _trigPin;
    uint8_t _echoPin;
    uint8_t _lluviaPin;
    DHT _dht;

public:
    Sensores(uint8_t dhtPin, uint8_t trigPin, uint8_t echoPin, uint8_t lluviaPin);
    
    // Métodos de autodiagnóstico para el Setup
    bool testDHT();
    bool testUltrasonico();
    bool testLluvia();

    // Métodos de lectura
    float leerTemperatura();
    float leerHumedad();
    float leerNivelcm();
    bool estaLloviendo();
};

#endif
