#ifndef ALERTAS_H
#define ALERTAS_H

#include <Arduino.h>

// Definición clara de estados de alerta
enum EstadoAlerta {
    ALERTA_VERDE,
    ALERTA_AMARILLO,
    ALERTA_NARANJA,
    ALERTA_ROJO,
    ALERTA_LLUVIA_AZUL
};

class Alertas {
private:
    uint8_t _pinR;
    uint8_t _pinG;
    uint8_t _pinB;
    uint8_t _pinBuzzer;
    EstadoAlerta _alertaAnterior;
    
    void setRGB(uint8_t r, uint8_t g, uint8_t b);

public:
    Alertas(uint8_t pinR, uint8_t pinG, uint8_t pinB, uint8_t pinBuzzer);
    
    bool testAlertas();
    void actualizarAlerta(EstadoAlerta estado);
};

#endif