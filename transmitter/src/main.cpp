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
 *   ch1   JOY IZQ X   -> A0
 *   ch2   JOY IZQ Y   -> A1
 *   ch3   JOY DER X   -> A2
 *   ch4   JOY DER Y   -> A3
 *   ch5   JOY IZQ BTN -> D0
 *   ch6   JOY DER BTN -> D1
 *   ch7   libre       -> A4
 *   ch8-ch10 reservados (switches / potenciómetros, pendientes de cablear)
 *
 * Nota: D0 y D1 son los pines RX/TX del UART; usarlos como entradas
 * puede interferir con el monitor serie por USB.
 */

#include <Arduino.h>
#include <ProtocoloRF.h>

#define CE_PIN 9
#define CSN_PIN 10

// Instancia del transceptor nRF24L01+
RF24 radio(CE_PIN, CSN_PIN);

// Datos a transmitir (estructura definida en ProtocoloRF.h)
DatosRF datos;

void printData(const DatosRF &d) {
  Serial.print(F("ch1="));
  Serial.print(d.ch1);
  Serial.print(F(", ch2="));
  Serial.print(d.ch2);
  Serial.print(F(", ch3="));
  Serial.print(d.ch3);
  Serial.print(F(", ch4="));
  Serial.print(d.ch4);
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
  pinMode(0, INPUT_PULLUP);  // Botón joystick izquierdo
  pinMode(1, INPUT_PULLUP);  // Botón joystick derecho

  Serial.begin(115200);
  while (!Serial) {
    // algunas placas necesitan esperar para acceder al serie por USB
  }

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

  /* Si un canal está invertido, intercambia 0 y 255:
     Normal:    datos.ch1 = map(analogRead(A0), 0, 1024, 0, 255);
     Invertido: datos.ch1 = map(analogRead(A0), 0, 1024, 255, 0); */

  datos.ch1 = map(analogRead(A0), 0, 1024, 0, 255);  // JOY IZQ X
  datos.ch2 = map(analogRead(A1), 0, 1024, 0, 255);  // JOY IZQ Y
  datos.ch3 = map(analogRead(A2), 0, 1024, 0, 255);  // JOY DER X
  datos.ch4 = map(analogRead(A3), 0, 1024, 0, 255);  // JOY DER Y
  datos.ch5 = digitalRead(0);  // Botón JOY IZQ
  datos.ch6 = digitalRead(1);  // Botón JOY DER
  datos.ch7 = map(analogRead(A4), 0, 1024, 0, 255);  // Libre (A4)
  // ch8-ch10: reservados, mantienen el valor inicial (0)

  bool report = radio.write(&datos, sizeof(DatosRF));

  if (report) {
    Serial.print(F("se transmite "));  // payload entregado
    printData(datos);
  } else {
    Serial.println(F("Transmission failed or timed out"));  // no entregado
  }

  // Pausa para que la salida por serie sea legible
  delay(1000);
}
