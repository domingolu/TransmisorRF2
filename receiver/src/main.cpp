/*
 * Receptor RF - Arduino Pro Mini 8Mhz 3.3v + nRF24L01+
 *
 * Recibe los 10 canales (1 byte cada uno) enviados por el transmisor
 * y los muestra por el puerto serie. El protocolo (estructura y
 * configuración de radio) está en la librería compartida ProtocoloRF.
 *
 * Pre-requisito: librería RF24.h by TMRh20 (declarada en platformio.ini).
 *
 * Conexión nRF24L01+ -> Arduino Pro Mini:
 *   nRF24L01+   Pro Mini
 *   -------------------------
 *   GND         GND
 *   VCC         3.3 V   (¡el módulo no tolera 5V!)
 *   CE          8
 *   CSN         10
 *   SCK/CLK     13
 *   MOSI        11
 *   MISO        12
 *
 * Importante:
 * - El transmisor y el receptor usan la misma dirección (RF_PIPE) y la
 *   misma estructura DatosRF, definidas en libraries/ProtocoloRF.
 * - Configurar el monitor serie a 115200 baudios.
 *
 * Formato de salida:
 *   "YAW: 128 | THROTTLE: 127 | ROLL: 127 | PITCH: 127 | ... | CH10: 0"
 */

#include <Arduino.h>
#include <ProtocoloRF.h>

// RF24(CE, CSN)
RF24 radio(8, 10);

// Estructura recibida (definida en ProtocoloRF.h; 10 canales de 0 a 255).
// El orden y los tipos deben coincidir con el transmisor.
DatosRF datos;

void setup() {
  // Inicializa el puerto serie.
  Serial.begin(115200);

  // Inicializa el módulo nRF24L01+.
  if (!radio.begin()) {
    Serial.println(F("Error: no se detecta el nRF24L01+"));
    while (true) {
      // Detiene el programa si el módulo no responde.
    }
  }

  // Desactiva los acuses de recibo automáticos.
  // Esta configuración debe coincidir con la del transmisor.
  radio.setAutoAck(RF_AUTO_ACK);

  // Velocidad de comunicación del módulo (debe coincidir con el transmisor).
  radio.setDataRate(RF_DATA_RATE);

  // Abre el canal de recepción número 1 con la dirección común.
  radio.openReadingPipe(1, RF_PIPE);

  // Coloca el módulo en modo receptor.
  radio.startListening();

  Serial.println(F("Receptor iniciado"));
  Serial.println(F("Esperando datos..."));
  Serial.println();
}

void loop() {
  /*
    Puede haber uno o varios paquetes pendientes en el búfer
    de recepción. El bucle procesa todos los paquetes disponibles.
   */
  while (radio.available()) {
    // Lee un paquete y lo copia en la estructura "datos".
    radio.read(&datos, sizeof(DatosRF));

    // Muestra los diez canales en una sola línea.
    // Los cuatro primeros son los ejes del cuadricóptero.
    Serial.print(F("YAW: "));
    Serial.print(datos.yaw);

    Serial.print(F(" | THROTTLE: "));
    Serial.print(datos.throttle);

    Serial.print(F(" | ROLL: "));
    Serial.print(datos.roll);

    Serial.print(F(" | PITCH: "));
    Serial.print(datos.pitch);

    Serial.print(F(" | CH5: "));
    Serial.print(datos.ch5);

    Serial.print(F(" | CH6: "));
    Serial.print(datos.ch6);

    Serial.print(F(" | CH7: "));
    Serial.print(datos.ch7);

    Serial.print(F(" | CH8: "));
    Serial.print(datos.ch8);

    Serial.print(F(" | CH9: "));
    Serial.print(datos.ch9);

    Serial.print(F(" | CH10: "));
    Serial.println(datos.ch10);

    // Pequeña pausa para facilitar la lectura del monitor serie.
    delay(20);
  }
}
