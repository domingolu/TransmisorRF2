/*
 * Transmisor RF - Arduino Nano + nRF24L01+
 *
 * Transmite 10 canales (1 byte cada uno) al receptor. El protocolo
 * (estructura y configuración de radio) está en la librería
 * compartida ProtocoloRF, incluida también por el receptor.
 *
 * Conexión nRF24L01+ -> Arduino Nano:
 *     GND  -> GND
 *     VCC  -> 3.3V   (¡el módulo no tolera 5V!)
 *     CE   -> D9
 *     CSN  -> D10
 *     SCK  -> D13
 *     MOSI -> D11
 *     MISO -> D12
 *
 * Canales (ver docs/protocolo-nrf24l01.md):
 *   ch1   YAW      stick IZQ horizontal (VRX -> A0, 0-255, centro 127)
 *   ch2   THROTTLE stick IZQ vertical   (VRY -> A1, 0-255, centro 127)
 *   ch3   ROLL     stick DER horizontal (VRX -> A2, 0-255, centro 127)
 *   ch4   PITCH    stick DER vertical   (VRY -> A3, 0-255, centro 127)
 *   ch5   JOY IZQ BTN -> D0   (1 pulsado, 0 sin pulsar)
 *   ch6   JOY DER BTN -> D1   (1 pulsado, 0 sin pulsar)
 *   ch7   SW L1       -> D3   (1 activado, 0 inactivo)
 *   ch8   SW L2       -> D4   (1 activado, 0 inactivo)
 *   ch9   POT P1      -> A7   (0-255, mapeo directo)
 *   ch10  POT P2      -> A6   (0-255, mapeo directo)
 *
 * Tasa de transmisión: ~50 Hz (un paquete cada 20 ms).
 *
 * Nota: D0 y D1 son los pines RX/TX del UART; usarlos como entradas
 * puede interferir con el monitor serie por USB.
 */

#include <Arduino.h>
#include <ProtocoloRF.h>
#include <ezButton.h>
#include "Joystick.h"

#define CE_PIN 9
#define CSN_PIN 10

// Instancia del transceptor nRF24L01+
RF24 radio(CE_PIN, CSN_PIN);

// Joysticks: VRX, VRY y botón (SW). El botón usa pull-up
// interno (ezButton lo configura en el constructor).
Joystick joystickLeft(A0, A1, 0);
Joystick joystickRight(A2, A3, 1);

// Switches L1 y L2 (tipo toggle: activado en esa posición).
// ezButton configura INPUT_PULLUP y el debounce en el
// constructor; setDebounceTime() lo fija explícito.
ezButton switchL1(3);
ezButton switchL2(4);

// Datos a transmitir (estructura definida en ProtocoloRF.h)
DatosRF datos;

void printData(const DatosRF &d) {
  Serial.print(F("yaw="));
  Serial.print(d.yaw);
  Serial.print(F(", throttle="));
  Serial.print(d.throttle);
  Serial.print(F(", roll="));
  Serial.print(d.roll);
  Serial.print(F(", pitch="));
  Serial.print(d.pitch);
  Serial.print(F(", ch5="));
  Serial.print(d.ch5);
  Serial.print(F(", ch6="));
  Serial.print(d.ch6);
  Serial.print(F(", ch7="));
  Serial.print(d.ch7);
  Serial.print(F(", ch8="));
  Serial.print(d.ch8);
  Serial.print(F(", ch9="));
  Serial.print(d.ch9);
  Serial.print(F(", ch10="));
  Serial.println(d.ch10);  // println para terminar la línea
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // algunas placas necesitan esperar para acceder al serie por USB
  }

  // Inicializa los joysticks (debounce del botón: 50 ms)
  joystickLeft.begin();
  joystickRight.begin();

  // Debounce de los switches (50 ms, igual que los
  // botones de los joysticks)
  switchL1.setDebounceTime(50);
  switchL2.setDebounceTime(50);

  // Inicializa el transceptor en el bus SPI
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // bucle infinito
  }
  radio.setAutoAck(RF_AUTO_ACK);
  // PA bajo para evitar problemas de alimentación en proximidad
  radio.setPALevel(RF_PA_LEVEL);
  // Payload de solo los bytes que necesitamos (10 canales)
  radio.setPayloadSize(sizeof(DatosRF));
  radio.setDataRate(RF_DATA_RATE);
  radio.openWritingPipe(RF_PIPE);

  // Valores neutros iniciales de todos los canales
  inicializarDatosRF(datos);

  Serial.println(F("Iniciando..."));
}

/**************************************************/

void loop() {

  // Actualizar el estado de los joysticks (debounce del botón
  // y lectura analógica) y de los switches (debounce);
  // hay que llamarlos cada ciclo
  joystickLeft.loop();
  joystickRight.loop();
  switchL1.loop();
  switchL2.loop();

  /* Si un eje está invertido, intercambia 0 y 255:
     Normal:    datos.yaw = map(joystickLeft.readX(), 0, 1024, 0, 255);
     Invertido: datos.yaw = map(joystickLeft.readX(), 0, 1024, 255, 0); */

  // Ejes del cuadricóptero: "arriba" en el stick -> valor mayor
  datos.yaw = map(joystickLeft.readX(), 0, 1024, 0, 255);  // stick IZQ horizontal
  datos.throttle = map(joystickLeft.readY(), 0, 1024, 0, 255);  // stick IZQ vertical
  datos.roll = map(joystickRight.readX(), 0, 1024, 0, 255);  // stick DER horizontal
  datos.pitch = map(joystickRight.readY(), 0, 1024, 0, 255);  // stick DER vertical

  // Botones: con pull-up interno getButtonState() da 0 al
  // pulsar; se invierte para que pulsado = 1 y sin pulsar = 0
  datos.ch5 = !joystickLeft.getButtonState();  // Botón JOY IZQ
  datos.ch6 = !joystickRight.getButtonState();  // Botón JOY DER

  // Switches: misma inversión que los botones (con pull-up
  // interno, getState() da 0 en la posición activada).
  // Son tipo toggle: ch7/ch8 quedan en 1 mientras el
  // switch esté en esa posición.
  datos.ch7 = !switchL1.getState();  // SW L1 (D3)
  datos.ch8 = !switchL2.getState();  // SW L2 (D4)

  // Potenciómetros: P1 -> A7 (ch9), P2 -> A6 (ch10).
  // Mapeo directo (0 = un extremo, 255 = el otro) con
  // tope 1023 para usar el rango completo del ADC.
  datos.ch9 = map(analogRead(A7), 0, 1023, 0, 255);  // POT P1 (A7)
  datos.ch10 = map(analogRead(A6), 0, 1023, 0, 255);  // POT P2 (A6)
  
  bool report = radio.write(&datos, sizeof(DatosRF));

  if (report) {
    Serial.print(F("se transmite "));  // payload entregado
    printData(datos);
  } else {
    Serial.println(F("Transmission failed or timed out"));  // no entregado
  }

  // ~50 Hz: un paquete cada 20 ms
  delay(20);
}
