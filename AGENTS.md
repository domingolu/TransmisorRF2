# AGENTS.md

Monorepo PlatformIO para un mando RF: `transmitter/` (Arduino Nano +
nRF24L01+) y `receiver/` (Arduino Pro Mini + nRF24L01+). Migrado desde
el repo `TransmisorRF` (ahí sigue el historial anterior).

## Build y test

- PlatformIO 6.2 instalado vía pip (Python 3.14): ejecutar como
  `python -m platformio run ...` (`pio` no está en PATH en esta máquina).
- `python -m platformio run -e firmware` compila; añadir `-t upload` para
  flashear (el Pro Mini **no tiene USB**: hace falta adaptador FTDI en
  TX/RX/GND/DTR y fijar `upload_port`).
- Cada test de hardware es un entorno: `-e test-botones`,
  `-e test-joystick`, `-e test-potenciometros`, `-e test-switches`,
  `-e test-pantalla-32`, `-e test-pantalla-64`, `-e test-pantalla-lib`,
  `-e test-getting-started`. Los 11 entornos compilan (verificado).
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
- Mapa de canales y cableado: `docs/protocolo-nrf24l01.md` y
  `docs/hardware/wiring.md`. El mapa refleja el comportamiento real del
  código (el comentario del viejo `Transmitter.ino` describía otra
  distribución que nunca se cableó).
- El nRF24L01+ se alimenta a **3.3 V** (nunca 5 V). Los botones de los
  joysticks del TX están en D0/D1 (pines UART) y pueden interferir con el
  monitor serie por USB.

## Gotchas de código

- avr-gcc rechaza identificadores no ASCII (`tamaño` → `tamanio`). UTF-8
  en comentarios sí funciona.
- Adafruit_GFX no tiene `textWidth`; medir texto con `getTextBounds`.
- Las librerías locales header-only necesitan `library.properties` en su
  carpeta para que PlatformIO las reconozca.
