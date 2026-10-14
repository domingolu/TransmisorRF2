# MEMORY.md
Memoria del proyecto entre sesiones. Máximo ~50 líneas: resume o elimina lo que ya no aporte.

## Estado actual
- v1.4: potenciómetros P1/P2 por ch9/ch10 implementados
  (mapa de canales 10/10 completo); pendiente probar en
  hardware. TX y RX probados en hardware.
- TX: Nano con nuevo bootloader (`nanoatmega328new`), serie 115200.
- RX: Pro Mini 3.3V/8MHz (`pro8MHzatmega328`), CE→D8, CSN→D10, serie 115200.
- Los 11 entornos compilan (verificado con PlatformIO 6.2).

## Decisiones (y por qué)
- Monorepo único (transmitter/ + receiver/): el protocolo RF es compartido y debe versionarse junto con ambos firmwares.
- Protocolo en libraries/ProtocoloRF (header compartido, lib_extra_dirs): DatosRF y RF_PIPE nunca se desincronizan. Corrige la incompatibilidad original (TX enviaba 10 canales, RX definía 7).
- Tests como entornos en src/tests/ con build_src_filter: src_dir por entorno no existe en PIO 6.x.
- Todos los envs del TX usan nanoatmega328new: la placa física tiene el nuevo bootloader (flasheo a 115200; con nanoatmega328 el upload a 57600 puede fallar).
- Mapa de canales completo (10/10 asignados): ch1-6 = joysticks (A0-A3, D0/D1); ch7-8 = switches L1/L2 (D3/D4, toggle, debounce 50 ms, 1=activado); ch9-10 = potenciómetros P1/P2 (A7/A6, mapeo directo, tope ADC 1023).
- Tasa ~50 Hz (delay 20 ms): 1 Hz original era inservible para un control remoto.
- Botones con semántica 1=pulsado, 0=sin pulsar: ezButton con pull-up interno da 0 al pulsar, se invierte en el TX.
- getButtonState() (nivel) y no isPressed() (flanco): nivel es adecuado para RC.

## Aprendizajes y errores a evitar
- PIO 6.2: src_dir solo en [platformio]; usar build_src_filter por entorno.
- IDs del registro cambiaron: nrf24/RF24 (no tmrh20), arduinogetstarted/ezButton (no newbiely).
- No poner dependencies= en library.properties de librerías locales: crashea con AttributeError 'str' object has no attribute 'get'.
- avr-gcc rechaza identificadores no ASCII (tamaño → tamanio); UTF-8 en comentarios sí funciona.
- ezButton(pin) hace pinMode INPUT_PULLUP solo: no redefinir el pin en setup().
- D0/D1 (UART) funcionaron en la prueba real del TX; si hay fallas de serie, mover pines.

## Próximos pasos
- Crear repo en GitHub (domingolu/TransmisorRF2) y hacer push.
- Probar los potenciómetros P1/P2 (ch9/ch10) en hardware.
- Pantalla OLED en A4/A5 (librería Pantalla ya existe).
