/*
 * Test de dos switches (L1 -> D3, L2 -> D4)
 *
 * Arduino Nano - Two Switches Status Monitor
 *
 * This program reads and displays the state of two switches:
 * - Switch L1 connected to digital pin D3
 * - Switch L2 connected to digital pin D4
 *
 * Hardware:
 * - Two switches/pushbuttons
 * - L1: one terminal to D3, other terminal to GND
 * - L2: one terminal to D4, other terminal to GND
 * - Enable internal pull-up resistors (no external resistors needed)
 *
 * Created: September 17, 2026
 */


#include <Arduino.h>

// Pin definitions
const int PIN_L1 = 3;  // Switch L1 on digital pin 3
const int PIN_L2 = 4;  // Switch L2 on digital pin 4


void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Configure pins as inputs with internal pull-up resistors
  pinMode(PIN_L1, INPUT_PULLUP);
  pinMode(PIN_L2, INPUT_PULLUP);

  Serial.println("Two Switches Monitor Started");
  Serial.println("L1 on D3, L2 on D4");
  Serial.println("--------------------------");
}


void loop() {
  // Read switch states
  // LOW = pressed (connected to GND)
  // HIGH = released (pull-up resistor)
  int stateL1 = digitalRead(PIN_L1);
  int stateL2 = digitalRead(PIN_L2);

  // Display status
  Serial.print("L1 (D3): ");
  if (stateL1 == LOW) {
    Serial.print("PRESSED");
  } else {
    Serial.print("RELEASED");
  }

  Serial.print("  |  L2 (D4): ");
  if (stateL2 == LOW) {
    Serial.println("PRESSED");
  } else {
    Serial.println("RELEASED");
  }

  // Small delay for readability
  delay(200);
}
