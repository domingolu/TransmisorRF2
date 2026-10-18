# Protocolo de comunicación RF (nRF24L01+)

La fuente de verdad del protocolo es `libraries/ProtocoloRF/ProtocoloRF.h`,
incluida por el transmisor y el receptor. Cualquier cambio en la estructura
o en la configuración de radio se hace ahí, nunca duplicado en cada proyecto.

## Configuración de radio

| Parámetro | Valor |
|---|---|
| Pipe (dirección) | `0xE8E8F0F0E1LL` (`RF_PIPE`) |
| Data rate | `RF24_250KBPS` |
| PA level | `RF24_PA_LOW` (solo afecta al transmisor) |
| Auto-ACK | desactivado |
| Payload | 10 bytes (1 byte por canal) |
| Tasa de transmisión | ~50 Hz (un paquete cada 20 ms) |

## Canales (estructura `DatosRF`)

Un byte por canal (0-255). Los cuatro primeros canales son los ejes
de vuelo del cuadricóptero:

| Eje | Stick | Movimiento | Controla |
|---|---|---|---|
| **Throttle** | Izquierdo vertical | Arriba / abajo | Altura (subida / bajada) |
| **Yaw** | Izquierdo horizontal | Izquierda / derecha | Rotación sobre su eje vertical |
| **Pitch** | Derecho vertical | Arriba / abajo | Inclinación adelante / atrás |
| **Roll** | Derecho horizontal | Izquierda / derecha | Inclinación lateral |

Mapeo: los joysticks usan `map(analogRead(pin), 0, 1024, 0, 255)` y
los potenciómetros `map(analogRead(pin), 0, 1023, 0, 255)` (rango
completo del ADC). Botones y switches se envían con semántica
**1 = activado, 0 = inactivo** y antirrebote de 50 ms (ezButton).

| Canal | Campo | Señal | Pin TX (Nano) | Valor inicial |
|---|---|---|---|---|
| ch1 | `yaw` | Stick izquierdo horizontal (yaw) | A0 (VRX izq.) | 127 (centro) |
| ch2 | `throttle` | Stick izquierdo vertical (throttle) | A1 (VRY izq.) | 127 |
| ch3 | `roll` | Stick derecho horizontal (roll) | A2 (VRX der.) | 127 |
| ch4 | `pitch` | Stick derecho vertical (pitch) | A3 (VRY der.) | 127 |
| ch5 | `ch5` | Botón joystick izquierdo (1 = pulsado) | D0 | 0 |
| ch6 | `ch6` | Botón joystick derecho (1 = pulsado) | D1 | 0 |
| ch7 | `ch7` | Switch L1 (1 = activado, tipo toggle) | D3 | 0 |
| ch8 | `ch8` | Switch L2 (1 = activado, tipo toggle) | D4 | 0 |
| ch9 | `ch9` | Potenciómetro P1 (mapeo directo) | A7 | 0 |
| ch10 | `ch10` | Potenciómetro P2 (mapeo directo) | A6 | 0 |

### Semántica de valores

- **Centro = 127**: es el valor neutro inicial de los cuatro ejes
  (stick al centro). El firmware lo carga en `inicializarDatosRF()`
  y se sobreescribe con la lectura real en el primer `loop()`.
- **"Arriba" en el stick = valor mayor** (255) y "abajo" = valor
  menor (0). Para invertir un eje, se intercambian los extremos del
  `map()` (ver el ejemplo en `transmitter/src/main.cpp`).
- **Throttle**: 127 es el centro *geométrico* del stick, no el idle
  de vuelo. En un cuadricóptero el idle se recorta por software en
  el lado del controlador de vuelo (mezcla/failsafe), no en el mando.
- **Botones y switches**: 1 = activado/pulsado, 0 = inactivo.
  Los switches son tipo toggle (el valor queda en 1 mientras están
  en esa posición).

> **Nota de nomenclatura:** los campos `yaw`, `throttle`, `roll` y
> `pitch` de `DatosRF` reemplazan a los antiguos `ch1`–`ch4`. El orden
> y los tipos no cambiaron, así que el formato en el aire es idéntico
> (compatible con firmwares previos). ch5–ch10 conservaron el nombre
> `chN` porque su función (botones, switches, potenciómetros) todavía
> no está definida para el vuelo.

> **Nota de migración:** el código original del transmisor (repo `TransmisorRF`)
> tenía un comentario que describía otra distribución (ch3 = botón izquierdo,
> ch7 = switch izquierdo, ch9/ch10 = potenciómetros), pero el código que se
> ejecuta usa el mapeo de la tabla anterior. Este documento refleja el
> comportamiento real. Si tu cableado sigue el comentario original, ajusta
> `loop()` en `transmitter/src/main.cpp`.

> **Ojo con D0/D1:** son los pines RX/TX del UART del Nano. Usarlos como
> entradas digitales puede interferir con el monitor serie por USB. Si hay
> comportamiento raro, mueve los botones a otros pines digitales libres.

## Reglas

1. La estructura `DatosRF` y `RF_PIPE` deben ser **idénticas** en TX y RX
   (por eso viven en una librería compartida).
2. El TX fija el tamaño de payload a `sizeof(DatosRF)` = 10 bytes.
3. El RX lee `sizeof(DatosRF)` bytes de cada paquete.
4. El orden y los tipos de la estructura no se cambian sin actualizar
   **ambos** firmwares a la vez (el header compartido lo garantiza).

## Alcance

nRF24L01+ en 2.4 GHz: ~100 m en campo abierto con PA_LOW y antena integrada
(los módulos con antena externa y PA amplificado alcanzan bastante más).
Alimentar el módulo a **3.3 V** (máx. 3.6 V) — nunca a 5 V.
