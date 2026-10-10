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

## Canales (estructura `DatosRF`)

Un byte por canal (0-255). Los analógicos se mapean con
`map(analogRead(pin), 0, 1024, 0, 255)`.

| Canal | Señal | Pin TX (Nano) | Valor inicial |
|---|---|---|---|
| ch1 | Joystick izquierdo, eje X | A0 | 127 (centro) |
| ch2 | Joystick izquierdo, eje Y | A1 | 127 |
| ch3 | Joystick derecho, eje X | A2 | 127 |
| ch4 | Joystick derecho, eje Y | A3 | 127 |
| ch5 | Botón joystick izquierdo | D0 | 0 |
| ch6 | Botón joystick derecho | D1 | 0 |
| ch7 | Entrada analógica libre | A4 | 0 |
| ch8–ch10 | Reservados (switches / potenciómetros) | — | 0 |

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
