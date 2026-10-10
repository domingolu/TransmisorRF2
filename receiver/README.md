# Receptor RF (Arduino Pro Mini)

## Specs

- **Placa:** Arduino Pro Mini (ATmega328, 5 V, 16 MHz) — `board = pro16MHzatmega328`
- **Radio:** nRF24L01+ (mismo protocolo que el transmisor, definido en `../libraries/ProtocoloRF`)
- **Pines RF:** CE → D3, CSN → D2 (SPI por defecto: D11/D12/D13)
- **Salida:** 10 canales por monitor serie (9600 baudios)

## Funcionalidades

- Recibe paquetes de 10 canales (1 byte cada uno) del transmisor.
- Los muestra por el puerto serie en una sola línea:
  `CH1: 128 | CH2: 127 | ... | CH10: 0`
- No genera señales PWM ni controla servomotores todavía (siguiente paso).

## Tests

| Entorno | Qué prueba | Comando |
|---|---|---|
| `test-getting-started` | Ejemplo RF24 (roles TX/RX) | `pio run -e test-getting-started -t upload` |

## Flashear

El Pro Mini **no tiene USB**: hace falta un adaptador FTDI/USB-serie
conectado a los pines **TX, RX, GND** (y **DTR** para el auto-reset).
Selecciona el puerto del adaptador en PlatformIO antes de
`pio run -e firmware -t upload`.

## Notas

- El módulo nRF24L01+ se alimenta a **3.3 V** (ver `../docs/hardware/wiring.md`).
- El receptor y el transmisor comparten la estructura `DatosRF` y la
  dirección `RF_PIPE` de `../libraries/ProtocoloRF`.
