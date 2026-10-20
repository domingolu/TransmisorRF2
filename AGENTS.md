# AGENTS.md

## Proyecto

Proyecto DIY para un mando RF que controla un cuadricóptero (drone):
`transmitter/` (Arduino Nano + nRF24L01+) y `receiver/` (Arduino Pro
Mini + nRF24L01+). Monorepo PlatformIO.



## Build y test

- PlatformIO 6.2 instalado vía pip (Python 3.14): ejecutar como
  `python -m platformio run ...` (`pio` no está en PATH en esta máquina).
- `python -m platformio run -e firmware` compila; añadir `-t upload` para
  flashear (el Pro Mini **no tiene USB**: hace falta adaptador FTDI en
  TX/RX/GND/DTR y fijar `upload_port`).
- Cada test de hardware es un entorno: `-e test-botones`,
  `-e test-joystick`, `-e test-potenciometros`, `-e test-switches`,
  `-e test-pantalla-32`, `-e test-pantalla-64`, `-e test-pantalla-lib`,
  `-e test-getting-started`. Los 12 entornos compilan (verificado).
- `-e wokwi` compila el firmware para el simulador de Wokwi (ver
  `transmitter/README.md`).
- `TransmisorRF.code-workspace` abre el repo como multi-root
  (`.`, `transmitter/`, `receiver/`).

## Gotchas de PlatformIO 6.2 (verificados compilando)

- `src_dir` solo es válido en la sección `[platformio]`, **no** por
  entorno. Para seleccionar fuentes por entorno usa `build_src_filter`
  (nombre antiguo: `src_filter`). Los tests viven en `src/tests/` y cada
  entorno los filtra con `build_src_filter = +<tests/<nombre>.cpp>`; el
  firmware usa `build_src_filter = +<main.cpp>` para excluir los tests.
- IDs del registro cambiaron: RF24 es `nrf24/RF24` (NO `tmrh20/RF24`),
  ezButton es `arduinogetstarted/ezButton` (NO `newbiely/ezButton`).
  Adafruit: `adafruit/Adafruit GFX Library` y `adafruit/Adafruit SSD1306`.
- **No poner `dependencies=` en el `library.properties` de una librería
  local**: PlatformIO 6.2 crashea con
  `AttributeError: 'str' object has no attribute 'get'` (el parser de
  `.properties` no lo convierte al formato dict; solo `library.json` lo
  hace). Declarar dependencias en el `lib_deps` de cada entorno.
- Código compartido entre proyectos: `libraries/ProtocoloRF`, enlazado con
  `lib_extra_dirs = ../libraries` en ambos `platformio.ini`.

## Protocolo RF

- Fuente de verdad única: `libraries/ProtocoloRF/ProtocoloRF.h` —
  `struct DatosRF` (10 canales de 1 byte), `RF_PIPE 0xE8E8F0F0E1LL`,
  250 kbps, PA LOW, auto-ACK off. Nunca duplicarlo en transmitter/ o
  receiver/.
- Ejes de vuelo (cuadricóptero): ch1 = `yaw` (stick izq. horizontal),
  ch2 = `throttle` (izq. vertical), ch3 = `roll` (der. horizontal),
  ch4 = `pitch` (der. vertical); ch5-ch10 sin nombre de vuelo todavía.
- Los 4 ejes del stick se mapean con `mapJoystickValues()`
  (`transmitter/src/main.cpp`): calibración por eje (`CalibracionEje`),
  centro fijo en **127** y flag `reverse` por eje (ninguno invertido).
  Valores de calibración genéricos (0/512/1023) hasta medir el hardware.
- Mapa de canales y cableado: `docs/protocolo-nrf24l01.md` y
  `docs/hardware/wiring.md`. El mapa refleja el comportamiento real del código (el comentario del viejo `Transmitter.ino` describía otra
  distribución que nunca se cableó).
- El nRF24L01+ se alimenta a **3.3 V** (nunca 5 V). Los botones de los
  joysticks del TX están en D0/D1 (pines UART) y pueden interferir con el monitor serie por USB.

## Gotchas de código

- avr-gcc rechaza identificadores no ASCII (`tamaño` → `tamanio`). UTF-8 en comentarios sí funciona.
- Adafruit_GFX no tiene `textWidth`; medir texto con `getTextBounds`.
- Las librerías locales header-only necesitan `library.properties` en su carpeta para que PlatformIO las reconozca.
- El buffer del SSD1306 (1024 B en 128x64) se reserva con `malloc()`: **no**
  aparece en el RAM que reporta PlatformIO (solo cuenta `.data` + `.bss`).
  El RAM real = estáticos + buffer + stack (medido: 1789 B de 2048, ~259 B
  libres para el stack; la cadena más profunda del panel usa ~141 B).
- Un refresco completo de 128x64 son 1024 B por I2C ≈ 24 ms a 400 kHz
  (≈ 92 ms a 100 kHz): no se puede refrescar la pantalla en cada ciclo de
  20 ms del TX (50 Hz) sin pausar la radio.
- Wire en AVR tiene un buffer de 32 B: `Wire.write(buf, n)` con n > 32
  **descarta** los bytes que sobran (`setWriteError`). Hay que trocear en
  trozos de ≤ 32 B con `endTransmission()`/`beginTransmission()` entre ellos.
- Cada fila de texto de 8 px del GFX ocupa exactamente una página del
  SSD1306 (buffer indexado como `buffer[x + (y/8)*WIDTH]`): el refresco por
  páginas sucias se hace con `PAGEADDR` + 128 B por página.
- `Adafruit_SSD1306::ssd1306_commandList()` es `protected`; los públicos
  son `ssd1306_command()` y `getBuffer()`.

## Convenciones

- Textos en español.
- Código simple, nombres descriptivos y comentarios solo donde aporten.
- Antes de implementar cambios, revisar el hardware y la estructura existentes en el repositorio.
- Las pantallas OLED son siempre opcionales y no bloquean el flujo
  principal: un mando de vuelo debe funcionar sin pantalla.

## Forma de trabajar

- Haz solo lo que se pide: no añadas funcionalidades por tu cuenta.
- Cambios pequeños y enfocados; no reescribas lo que ya funciona.
- Documentar todo lo que se hace en los respectivos README.md
- Al terminar, resume qué has cambiado y cualquier decisión que yo deba revisar.

## Límites

- ✅ Siempre: actualizar `MEMORY.md` al terminar cada tarea.
- ⚠️ Pregunta antes: crear o modificar archivos, borrar archivos o cambiar el formato de los datos guardados.
- ⚠️ Pregunta antes: añadir dependencias, frameworks o un paso de build 
- 🚫 Nunca: <nada por ahora>

## Memoria

- Al empezar, lee `MEMORY.md` para conocer el estado del proyecto y las decisiones tomadas.
- Al terminar una tarea, actualízalo: estado actual, decisiones importantes (con su porqué) y errores a evitar.
- Mantenlo breve (máximo ~50 líneas): resume o elimina lo que ya no aporte.
- Si algo se convierte en una regla permanente, propón moverlo a `AGENTS.md` en lugar de dejarlo en la memoria.
- No guardes nunca datos sensibles (claves, tokens, datos personales)
