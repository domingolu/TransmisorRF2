/*
 * RF24.h - Stub de la librería RF24 para simular en Wokwi
 *
 * Wokwi no tiene una parte oficial para el nRF24L01+, así que el enlace
 * RF no se puede simular. Este stub implementa únicamente la parte de la
 * API que usa el firmware del transmisor, para que el firmware real
 * (src/main.cpp) se pueda ejecutar en el simulador sin cambios y así
 * comprobar la lectura de los mandos y el mapeo de los 10 canales.
 *
 * Lo que NO hace (a propósito):
 *   - No usa SPI: no hay radio real detrás.
 *   - No guarda ni entrega paquetes: write() solo informa por el monitor
 *     serie y enciende el LED de actividad.
 *
 * Solo se usa en el entorno [env:wokwi] (ver platformio.ini).
 *
 * API compatible con la librería RF24 de TMRh20 (nrf24/RF24).
 */

#ifndef RF24_H
#define RF24_H

#include <Arduino.h>
#include <stdint.h>

// Fallback por si el pin de la placa no define LED_BUILTIN
#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

// Niveles de potencia (mismos valores que la librería real)
enum rf24_pa_dbm_e {
  RF24_PA_MIN = 0,
  RF24_PA_LOW,
  RF24_PA_HIGH,
  RF24_PA_MAX,
  RF24_PA_ERROR
};

// Velocidades de datos (mismos valores que la librería real)
enum rf24_datarate_e {
  RF24_1MBPS = 0,
  RF24_2MBPS,
  RF24_250KBPS
};

class RF24 {
 public:
  RF24(uint8_t cePin, uint8_t csnPin)
      : _cePin(cePin), _csnPin(csnPin), _payloadSize(0), _ledOn(false) {}

  // Siempre true: sin radio que comprobar.
  bool begin(void) {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println(F("RF24 stub: nRF24L01+ NO simulado (enlace RF ficticio)"));
    return true;
  }

  void setAutoAck(bool enable) {}
  void setPALevel(uint8_t level, bool lnaEnable = true) {}
  bool setDataRate(rf24_datarate_e speed) { return true; }
  void setPayloadSize(uint8_t size) { _payloadSize = size; }
  void openWritingPipe(uint64_t address) {}
  void openReadingPipe(uint8_t number, uint64_t address) {}
  void startListening(void) {}
  void stopListening(void) {}

  // Sin radio no hay paquetes: siempre false / 0 bytes leídos.
  bool available(void) { return false; }
  bool read(void* data, uint8_t len) { return false; }

  // Simula el envío: vuelca el payload por el monitor serie y hace
  // parpadear el LED de actividad del Nano (D13).
  bool write(const void* buf, uint8_t len) {
    const uint8_t* p = (const uint8_t*)buf;
    Serial.print(F("payload["));
    Serial.print(len);
    Serial.print(F("]: "));
    for (uint8_t i = 0; i < len; i++) {
      if (p[i] < 0x10) Serial.print('0');
      Serial.print(p[i], HEX);
      Serial.print(' ');
    }
    Serial.println();

    _ledOn = !_ledOn;
    digitalWrite(LED_BUILTIN, _ledOn ? HIGH : LOW);
    return true;
  }

 private:
  uint8_t _cePin;
  uint8_t _csnPin;
  uint8_t _payloadSize;
  bool _ledOn;
};

#endif  // RF24_H
