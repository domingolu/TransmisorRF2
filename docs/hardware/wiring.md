# Cableado (wiring)

## Módulo nRF24L01+ → Arduino Nano (Transmisor)

| nRF24L01+ | Nano |
|---|---|
| GND | GND |
| VCC | **3.3V** (¡el módulo no tolera 5V!) |
| CE | D9 |
| CSN | D10 |
| SCK/CLK | D13 |
| MOSI | D11 |
| MISO | D12 |

## Módulo nRF24L01+ → Arduino Pro Mini 3.3V/8MHz (Receptor)

| nRF24L01+ | Pro Mini |
|---|---|
| GND | GND |
| VCC | **3.3V** |
| CE | D8 |
| CSN | D10 |
| SCK/CLK | D13 |
| MOSI | D11 |
| MISO | D12 |

SPI es común en ambas placas: D11 (MOSI), D12 (MISO), D13 (SCK).

> El Pro Mini del receptor es de **3.3 V/8 MHz** y no tiene USB:
> para flashearlo hace falta un adaptador FTDI/USB-serie **a 3.3 V**
> en TX, RX, GND (y DTR para el auto-reset). El RX del Pro Mini
> 3.3 V no tolera 5 V.

## Componentes del transmisor (según los tests)

| Componente | Pines |
|---|---|
| Joystick izquierdo | VRX → A0, VRY → A1, SW → D0 |
| Joystick derecho | VRX → A2, VRY → A3, SW → D1 |
| Potenciómetros | P1 → A7, P2 → A6 |
| Switches | L1 → D3, L2 → D4 (`INPUT_PULLUP`) |
| Botones | D5, D6, D7, D8 (ezButton) |
| Pantalla OLED SSD1306 (I2C) | SDA → A4, SCL → A5, VCC → 5V/3.3V según módulo, GND → GND (dirección 0x3C) |

⚠️ A4/A5 son SDA/SCL de I2C: los usa la pantalla OLED del panel
(ver `../transmitter/README.md`). No son pines digitales de uso general.

⚠️ D0/D1 son RX/TX del UART del Nano; los botones de los joysticks están
ahí en el wiring actual y pueden interferir con el monitor serie por USB.
