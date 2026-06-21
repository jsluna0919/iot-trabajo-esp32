#ifndef REDBASE_H
#define REDBASE_H

// Esta es una interfaz abstracta. No se modifica si cambias de Wi-Fi a SIMCard
class RedBase {
public:
    virtual bool conectar() = 0;
    virtual bool enviarDatos(float temp, float hum, float nivel, bool lluvia) = 0;
};

#endif
