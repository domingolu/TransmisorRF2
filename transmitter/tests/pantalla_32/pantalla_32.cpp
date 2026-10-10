/*
 * Test de pantalla OLED SSD1306 128x32 por I2C
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
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Hello World!"));
  display.display();
}

void loop() {
}
