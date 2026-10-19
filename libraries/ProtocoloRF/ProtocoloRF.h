/**
 * ProtocoloRF.h
 *
 * Protocolo de comunicación común entre el transmisor y el receptor RF.
 *
 * Esta librería es la fuente de verdad del protocolo: la estructura de datos
 * y la configuración de radio se definen aquí una sola vez y ambos proyectos
 * (transmitter/ y receiver/) la incluyen vía lib_extra_dirs = ../libraries.
 *
 * Reglas:
 *   - La estructura DatosRF y la dirección RF_PIPE deben coincidir
 *     exactamente en el transmisor y el receptor.
 *   - No cambiar el orden ni los tipos de la estructura sin actualizar
 *     ambos firmwares a la vez.
 *
 * Documentación detallada: docs/protocolo-nrf24l01.md
 */

#ifndef PROTOCOLO_RF_H
#define PROTOCOLO_RF_H

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// ============================================================================
// Configuración de radio (DEBE ser idéntica en transmisor y receptor)
// ============================================================================

// Dirección del pipe de comunicación (64 bits, little-endian)
#define RF_PIPE 0xE8E8F0F0E1LL

// Velocidad de datos: 250 kbps (la más lenta y robusta del nRF24L01+)
#define RF_DATA_RATE RF24_250KBPS

// Nivel de potencia del PA (bajo, para pruebas en proximidad).
// Solo afecta al transmisor.
#define RF_PA_LEVEL RF24_PA_LOW

// Acuses de recibo automáticos desactivados
#define RF_AUTO_ACK false

// ============================================================================
// Estructura de datos (DEBE coincidir exactamente en transmisor y receptor)
// ============================================================================

// Número de canales transmitidos
#define RF_NUM_CANALES 10

// Un byte por canal (0-255). ¡No cambiar el orden ni los tipos!
// Los cuatro primeros canales son los ejes del cuadricóptero:
//   yaw = stick izquierdo horizontal, throttle = izquierdo vertical
//   roll = stick derecho horizontal, pitch = derecho vertical
struct DatosRF {
  byte yaw;      // ch1: stick izq. horizontal (A0)
  byte throttle; // ch2: stick izq. vertical (A1)
  byte roll;     // ch3: stick der. horizontal (A2)
  byte pitch;    // ch4: stick der. vertical (A3)
  byte ch5;      // botón joystick izquierdo (D0)
  byte ch6;      // botón joystick derecho (D1)
  byte ch7;      // switch L1 (D3)
  byte ch8;      // switch L2 (D4)
  byte ch9;      // potenciómetro P1 (A7)
  byte ch10;     // potenciómetro P2 (A6)
};

/**
 * Inicializa los canales con valores neutros:
 * analógicos al centro (127), digitales en 0 y throttle en 0
 * (motores detenidos al arrancar: es lo más seguro).
 * Llamar en setup() antes de transmitir.
 */
inline void inicializarDatosRF(DatosRF &d) {
  d.yaw = 127;
  d.throttle = 0;
  d.roll = 127;
  d.pitch = 127;
  d.ch5 = 0;
  d.ch6 = 0;
  d.ch7 = 0;
  d.ch8 = 0;
  d.ch9 = 0;
  d.ch10 = 0;
}

#endif // PROTOCOLO_RF_H
