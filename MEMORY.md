# MEMORY.md
Memoria del proyecto entre sesiones. Máximo ~50 líneas: resume o elimina lo que ya no aporte.

## Estado actual
- v1.5: nomenclatura de ejes del cuadricóptero (yaw/throttle/roll/pitch)
  en `DatosRF`, el firmware y la documentación.
- Repo público en GitHub: github.com/domingolu/TransmisorRF2
  (rama `main`, remoto `origin`; auth con Git Credential Manager).
- TX: Nano con nuevo bootloader (`nanoatmega328new`), serie 115200.
- RX: Pro Mini 3.3V/8MHz (`pro8MHzatmega328`), CE→D8, CSN→D10, serie 115200.
- Los 12 entornos compilan (verificado con PlatformIO 6.2).
- Simulación Wokwi del TX lista: `transmitter/diagram.json` +
  `transmitter/wokwi.toml` + entorno `[env:wokwi]` (verificado con
  `wokwi-cli lint` y `pio run -e wokwi`).
- **Panel OLED en el TX** (`lib/Panel/`): muestra los 10 canales, las
  lecturas crudas de los 4 ejes y el estado/contador del envío.
  Refresco por páginas sucias; la pantalla es opcional (no bloquea).

## Decisiones (y por qué)
- Monorepo único (transmitter/ + receiver/): el protocolo RF es compartido y debe versionarse junto con ambos firmwares.
- Protocolo en libraries/ProtocoloRF (header compartido, lib_extra_dirs): DatosRF y RF_PIPE nunca se desincronizan. Corrige la incompatibilidad original (TX enviaba 10 canales, RX definía 7).
- Tests como entornos en src/tests/ con build_src_filter: src_dir por entorno no existe en PIO 6.x.
- Todos los envs del TX usan nanoatmega328new: la placa física tiene el nuevo bootloader (flasheo a 115200; con nanoatmega328 el upload a 57600 puede fallar).
- Mapa de canales completo (10/10 asignados): ch1-6 = joysticks (A0-A3, D0/D1); ch7-8 = switches L1/L2 (D3/D4, toggle, debounce 50 ms, 1=activado); ch9-10 = potenciómetros P1/P2 (A7/A6, mapeo directo, tope ADC 1023).
- Campos de DatosRF renombrados a yaw/throttle/roll/pitch (ch1-ch4) porque el mando controla un cuadricóptero; ch5-ch10 conservan chN hasta que se defina su función en vuelo. Renombrar campos no cambia el formato en el aire (mismo orden y tipos), así que es compatible con firmwares previos.
- Ejes del stick mapeados con `mapJoystickValues()`: calibración por eje (`CalibracionEje`), centro fijo en 127 y flag `reverse` por eje. Se adopta la idea del proyecto de referencia NRF24_6CH (resetData/mapJoystickValues) pero con centro 127 (no 128) y sin `resetData` propio (ya está `inicializarDatosRF()` en la librería compartida).
- Throttle arranca en 0 (motores detenidos): más seguro que el centro 127 al navegar.
- Calibración con valores genéricos (0/512/1023) hasta medir el hardware real; pendiente la feature de calibración con OLED + pulsadores (y hacer configurables los flags `reverse`).
- Tasa ~50 Hz (delay 20 ms): 1 Hz original era inservible para un control remoto.
- Botones con semántica 1=pulsado, 0=sin pulsar: ezButton con pull-up interno da 0 al pulsar, se invierte en el TX.
- getButtonState() (nivel) y no isPressed() (flanco): nivel es adecuado para RC.
- Simulación Wokwi: el enlace RF NO se puede simular (Wokwi no tiene parte
  oficial para el nRF24L01+). Se resolvió con un stub header-only de RF24
  en `transmitter/wokwi-lib/RF24/`, enlazado solo en `[env:wokwi]` vía
  `lib_extra_dirs`: el firmware real compila sin cambios. El stub va en
  `wokwi-lib/` (no en `lib/`) porque si estuviera en `lib/` el LDF lo
  enlazaría también en `[env:firmware]`.
- El diagrama se valida con `wokwi-cli lint` (binario de GitHub releases;
  en Windows `wokwi-cli-win-x64.exe`): detecta pines y atributos
  inválidos, es la única verificación sin abrir el simulador.
  `wokwi-cli run` pide WOKWI_CLI_TOKEN (cuenta Wokwi): no hay simulación
  headless sin token.
- Panel OLED: refresco por páginas sucias (PAGEADDR + 128 B, ~3 ms) en vez
  de refresco completo (~24 ms), porque un refresco completo en cada
  ciclo de 20 ms del TX (50 Hz) frenaría la radio. Cada fila de texto de
  8 px = una página del OLED (buffer indexado `buffer[x + (y/8)*WIDTH]`).
- La pantalla es **opcional**: `Pantalla::iniciar()` devuelve false y no
  bloquea si no está conectada (un mando de vuelo debe volar sin ella).
- RAM real del TX con OLED = ~1789 B de 2048 (87 %): el framebuffer del
  SSD1306 son 1024 B de `malloc()` que NO salen en el reporte de
  PlatformIO. Quedan ~259 B de stack; la cadena más profunda del panel
  usa ~141 B. Fallback a 128x32 si hiciera falta (512 B de buffer).
- `Wire` en AVR tiene buffer de 32 B y `write(buf, n)` con n > 32
  DESCARTA el sobrante: trocear las transferencias (lo hace Adafruit con
  WIRE_MAX=32).
- `ssd1306_commandList()` es `protected`; usar `ssd1306_command()` o
  `Wire` directamente (y `getBuffer()` es público).

## Aprendizajes y errores a evitar
- PIO 6.2: src_dir solo en [platformio]; usar build_src_filter por entorno.
- IDs del registro cambiaron: nrf24/RF24 (no tmrh20), arduinogetstarted/ezButton (no newbiely).
- No poner dependencies= en library.properties de librerías locales: crashea con AttributeError 'str' object has no attribute 'get'.
- avr-gcc rechaza identificadores no ASCII (tamaño → tamanio); UTF-8 en comentarios sí funciona.
- ezButton(pin) hace pinMode INPUT_PULLUP solo: no redefinir el pin en setup().
- D0/D1 (UART) funcionaron en la prueba real del TX; si hay fallas de serie, mover pines.

## Próximos pasos
- Medir el rango real de cada eje (el monitor imprime el crudo) y cargar los valores en CalibracionEje; verificar que cada stick mueva el eje correcto (e invertir los que haga falta).
- Probar los potenciómetros P1/P2 (ch9/ch10) en hardware.
- Feature de calibración con OLED + pulsadores (y flags `reverse` configurables).
