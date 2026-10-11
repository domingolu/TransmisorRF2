/*
 * Test de pantalla OLED SSD1306 128x64 por I2C (versión simplificada)
 *
 * Conexiones (ver docs/hardware/wiring.md):
 *   VCC -> 5V/3.3V (según el módulo), GND -> GND,
 *   SDA -> A4, SCL -> A5, dirección 0x3C
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Inicializar display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // Si falla, se queda aquí (no hay pantalla)
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(2);                // Tamaño de texto
  display.setTextColor(SSD1306_WHITE);   // Texto blanco
  display.setCursor(10, 20);             // Posición inicial (ajustable)
  display.println("Hello Word");         // Texto a mostrar
  display.display();                     // Actualizar pantalla
}

void loop() {
  // No hace falta nada aquí
}
