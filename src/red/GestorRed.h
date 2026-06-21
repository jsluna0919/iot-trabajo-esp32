#ifndef GESTORRED_H
#define GESTORRED_H

#include <Arduino.h>
#include <Preferences.h>

class GestorRed {
private:
    Preferences _prefs;
    String _ssid;
    String _pass;
    float _nivelBase;
    bool _modoConfig;

    void iniciarPortalConfig();

public:
    GestorRed();
    void inicializar();
    bool esModoConfig() const { return _modoConfig; }
    
    // Métodos para obtener los datos guardados
    String getSSID() { return _ssid; }
    String getPass() { return _pass; }
    float getNivelBase() { return _nivelBase; }
    
    // Procesar la web del portal
    void manejarPortal();
};

#endif
