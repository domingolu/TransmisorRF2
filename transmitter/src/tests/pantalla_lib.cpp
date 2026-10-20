/**
 * Demo de la librería propia Pantalla
 *
 * Ejemplo de uso de la biblioteca Pantalla para:
 * - Mostrar "Hello Word" centrado
 * - Dibujar un píxel en cada esquina de la pantalla OLED SSD1306
 */

#include <Arduino.h>
#include <Pantalla.h>

Pantalla pantalla;  // Instancia de nuestra biblioteca

void setup() {
  // iniciar() no bloquea: si no hay pantalla, se avisa y se sigue.
  if (!pantalla.iniciar()) {
    return;
  }

  pantalla.limpiar();          // Opcional: empezar con pantalla limpia

  pantalla.mostrarMensaje("Hello Word");  // Mensaje centrado
  pantalla.dibujarEsquinas();             // Píxeles en las esquinas

  pantalla.refrescarTodo();   // Enviar todo a la pantalla
}

void loop() {
  // No es necesario hacer nada para mantener la imagen fija
}
