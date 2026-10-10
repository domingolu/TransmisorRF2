/*
 * Joystick.h - Library for reading analog joysticks with button
 *
 * This library separates joystick reading logic from the main program.
 * Supports reading X/Y analog values and button state with debouncing.
 *
 * Created based on ArduinoGetStarted.com example
 * Modified for modular architecture by Domingo García
 * Modified to header-only implementation
 */

#ifndef Joystick_h
#define Joystick_h

#include <Arduino.h>
#include <ezButton.h>

class Joystick {
  private:
    int _vrxPin;      // VRX pin number
    int _vryPin;      // VRY pin number
    ezButton* _button; // Pointer to button object
    int _xValue;      // Cached X value
    int _yValue;      // Cached Y value

  public:
    // Constructor: initialize joystick with pin numbers
    Joystick(int vrxPin, int vryPin, int swPin) {
      _vrxPin = vrxPin;
      _vryPin = vryPin;
      _button = new ezButton(swPin);
      _xValue = 0;
      _yValue = 0;
    }

    // Initialize the joystick (call in setup)
    void begin() {
      // ezButton is initialized in constructor
      _button->setDebounceTime(50); // Default debounce time
    }

    // Update joystick state (call in loop)
    void loop() {
      _button->loop(); // Update button state

      // Read analog values
      _xValue = analogRead(_vrxPin);
      _yValue = analogRead(_vryPin);
    }

    // Read current X axis value (0-1023)
    int readX() {
      return _xValue;
    }

    // Read current Y axis value (0-1023)
    int readY() {
      return _yValue;
    }

    // Check if button is pressed (returns true once per press)
    bool isPressed() {
      return _button->isPressed();
    }

    // Check if button is released (returns true once per release)
    bool isReleased() {
      return _button->isReleased();
    }

    // Get raw button state (0 or 1)
    int getButtonState() {
      return _button->getState();
    }

    // Set debounce time in milliseconds (default: 50ms)
    void setDebounceTime(unsigned long time) {
      _button->setDebounceTime(time);
    }
};

#endif
