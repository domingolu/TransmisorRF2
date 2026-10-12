# Transmisor RF (Arduino Nano)

Transmisor RF (TX) hecho con Arduino Nano que servirá como mando a distancia para proyectos DIY.

## Specs

- **Placa:** Arduino Nano (ATmega328, 5 V, 16 MHz) — `board = nanoatmega328`
- **Radio:** nRF24L01+ (2.4 GHz, 250 kbps, PA LOW, auto-ACK off)
- **Canales:** 10 (1 byte cada uno, 0-255) — ver `../docs/protocolo-nrf24l01.md`
- **Entradas:** 2 joysticks analógicos con botón (A0-A3, D0/D1); A4 reservado (futura pantalla OLED)
- **Pines RF:** CE → D9, CSN → D10 (SPI por defecto: D11/D12/D13)
- **Serie:** 115200 baudios

## Funcionalidades

- Lee los dos joysticks (ejes X/Y y botones con antirrebote,
  vía la librería propia `Joystick`) y transmite los 10 canales
  por RF al receptor a ~50 Hz.
- Botones con semántica 1 = pulsado, 0 = sin pulsar
  (pull-up interno invertido en el firmware).
- Estructura de datos y configuración de radio compartidas con el
  receptor (`libraries/ProtocoloRF`).
- Muestra por monitor serie los valores transmitidos.

## Librerías propias

- `lib/Joystick/` — lectura de joystick analógico con botón (antirrebote; usa ezButton)
- `lib/Pantalla/` — pantalla OLED SSD1306 I2C (usa Adafruit GFX / SSD1306)

## Tests de hardware

Los tests viven en `src/tests/` y cada entorno compila solo su sketch
(vía `build_src_filter` en `platformio.ini`).

| Entorno | Qué prueba | Comando |
|---|---|---|
| `test-botones` | 4 botones con debounce (D5-D8) | `pio run -e test-botones -t upload` |
| `test-joystick` | 2 joysticks con botón (librería Joystick) | `pio run -e test-joystick -t upload` |
| `test-potenciometros` | 2 potenciómetros (A6, A7) | `pio run -e test-potenciometros -t upload` |
| `test-switches` | 2 switches (D3, D4) | `pio run -e test-switches -t upload` |
| `test-pantalla-32` | OLED SSD1306 128x32 | `pio run -e test-pantalla-32 -t upload` |
| `test-pantalla-64` | OLED SSD1306 128x64 | `pio run -e test-pantalla-64 -t upload` |
| `test-pantalla-lib` | Librería Pantalla (demo) | `pio run -e test-pantalla-lib -t upload` |
| `test-getting-started` | Ejemplo RF24 (roles TX/RX) | `pio run -e test-getting-started -t upload` |

Para compilar sin flashear, quita `-t upload`.

## Notas

- D0/D1 son RX/TX del UART del Nano; si el monitor serie falla, mueve los botones de los joysticks a otros pines digitales libres.
- El módulo nRF24L01+ se alimenta a **3.3 V** (ver `../docs/hardware/wiring.md`).
