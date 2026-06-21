#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include "RedBase.h"
#include <Arduino.h>

class WifiClient : public RedBase {
private:
    const char* _ssid;
    const char* _password;
    const char* _apiUrl;

public:
    WifiClient(const char* ssid, const char* password, const char* apiUrl);
    bool conectar() override;
    bool enviarDatos(float temp, float hum, float nivel, bool lluvia) override;
};

#endif