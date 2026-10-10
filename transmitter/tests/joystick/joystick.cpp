/*
 * Test de dos joysticks con botón (librería propia Joystick)
 *
 * Arduino Dual Joystick Example - Main Program
 *
 * This example demonstrates how to use the Joystick library
 * to read two analog joysticks with button states.
 *
 * Hardware:
 * - Two analog joystick modules with VRX, VRY, and SW pins
 * - Left Joystick:  VRX -> A0, VRY -> A1, SW -> 0
 * - Right Joystick: VRX -> A2, VRY -> A3, SW -> 1
 *
 * Created based on ArduinoGetStarted.com example
 * Modified for modular architecture by Domingo García
 * Modified for dual joystick support
 * Date: September 17, 2026
 */


#include <Arduino.h>
#include "Joystick.h"


// Create two joystick instances
// Parameters: VRX pin, VRY pin, SW pin
Joystick joystickLeft(A0, A1, 0);
Joystick joystickRight(A2, A3, 1);


void setup() {
  Serial.begin(9600);

  // Initialize both joysticks
  joystickLeft.begin();
  joystickRight.begin();

  // Optional: customize debounce time
  // joystickLeft.setDebounceTime(50);
  // joystickRight.setDebounceTime(50);

  Serial.println("Dual Joystick initialized. Move joysticks or press buttons...");
}


void loop() {
  // Update both joystick states (required for button debouncing)
  joystickLeft.loop();
  joystickRight.loop();

  // ========== LEFT JOYSTICK ==========

  // Check left button events
  if (joystickLeft.isPressed()) {
    Serial.println("[LEFT] Button pressed");
    // TODO: Add your custom action here
  }

  if (joystickLeft.isReleased()) {
    Serial.println("[LEFT] Button released");
    // TODO: Add your custom action here
  }

  // Read left joystick values
  int xLeft = joystickLeft.readX();
  int yLeft = joystickLeft.readY();
  int bLeft = joystickLeft.getButtonState();

  // ========== RIGHT JOYSTICK ==========

  // Check right button events
  if (joystickRight.isPressed()) {
    Serial.println("[RIGHT] Button pressed");
    // TODO: Add your custom action here
  }

  if (joystickRight.isReleased()) {
    Serial.println("[RIGHT] Button released");
    // TODO: Add your custom action here
  }

  // Read right joystick values
  int xRight = joystickRight.readX();
  int yRight = joystickRight.readY();
  int bRight = joystickRight.getButtonState();

  // ========== PRINT DATA ==========

  Serial.print("LEFT:  x=");
  Serial.print(xLeft);
  Serial.print(", y=");
  Serial.print(yLeft);
  Serial.print(", btn=");
  Serial.print(bLeft);

  Serial.print("  |  RIGHT: x=");
  Serial.print(xRight);
  Serial.print(", y=");
  Serial.print(yRight);
  Serial.print(", btn=");
  Serial.println(bRight);

  // Small delay for readability (optional)
  delay(100);
}
