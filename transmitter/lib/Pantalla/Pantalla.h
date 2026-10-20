/**
 * Pantalla.h
 *
 * Biblioteca "header-only" para manejar una pantalla OLED SSD1306 (128x64)
 * con funciones básicas:
 *   - Inicialización (sin bloquear si la pantalla no está conectada)
 *   - Mostrar un mensaje centrado
 *   - Dibujar píxeles en las esquinas
 *   - Escribir texto en una posición concreta
 *   - Refresco por páginas (solo las que cambian)
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
 *     if (pantalla.iniciar()) {
 *       pantalla.limpiar();
 *       pantalla.texto("Hola", 0, 0);
 *       pantalla.refrescarTodo();
 *     }
 *   }
 *
 * Notas:
 * - El buffer del SSD1306 (1024 bytes en 128x64) se reserva con malloc(),
 *   así que NO aparece en el reporte de RAM de PlatformIO (solo cuenta
 *   .data y .bss). El RAM real = estáticos + buffer + stack.
 * - Cada fila de texto de 8 px ocupa exactamente una página del OLED
 *   (el buffer se indexa como buffer[x + (y/8)*WIDTH]), por eso el
 *   refresco por páginas puede hacerse fila a fila.
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

// Altura de una página del OLED en píxeles (8 filas)
#define PANTALLA_ALTO_PAGINA 8

// Número de páginas del OLED (8 en 128x64)
#define PANTALLA_PAGINAS (PANTALLA_ALTO / PANTALLA_ALTO_PAGINA)

// Bytes máximos por transferencia I2C: el buffer de Wire en AVR es de
// 32 bytes y lo que sobra se descarta, así que hay que trocear.
#define PANTALLA_TROZO_I2C 32

// Modo de refresco:
//   1 = por páginas (solo las que cambian; ~3 ms por página)
//   0 = pantalla completa (~24 ms)
// El modo 1 usa el comando PAGEADDR del SSD1306 (estándar del datasheet).
// Si el simulador de Wokwi no lo implementara, se puede poner a 0 sin
// tocar nada más.
#ifndef PANTALLA_REFRESCO_PAGINAS
#define PANTALLA_REFRESCO_PAGINAS 1
#endif

/**
 * Clase Pantalla
 *
 * Encapsula el manejo básico de una pantalla OLED SSD1306:
 * - Inicialización
 * - Mostrar texto centrado
 * - Dibujar un píxel en cada esquina
 * - Escribir texto en una posición
 * - Refresco total o por páginas
 */
class Pantalla {
public:
  /**
   * Constructor.
   * Crea el objeto interno de Adafruit_SSD1306 con las dimensiones y pines definidos.
   */
  Pantalla()
    : _display(PANTALLA_ANCHO, PANTALLA_ALTO, &Wire, PANTALLA_RESET),
      _hayPantalla(false) {
    // El constructor solo crea el objeto; la inicialización real se hace en iniciar()
  }

  /**
   * Inicializa la pantalla OLED.
   * Debe llamarse en setup() antes de cualquier otra función.
   *
   * NO bloquea si la pantalla no responde: devuelve false y el programa
   * sigue funcionando (un mando de vuelo debe volar sin pantalla).
   *
   * @return true si la pantalla está conectada y lista.
   */
  bool iniciar() {
    _hayPantalla = _display.begin(SSD1306_SWITCHCAPVCC, PANTALLA_DIRECCION);
    if (!_hayPantalla) {
      // Sin pantalla: se sigue igual (el transmisor no depende de ella).
      Serial.println(F("Pantalla: no detectada, se sigue sin ella"));
      return false;
    }

    // Limpiar para evitar el splash de Adafruit
    _display.clearDisplay();
    _display.display();
    return true;
  }

  /**
   * @return true si la pantalla se inicializó correctamente.
   */
  bool hayPantalla() const {
    return _hayPantalla;
  }

  /**
   * @return Puntero al framebuffer de la pantalla (solo lectura).
   *         Sirve para depurar el contenido dibujado.
   */
  const uint8_t *getBufferParaTest() const {
    return _display.getBuffer();
  }

  /**
   * Muestra un mensaje centrado horizontalmente en la pantalla.
   *
   * @param mensaje  Texto a mostrar (se usa print, no println).
   * @param tamanio  Tamaño del texto (1 = normal, 2 = doble, etc.). Por defecto 2.
   * @param y        Coordenada vertical en píxeles. Por defecto 20.
   */
  void mostrarMensaje(const String &mensaje, uint8_t tamanio = 2, int16_t y = 20) {
    if (!_hayPantalla) {
      return;
    }
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
    if (!_hayPantalla) {
      return;
    }
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
   * Escribe texto en una posición concreta de la pantalla.
   * El texto se dibuja en el buffer; para que se vea hay que llamar
   * después a refrescarPagina() o refrescarTodo().
   *
   * @param texto    Texto a escribir.
   * @param x        Coordenada horizontal en píxeles.
   * @param y        Coordenada vertical en píxeles.
   * @param tamanio  Tamaño del texto (1 = normal, 2 = doble). Por defecto 1.
   */
  void texto(const char *texto, int16_t x, int16_t y, uint8_t tamanio = 1) {
    if (!_hayPantalla || texto == nullptr) {
      return;
    }
    _display.setTextSize(tamanio);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(x, y);
    _display.print(texto);
  }

  /**
   * Borra una fila completa de la pantalla (en el buffer).
   * Se usa antes de redibujar una fila, para que no queden restos
   * del texto anterior (por ejemplo "127" -> "99").
   *
   * @param y  Coordenada vertical del inicio de la fila.
   */
  void borrarFila(int16_t y) {
    if (!_hayPantalla) {
      return;
    }
    _display.fillRect(0, y, PANTALLA_ANCHO, PANTALLA_ALTO_PAGINA, SSD1306_BLACK);
  }

  /**
   * Limpia toda la pantalla (en el buffer).
   * Después hay que llamar a refrescarTodo() para que se vea.
   */
  void limpiar() {
    if (!_hayPantalla) {
      return;
    }
    _display.clearDisplay();
  }

  /**
   * Envía todo el buffer a la pantalla.
   *
   * Costo: 1024 bytes por I2C ≈ 24 ms a 400 kHz. Es un refresco
   * completo, así que solo conviene al arrancar o de vez en cuando.
   */
  void refrescarTodo() {
    if (!_hayPantalla) {
      return;
    }
    _display.display();
  }

  /**
   * Envía a la pantalla solo una página (8 filas de píxeles).
   *
   * Cada fila de texto de 8 px cae exactamente en una página, así que
   * redibujar un dato y refrescar su página es suficiente.
   *
   * Costo: 128 bytes por I2C ≈ 3 ms a 400 kHz.
   *
   * @param pagina  Número de página (0 a PANTALLA_PAGINAS - 1).
   */
  void refrescarPagina(uint8_t pagina) {
    if (!_hayPantalla || pagina >= PANTALLA_PAGINAS) {
      return;
    }

#if PANTALLA_REFRESCO_PAGINAS
    // Transferencias rápidas (400 kHz) como hace la librería Adafruit.
#if (ARDUINO >= 157) && !defined(ARDUINO_STM32_FEATHER)
    Wire.setClock(400000UL);
#endif

    // Seleccionar el rango de páginas y columnas de la SSD1306.
    // Byte de control 0x00: Co=0, D/C=0 -> son comandos.
    Wire.beginTransmission(PANTALLA_DIRECCION);
    Wire.write((uint8_t)0x00);
    Wire.write((uint8_t)SSD1306_PAGEADDR);
    Wire.write(pagina);
    Wire.write(pagina);
    Wire.write((uint8_t)SSD1306_COLUMNADDR);
    Wire.write(0);
    Wire.write(PANTALLA_ANCHO - 1);
    Wire.endTransmission();

    // Enviar los 128 bytes de esa página.
    // OJO: el buffer de Wire en AVR es de 32 bytes, y Wire.write() con
    // más bytes de los que caben DESCARTA el resto (setWriteError), así
    // que hay que trocear la transferencia: 1 byte de control + 31 de datos.
    const uint8_t *paginaDatos = _display.getBuffer() + (uint16_t)pagina * PANTALLA_ANCHO;

    Wire.beginTransmission(PANTALLA_DIRECCION);
    Wire.write((uint8_t)0x40);  // Co=0, D/C=1 -> datos
    uint8_t enviados = 1;        // el byte de control ya cuenta
    for (uint8_t x = 0; x < PANTALLA_ANCHO; x++) {
      if (enviados >= PANTALLA_TROZO_I2C) {
        Wire.endTransmission();
        Wire.beginTransmission(PANTALLA_DIRECCION);
        Wire.write((uint8_t)0x40);
        enviados = 1;
      }
      Wire.write(paginaDatos[x]);
      enviados++;
    }
    Wire.endTransmission();

#if (ARDUINO >= 157) && !defined(ARDUINO_STM32_FEATHER)
    Wire.setClock(100000UL);
#endif
#else
    // Modo pantalla completa: la librería Adafruit ya trocea la
    // transferencia por el buffer de Wire.
    (void)pagina;
    _display.display();
#endif
  }

private:
  Adafruit_SSD1306 _display;  // Objeto interno de la librería Adafruit
  bool _hayPantalla;          // true si la pantalla respondió al begin()
};

#endif  // PANTALLA_H
