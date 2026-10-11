# TransmisorRF2

Mando radio-control con Arduino y módulo **nRF24L01+**: un **transmisor**
(Arduino Nano) y un **receptor** (Arduino Pro Mini), desarrollados en un
único repositorio (monorepo) con **PlatformIO** y VS Code.

## Estructura del repositorio

```
TransmisorRF2/
├── docs/                       # Documentación común
│   ├── protocolo-nrf24l01.md   # Protocolo RF: pipe, payload y canales
│   └── hardware/wiring.md      # Cableado de ambos dispositivos
├── libraries/                  # Librerías propias compartidas
│   └── ProtocoloRF/            # Estructura DatosRF + configuración de radio
├── transmitter/                # Proyecto PlatformIO del transmisor (Nano)
│   ├── platformio.ini
│   ├── src/main.cpp            # Firmware del transmisor
│   ├── src/tests/              # Tests de hardware (uno por env)
│   └── lib/                    # Joystick y Pantalla (librerías propias)
└── receiver/                   # Proyecto PlatformIO del receptor (Pro Mini)
    ├── platformio.ini
    ├── src/main.cpp            # Firmware del receptor
    └── src/tests/
```

## Quickstart

1. Instalar la extensión **PlatformIO IDE** en VS Code.
2. Abrir la carpeta `transmitter/` o `receiver/` como proyecto.
3. Compilar y flashear:

   ```sh
   pio run -e firmware            # compila
   pio run -e firmware -t upload  # flashea (verificar el puerto)
   ```

4. Tests de hardware (ver `transmitter/README.md`), cada uno compila
   solo su sketch con `build_src_filter`:

   ```sh
   pio run -e test-botones
   pio run -e test-joystick
   # ... etc.
   ```

## Documentación

- **Protocolo RF** (`docs/protocolo-nrf24l01.md`): pipe, data rate, payload y mapa de canales.
- **Cableado** (`docs/hardware/wiring.md`): nRF24L01+ y componentes de cada placa.
- **Specs por dispositivo**: `transmitter/README.md` y `receiver/README.md`.

## Notas

- El protocolo vive en `libraries/ProtocoloRF` y lo incluyen ambos proyectos
  (`lib_extra_dirs = ../libraries`), así que la estructura de datos y la
  dirección del pipe nunca pueden desincronizarse entre TX y RX.
- Migrado desde el repositorio `TransmisorRF` (pruebas por componente).
  El historial anterior sigue disponible en ese repositorio.
