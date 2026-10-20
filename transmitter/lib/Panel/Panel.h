/**
 * Panel.h
 *
 * Panel de información del transmisor: muestra en la pantalla OLED los
 * datos que se transmiten por RF.
 *
 * Layout (128x64, texto de 6x8 px -> 21 caracteres por fila):
 *
 *   fila 0 (pág 0): YAW  127   THR    0
 *   fila 1 (pág 1): RLL  127   PIT  127
 *   fila 2 (pág 2): CH5    0   CH6    0
 *   fila 3 (pág 3): CH7    0   CH8    0
 *   fila 4 (pág 4): CH9    0   CH10   0
 *   fila 5 (pág 5): rawY  512  rawT  512
 *   fila 6 (pág 6): rawR  512  rawP  512
 *   fila 7 (pág 7): TX OK  pkts 1234
 *
 * "rawY/T/R/P" son las lecturas crudas del ADC (0-1023) de los 4 ejes,
 * útiles para calibrar sin necesidad de un PC.
 *
 * Cada fila de texto de 8 px cae exactamente en una página del OLED
 * (el buffer se indexa como buffer[x + (y/8)*WIDTH]), así que cada dato
 * tiene su propia página: al cambiar uno solo se repinta y se envía esa
 * (~3 ms) en lugar de la pantalla entera (~24 ms). Eso mantiene el ritmo
 * de transmisión a 50 Hz.
 *
 * Uso típico:
 *   #include <Pantalla.h>
 *   #include <Panel.h>
 *
 *   Pantalla pantalla;
 *   Panel panel(pantalla);
 *
 *   void setup() {
 *     if (pantalla.iniciar()) {
 *       panel.iniciar();
 *     }
 *   }
 *
 *   void loop() {
 *     panel.actualizar(datos, crudos.yaw, crudos.throttle, crudos.roll,
 *                      crudos.pitch, txOk, paquetes);
 *   }
 */

#ifndef PANEL_H
#define PANEL_H

#include <Arduino.h>
#include <ProtocoloRF.h>
#include <Pantalla.h>

// Fila de texto (y, por tanto, página del OLED) donde se dibuja cada dato.
// Cada dato tiene su propia página: al cambiar uno solo se repinta esa.
#define PANEL_FILA_YAW     0
#define PANEL_FILA_ROLL    1
#define PANEL_FILA_CH5     2
#define PANEL_FILA_CH8     3
#define PANEL_FILA_CH9     4
#define PANEL_FILA_RAW_YT  5
#define PANEL_FILA_RAW_RP  6
#define PANEL_FILA_ESTADO  7

// Número de filas usadas por el panel (una por página)
#define PANEL_FILAS 8

// Cada cuánto se actualiza la fila de estado (el contador de paquetes
// cambia en cada envío, así que refrescarla a 50 Hz gastaría I2C).
#define PANEL_INTERVALO_ESTADO_MS 500

/**
 * Clase Panel
 *
 * Dibuja los datos del transmisor en la pantalla OLED y refresca solo
 * las páginas que han cambiado.
 */
class Panel {
public:
  /**
   * Constructor.
   *
   * @param pantalla  Referencia a la pantalla donde se dibuja.
   */
  explicit Panel(Pantalla &pantalla)
    : _pantalla(pantalla), _anterior{}, _crudosAnteriores{0, 0, 0, 0},
      _txOkAnterior(false), _primeraVez(true),
      _proximoEstadoMs(0) {
    // Los datos se inicializan en iniciar()
  }

  /**
   * Prepara el panel: limpia la pantalla y marca todo para pintar.
   * Llamar una sola vez en setup(), después de pantalla.iniciar().
   */
  void iniciar() {
    if (!_pantalla.hayPantalla()) {
      return;
    }
    _pantalla.limpiar();
    _primeraVez = true;
    _proximoEstadoMs = 0;
  }

  /**
   * Actualiza el panel con los datos actuales.
   *
   * Repinta y envía por I2C únicamente las páginas cuyos datos cambiaron.
   * Si no cambió nada, no se toca la pantalla (y no se pierde tiempo).
   *
   * @param datos      Datos que se transmiten (canales ya mapeados).
   * @param rawYaw     Lectura cruda del ADC del eje yaw (0-1023).
   * @param rawThrottle Lectura cruda del ADC del eje throttle (0-1023).
   * @param rawRoll    Lectura cruda del ADC del eje roll (0-1023).
   * @param rawPitch   Lectura cruda del ADC del eje pitch (0-1023).
   * @param txOk       true si el último envío por RF fue correcto.
   * @param paquetes   Número de paquetes enviados.
   */
  void actualizar(const DatosRF &datos, int rawYaw, int rawThrottle, int rawRoll,
                  int rawPitch, bool txOk, uint16_t paquetes) {
    if (!_pantalla.hayPantalla()) {
      return;
    }

    uint16_t paginas = 0;  // bitmap de páginas a refrescar

    // Fila 0: yaw y throttle
    if (_primeraVez || datos.yaw != _anterior.yaw || datos.throttle != _anterior.throttle) {
      _pintarEjes(PANEL_FILA_YAW, datos.yaw, datos.throttle);
      paginas |= (1 << PANEL_FILA_YAW);
    }

    // Fila 1: roll y pitch
    if (_primeraVez || datos.roll != _anterior.roll || datos.pitch != _anterior.pitch) {
      _pintarEjes(PANEL_FILA_ROLL, datos.roll, datos.pitch);
      paginas |= (1 << PANEL_FILA_ROLL);
    }

    // Fila 2: ch5 y ch6
    if (_primeraVez || datos.ch5 != _anterior.ch5 || datos.ch6 != _anterior.ch6) {
      _pintarCanales(PANEL_FILA_CH5, datos.ch5, datos.ch6, "CH5", "CH6");
      paginas |= (1 << PANEL_FILA_CH5);
    }

    // Fila 3: ch7 y ch8
    if (_primeraVez || datos.ch7 != _anterior.ch7 || datos.ch8 != _anterior.ch8) {
      _pintarCanales(PANEL_FILA_CH8, datos.ch7, datos.ch8, "CH7", "CH8");
      paginas |= (1 << PANEL_FILA_CH8);
    }

    // Fila 4: ch9 y ch10
    if (_primeraVez || datos.ch9 != _anterior.ch9 || datos.ch10 != _anterior.ch10) {
      _pintarCanales(PANEL_FILA_CH9, datos.ch9, datos.ch10, "CH9", "CH10");
      paginas |= (1 << PANEL_FILA_CH9);
    }

    // Filas 5 y 6: lecturas crudas de los 4 ejes
    if (_primeraVez || rawYaw != _crudosAnteriores[0] || rawThrottle != _crudosAnteriores[1]
        || rawRoll != _crudosAnteriores[2] || rawPitch != _crudosAnteriores[3]) {
      _pintarCrudos(rawYaw, rawThrottle, rawRoll, rawPitch);
      paginas |= (1 << PANEL_FILA_RAW_YT) | (1 << PANEL_FILA_RAW_RP);
    }

    // Fila 7: estado de la transmisión y contador de paquetes.
    // El estado se actualiza al instante; el contador, cada medio segundo
    // (si no, esta página se refrescaría en cada envío).
    if (_primeraVez || txOk != _txOkAnterior || _tocaEstado()) {
      _pintarEstado(txOk, paquetes);
      paginas |= (1 << PANEL_FILA_ESTADO);
    }

    if (paginas == 0) {
      return;  // nada cambió: no se toca la pantalla
    }

    // Enviar solo las páginas que cambiaron.
    for (uint8_t pagina = 0; pagina < PANEL_FILAS; pagina++) {
      if (paginas & (1 << pagina)) {
        _pantalla.refrescarPagina(pagina);
      }
    }

    // Guardar el estado actual para la próxima comparación.
    _anterior = datos;
    _crudosAnteriores[0] = rawYaw;
    _crudosAnteriores[1] = rawThrottle;
    _crudosAnteriores[2] = rawRoll;
    _crudosAnteriores[3] = rawPitch;
    _txOkAnterior = txOk;
    _primeraVez = false;
  }

private:
  /**
   * Pinta una fila con los 4 ejes de vuelo (yaw/throttle/roll/pitch).
   *
   * @param fila   Número de fila (0 = yaw/throttle, 1 = roll/pitch).
   * @param valor1 Valor del primer eje de la fila.
   * @param valor2 Valor del segundo eje de la fila.
   */
  void _pintarEjes(uint8_t fila, uint8_t valor1, uint8_t valor2) {
    int16_t y = fila * PANTALLA_ALTO_PAGINA;
    _pantalla.borrarFila(y);
    _pantalla.texto(fila == PANEL_FILA_YAW ? "YAW" : "RLL", 0, y);
    _pantalla.texto(_numero(valor1), 24, y);
    _pantalla.texto(fila == PANEL_FILA_YAW ? "THR" : "PIT", 60, y);
    _pantalla.texto(_numero(valor2), 90, y);
  }

  /**
   * Pinta una fila con dos canales (ch5-ch10).
   *
   * @param fila   Número de fila.
   * @param valor1 Valor del primer canal.
   * @param valor2 Valor del segundo canal.
   * @param et1    Etiqueta del primer canal.
   * @param et2    Etiqueta del segundo canal.
   */
  void _pintarCanales(uint8_t fila, uint8_t valor1, uint8_t valor2, const char *et1,
                      const char *et2) {
    int16_t y = fila * PANTALLA_ALTO_PAGINA;
    _pantalla.borrarFila(y);
    _pantalla.texto(et1, 0, y);
    _pantalla.texto(_numero(valor1), 30, y);
    _pantalla.texto(et2, 60, y);
    _pantalla.texto(_numero(valor2), 90, y);
  }

  /**
   * Pinta las dos filas con las lecturas crudas de los 4 ejes.
   */
  void _pintarCrudos(int rawYaw, int rawThrottle, int rawRoll, int rawPitch) {
    int16_t y1 = PANEL_FILA_RAW_YT * PANTALLA_ALTO_PAGINA;
    int16_t y2 = PANEL_FILA_RAW_RP * PANTALLA_ALTO_PAGINA;
    _pantalla.borrarFila(y1);
    _pantalla.borrarFila(y2);
    _pantalla.texto("rawY", 0, y1);
    _pantalla.texto(_numeroCrudo(rawYaw), 30, y1);
    _pantalla.texto("rawT", 60, y1);
    _pantalla.texto(_numeroCrudo(rawThrottle), 90, y1);
    _pantalla.texto("rawR", 0, y2);
    _pantalla.texto(_numeroCrudo(rawRoll), 30, y2);
    _pantalla.texto("rawP", 60, y2);
    _pantalla.texto(_numeroCrudo(rawPitch), 90, y2);
  }

  /**
   * Pinta la fila con el estado de la transmisión y el contador de paquetes.
   */
  void _pintarEstado(bool txOk, uint16_t paquetes) {
    int16_t y = PANEL_FILA_ESTADO * PANTALLA_ALTO_PAGINA;
    _pantalla.borrarFila(y);
    _pantalla.texto(txOk ? "TX OK" : "TX FAIL", 0, y);
    _pantalla.texto("pkts", 42, y);
    _pantalla.texto(_numeroPaquetes(paquetes), 78, y);
  }

  /**
   * @return true si toca refrescar el contador de paquetes (cada medio
   *         segundo, para no refrescar la página en cada envío).
   */
  bool _tocaEstado() {
    if ((int32_t)(millis() - _proximoEstadoMs) >= 0) {
      _proximoEstadoMs = millis() + PANEL_INTERVALO_ESTADO_MS;
      return true;
    }
    return false;
  }

  /**
   * Escribe un número alineado a la derecha en un campo de 'ancho'
   * caracteres, rellenando con espacios por la izquierda.
   *
   * No usa String a propósito: el heap va justo en este micro (el
   * framebuffer del OLED ya se lleva 1024 bytes de los 2048).
   *
   * @param valor   Número a escribir.
   * @param ancho   Ancho del campo en caracteres.
   * @param buffer  Buffer donde escribir el texto (debe caber 'ancho'+1).
   */
  static void _numeroDerecha(uint16_t valor, uint8_t ancho, char *buffer) {
    char digitos[6];
    uint8_t n = 0;
    do {
      digitos[n++] = '0' + (valor % 10);
      valor /= 10;
    } while (valor > 0 && n < 5);
    digitos[n] = '\0';

    uint8_t i = 0;
    for (uint8_t esp = n; esp < ancho; esp++) {
      buffer[i++] = ' ';
    }
    for (uint8_t j = n; j > 0; j--) {
      buffer[i++] = digitos[j - 1];
    }
    buffer[i] = '\0';
  }

  /**
   * Texto de un canal (0-255), alineado a la derecha en 3 caracteres.
   */
  const char *_numero(uint8_t valor) {
    _numeroDerecha(valor, 3, _bufferNumero);
    return _bufferNumero;
  }

  /**
   * Texto de una lectura cruda (0-1023), alineado a la derecha en
   * 4 caracteres.
   */
  const char *_numeroCrudo(uint16_t valor) {
    _numeroDerecha(valor, 4, _bufferCrudo);
    return _bufferCrudo;
  }

  /**
   * Texto del contador de paquetes (0-65535), alineado a la derecha
   * en 5 caracteres.
   */
  const char *_numeroPaquetes(uint16_t valor) {
    _numeroDerecha(valor, 5, _bufferPaquetes);
    return _bufferPaquetes;
  }

  Pantalla &_pantalla;

  // Estado anterior, para detectar qué cambió
  DatosRF _anterior;
  int _crudosAnteriores[4];
  bool _txOkAnterior;
  bool _primeraVez;
  uint32_t _proximoEstadoMs;

  // Buffers de texto reutilizables (evitan el heap)
  char _bufferNumero[4];
  char _bufferCrudo[5];
  char _bufferPaquetes[6];};

#endif  // PANEL_H
