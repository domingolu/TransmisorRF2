# Transmisor RF (Arduino Nano)

Transmisor RF (TX) hecho con Arduino Nano que servirá como mando a distancia para proyectos DIY.

## Specs

- **Placa:** Arduino Nano (ATmega328, 5 V, 16 MHz) — `board = nanoatmega328new` con nuevo bootloader
- **Radio:** nRF24L01+ (2.4 GHz, 250 kbps, PA LOW, auto-ACK off)
- **Canales:** 10 (1 byte cada uno, 0-255) — ver `../docs/protocolo-nrf24l01.md`
- **Entradas:** 2 joysticks analógicos con botón (A0-A3, D0/D1); 2 switches L1/L2 (D3/D4); 2 potenciómetros P1/P2 (A6/A7)
- **Pines RF:** CE → D9, CSN → D10 (SPI por defecto: D11/D12/D13)
- **Serie:** 115200 baudios

## Funcionalidades

- Lee los dos joysticks (ejes X/Y y botones con antirrebote,
  vía la librería propia `Joystick`) y transmite los 10 canales
  por RF al receptor a ~50 Hz.
- Los cuatro primeros canales son los ejes de vuelo del cuadricóptero:
  stick izquierdo vertical = **throttle**, izquierdo horizontal = **yaw**,
  derecho vertical = **pitch**, derecho horizontal = **roll**
  (centro = 127; "arriba" = valor mayor). El throttle arranca en 0
  (motores detenidos) hasta que se mueva el stick.
- Cada eje se calibra por su rango real (`CalibracionEje` en
  `src/main.cpp`); hoy se usan valores genéricos (0/512/1023) y
  ningún eje está invertido.
- Botones con semántica 1 = pulsado, 0 = sin pulsar
  (pull-up interno invertido en el firmware).
- Switches L1/L2 (D3/D4, tipo toggle) por ch7/ch8 con
  antirrebote de 50 ms; 1 = activado, 0 = inactivo.
- Potenciómetros P1/P2 (A7/A6) por ch9/ch10, mapeo
  directo 0-255 (tope de ADC 1023).
- Estructura de datos y configuración de radio compartidas con el
  receptor (`libraries/ProtocoloRF`).
- Muestra por monitor serie los valores transmitidos, con la lectura
  cruda (0-1023) de cada eje para poder calibrar.

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

## Simulación con Wokwi

El entorno `[env:wokwi]` simula el transmisor en [Wokwi](https://wokwi.com)
para comprobar la lectura de los mandos y el mapeo de los 10 canales sin
necesitar hardware.

```sh
pio run -e wokwi          # compila el firmware para el simulador
```

Después, con la extensión **Wokwi for VS Code** instalada, abrí la carpeta
`transmitter/` y ejecutá `F1` → **Wokwi: Start Simulator**.

Archivos de la simulación:

| Archivo | Para qué sirve |
|---|---|
| `diagram.json` | El circuito del transmisor en Wokwi |
| `wokwi.toml` | Apunta al firmware compilado (`.pio/build/wokwi/firmware.hex`) |
| `wokwi-lib/RF24/` | Stub local de la librería RF24 (solo para simular) |

### Qué se simula y qué no

- **Se simula**: los dos joysticks (ejes + botón), los dos
  potenciómetros, los dos switches y la lectura de la pantalla OLED.
- **No se simula**: el enlace RF. Wokwi no tiene una parte oficial para el
  `nRF24L01+`, así que el entorno `[env:wokwi]` usa un **stub** de la
  librería RF24 (`wokwi-lib/RF24/`) que implementa solo la parte de la API
  que usa el firmware. El firmware real (`src/main.cpp`) compila **sin
  cambios**: lo que cambia es la librería de radio.
- El stub vuelca el payload por el monitor serie y hace parpadear el LED
  de actividad del Nano (D13) en cada envío, para ver el ritmo de
  transmisión (~50 Hz).

### Limitaciones a tener en cuenta

- Los ejes del joystick en Wokwi tienen la polaridad **invertida** respecto
  al módulo KY-023: en Wokwi `HORZ` = 1023 cuando el stick va hacia la
  izquierda (y `VERT` = 1023 hacia arriba). Como la calibración del
  hardware real todavía usa valores genéricos (0/512/1023), el simulador
  sirve justamente para comprobar los flags `reverse` de `CalibracionEje`
  en `src/main.cpp` contra el comportamiento real del módulo.
- La pantalla OLED está cableada (SDA → A4, SCL → A5) pero el firmware
  actual todavía no la usa: se verá en negro hasta que se implemente la
  feature de calibración.
- Los botones de los joysticks van a D0/D1, que son los pines del UART
  del Nano. Es el mismo conflicto que existe en el hardware real: puede
  interferir con el monitor serie.

## Notas

- D0/D1 son RX/TX del UART del Nano; si el monitor serie falla, mueve los botones de los joysticks a otros pines digitales libres.
- El módulo nRF24L01+ se alimenta a **3.3 V** (ver `../docs/hardware/wiring.md`).
