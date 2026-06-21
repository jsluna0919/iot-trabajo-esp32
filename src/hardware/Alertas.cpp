#include "Alertas.h"

Alertas::Alertas(uint8_t pinR, uint8_t pinG, uint8_t pinB, uint8_t pinBuzzer) {
    _pinR = pinR;
    _pinG = pinG;
    _pinB = pinB;
    _pinBuzzer = pinBuzzer;
    
    pinMode(_pinR, OUTPUT);
    pinMode(_pinG, OUTPUT);
    pinMode(_pinB, OUTPUT);
    pinMode(_pinBuzzer, OUTPUT);
    _alertaAnterior = ALERTA_VERDE; // Inicializa en verde
}

void Alertas::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    // Si usas un LED de cátodo común (enciende con HIGH):
    digitalWrite(_pinR, r);
    digitalWrite(_pinG, g);
    digitalWrite(_pinB, b);
}

bool Alertas::testAlertas() {
    // Prueba rápida de colores y pitido al encender
    setRGB(1, 0, 0); delay(200); // Rojo
    setRGB(0, 1, 0); delay(200); // Verde
    setRGB(0, 0, 1); delay(200); // Azul
    setRGB(0, 0, 0);
    
    digitalWrite(_pinBuzzer, HIGH); delay(100);
    digitalWrite(_pinBuzzer, LOW);
    return true;
}

void Alertas::actualizarAlerta(EstadoAlerta estado) {
    // 1. Controlar el color del LED RGB de forma constante
    switch(estado) {
        case ALERTA_VERDE:        setRGB(0, 1, 0); break; // Verde
        case ALERTA_AMARILLO:     setRGB(1, 1, 0); break; // Amarillo
        case ALERTA_NARANJA:      setRGB(1, 1, 0); break; // Naranja (Ajustar según hardware)
        case ALERTA_ROJO:         setRGB(1, 0, 0); break; // Rojo
        case ALERTA_LLUVIA_AZUL:  setRGB(0, 0, 1); break; // Azul
    }

    // 2. Controlar el Buzzer (Solo pita UNA VEZ al cambiar de estado)
    if (estado != _alertaAnterior) {
        
        // Si pasa a estado VERDE, no emite ningún sonido, solo se silencia
        if (estado == ALERTA_VERDE) {
            digitalWrite(_pinBuzzer, LOW);
        } 
        // Si entra a cualquier estado de peligro, emite un único pitido de atención
        else {
            Serial.println("[ALERTAS] Cambio de estado detectado. Activando buzzer de aviso.");
            digitalWrite(_pinBuzzer, HIGH);
            delay(400); // Pitido corto de 400 milisegundos
            digitalWrite(_pinBuzzer, LOW);
        }

        // Guardamos el estado actual para que no vuelva a pitar en el próximo loop
        _alertaAnterior = estado;
    }
}