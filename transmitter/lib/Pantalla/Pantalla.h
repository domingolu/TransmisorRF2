/**
 * Pantalla.h
 *
 * Biblioteca "header-only" para manejar una pantalla OLED SSD1306 (128x64)
 * con funciones básicas:
 *   - Inicialización
 *   - Mostrar un mensaje centrado
 *   - Dibujar píxeles en las esquinas
 *
 * * Conexiones típicas (I2C):
 * - VCC  -> 5V (o 3.3V según tu módulo)
 * - GND  -> GND
 * - SCL  -> A5 (UNO/Nano) / pin SCL de tu placa
 * - SDA  -> A4 (UNO/Nano) / pin SDA de tu placa
 *
 * Uso típico:
 *   #include <Pantalla.h>
 *
 *   Pantalla pantalla;
 *
 *   void setup() {
 *     pantalla.iniciar();
 *     pantalla.mostrarMensaje("Hello Word");
 *     pantalla.dibujarEsquinas();
 *     pantalla.actualizar();
 *   }
 *
 *   void loop() {
 *     // nada necesario para mantener la imagen fija
 *   }
 */

#ifndef PANTALLA_H
#define PANTALLA_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Dimensiones de la pantalla (en píxeles)
#define PANTALLA_ANCHO  128
#define PANTALLA_ALTO   64

// Dirección I2C (ajustar si tu pantalla usa 0x3D)
#define PANTALLA_DIRECCION 0x3C

// Pin de reset (-1 si se comparte con el reset de Arduino)
#define PANTALLA_RESET -1

/**
 * Clase Pantalla
 *
 * Encapsula el manejo básico de una pantalla OLED SSD1306:
 * - Inicialización
 * - Mostrar texto centrado
 * - Dibujar píxeles en las esquinas
 * - Limpiar y actualizar la pantalla
 */
class Pantalla {
public:
  /**
   * Constructor.
   * Crea el objeto interno de Adafruit_SSD1306 con las dimensiones y pines definidos.
   */
  Pantalla()
    : _display(PANTALLA_ANCHO, PANTALLA_ALTO, &Wire, PANTALLA_RESET) {
    // El constructor solo crea el objeto; la inicialización real se hace en iniciar()
  }

  /**
   * Inicializa la pantalla OLED.
   * Debe llamarse en setup() antes de cualquier otra función.
   * Si falla la inicialización, entra en bucle infinito.
   */
  void iniciar() {
    if (!_display.begin(SSD1306_SWITCHCAPVCC, PANTALLA_DIRECCION)) {
      // Si no se detecta la pantalla, bloquear el programa
      for (;;);
    }

    // Limpiar para evitar el splash de Adafruit
    _display.clearDisplay();
    _display.display();
  }

  /**
   * Muestra un mensaje centrado horizontalmente en la pantalla.
   *
   * @param mensaje  Texto a mostrar (se usa print, no println).
   * @param tamanio  Tamaño del texto (1 = normal, 2 = doble, etc.). Por defecto 2.
   * @param y        Coordenada vertical en píxeles. Por defecto 20.
   */
  void mostrarMensaje(const String &mensaje, uint8_t tamanio = 2, int16_t y = 20) {
    _display.setTextSize(tamanio);
    _display.setTextColor(SSD1306_WHITE);

    // Calcular ancho del texto con el tamaño actual
    int16_t x1, y1;
    uint16_t anchoTexto, altoTexto;
    _display.getTextBounds(mensaje, 0, 0, &x1, &y1, &anchoTexto, &altoTexto);

    // Centrar horizontalmente
    int16_t x = (PANTALLA_ANCHO - (int16_t)anchoTexto) / 2;

    _display.setCursor(x, y);
    _display.print(mensaje);  // sin salto de línea
  }

  /**
   * Dibuja un píxel blanco en cada una de las 4 esquinas de la pantalla.
   * Útil para verificar orientación y límites de la pantalla.
   */
  void dibujarEsquinas() {
    // Esquina superior izquierda
    _display.drawPixel(0, 0, SSD1306_WHITE);
    // Esquina superior derecha
    _display.drawPixel(PANTALLA_ANCHO - 1, 0, SSD1306_WHITE);
    // Esquina inferior izquierda
    _display.drawPixel(0, PANTALLA_ALTO - 1, SSD1306_WHITE);
    // Esquina inferior derecha
    _display.drawPixel(PANTALLA_ANCHO - 1, PANTALLA_ALTO - 1, SSD1306_WHITE);
  }

  /**
   * Limpia toda la pantalla (la deja en negro).
   * Después de llamar a esta función, hay que volver a dibujar y llamar a actualizar().
   */
  void limpiar() {
    _display.clearDisplay();
  }

  /**
   * Envía el buffer gráfico a la pantalla para que se muestre.
   * Debe llamarse después de hacer dibujos o escribir texto.
   */
  void actualizar() {
    _display.display();
  }

private:
  Adafruit_SSD1306 _display;  // Objeto interno de la librería Adafruit
};

#endif // PANTALLA_H
