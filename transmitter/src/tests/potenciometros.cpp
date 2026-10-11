/*
 * ============================================================================
 * Test de dos potenciómetros (P1 -> A7, P2 -> A6)
 * ============================================================================
 *
 * DESCRIPTION:
 * This program reads and displays the values from two 10k potentiometers
 * connected to analog inputs. It shows raw ADC values (0-1023) and
 * calculated voltage (0-5V) for each potentiometer.
 *
 * HARDWARE REQUIREMENTS:
 * - Arduino Nano (or any Arduino with analog inputs)
 * - Two 10kΩ linear potentiometers
 * - Jumper wires
 * - USB cable for programming and Serial Monitor
 *
 * WIRING DIAGRAM:
 *
 *   Potentiometer 1 (P1):          Potentiometer 2 (P2):
 *   ┌─────────────┐                ┌─────────────┐
 *   │  1    2   3 │                │  1    2   3 │
 *   └──┬─────┬───┬─┘                └──┬─────┬───┬─┘
 *      │     │   │                     │     │   │
 *      │     │   └────────── GND       │     │   └────────── GND
 *      │     └────────────── A7        │     └────────────── A6
 *      └──────────────────── 5V        └──────────────────── 5V
 *
 *   Pin 1: VCC (5V)
 *   Pin 2: Signal (wiper) → Analog Input
 *   Pin 3: GND
 *
 * CONNECTIONS:
 * - P1 (Potentiometer 1):
 *   - Terminal 1 → 5V
 *   - Terminal 2 (center) → A7
 *   - Terminal 3 → GND
 *
 * - P2 (Potentiometer 2):
 *   - Terminal 1 → 5V
 *   - Terminal 2 (center) → A6
 *   - Terminal 3 → GND
 *
 * FEATURES:
 * - Real-time ADC value reading (0-1023)
 * - Voltage calculation (0.00V - 5.00V)
 * - Formatted serial output for easy monitoring
 * - Configurable sampling rate
 *
 * CREATED: September 17, 2026
 * AUTHOR: Domingo García
 * VERSION: 1.0
 * ============================================================================
 */


#include <Arduino.h>

// ============================================================================
// CONFIGURATION
// ============================================================================

// Analog pin assignments
const int PIN_P1 = A7;  // Potentiometer P1 connected to A7
const int PIN_P2 = A6;  // Potentiometer P2 connected to A6

// Serial communication speed (baud rate)
const long BAUD_RATE = 9600;

// Delay between readings in milliseconds
const unsigned long READ_DELAY = 200;


// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

int valueP1 = 0;  // Raw ADC value for P1 (0-1023)
int valueP2 = 0;  // Raw ADC value for P2 (0-1023)
float voltageP1 = 0.0;  // Calculated voltage for P1 (0-5V)
float voltageP2 = 0.0;  // Calculated voltage for P2 (0-5V)


// ============================================================================
// SETUP
// ============================================================================

void setup() {
  // Initialize serial communication with PC
  Serial.begin(BAUD_RATE);

  // Wait for Serial Monitor to connect (optional, remove if not needed)
  // while (!Serial) { ; }

  // Configure analog pins as inputs (default, but explicit for clarity)
  pinMode(PIN_P1, INPUT);
  pinMode(PIN_P2, INPUT);

  // Print initialization message
  Serial.println(F("============================================"));
  Serial.println(F("  Dual Potentiometer Tester - Arduino Nano"));
  Serial.println(F("============================================"));
  Serial.println(F(""));
  Serial.println(F("Hardware:"));
  Serial.println(F("  P1 → A7 (10kΩ potentiometer)"));
  Serial.println(F("  P2 → A6 (10kΩ potentiometer)"));
  Serial.println(F(""));
  Serial.println(F("Starting measurement..."));
  Serial.println(F(""));
}


// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  // -------------------------------------------------------------------------
  // Read analog values from potentiometers
  // -------------------------------------------------------------------------

  valueP1 = analogRead(PIN_P1);  // Read P1 (0-1023)
  valueP2 = analogRead(PIN_P2);  // Read P2 (0-1023)

  // -------------------------------------------------------------------------
  // Convert ADC values to voltage
  // Formula: Voltage = (ADC_value × 5.0) / 1023
  // -------------------------------------------------------------------------

  voltageP1 = (valueP1 * 5.0) / 1023.0;
  voltageP2 = (valueP2 * 5.0) / 1023.0;

  // -------------------------------------------------------------------------
  // Display results in Serial Monitor
  // -------------------------------------------------------------------------

  Serial.println(F("--- Potentiometer Readings ---"));

  // P1 output
  Serial.print(F("P1 (A7): "));
  Serial.print(valueP1);
  Serial.print(F(" (0-1023)"));
  Serial.print(F("  |  "));
  Serial.print(voltageP1, 2);  // 2 decimal places
  Serial.println(F(" V"));

  // P2 output
  Serial.print(F("P2 (A6): "));
  Serial.print(valueP2);
  Serial.print(F(" (0-1023)"));
  Serial.print(F("  |  "));
  Serial.print(voltageP2, 2);  // 2 decimal places
  Serial.println(F(" V"));

  // Separator line
  Serial.println(F("------------------------------"));
  Serial.println(F(""));

  // -------------------------------------------------------------------------
  // Wait before next reading
  // -------------------------------------------------------------------------

  delay(READ_DELAY);
}


// ============================================================================
// ADDITIONAL NOTES:
// ============================================================================
//
// TROUBLESHOOTING:
// - If readings are unstable, check connections and add 100nF capacitor
//   between signal pin and GND for each potentiometer
// - If values stay at 0 or 1023, verify wiring (VCC, GND, signal)
// - For 3.3V boards, change voltage calculation to use 3.3 instead of 5.0
//
// EXPECTED VALUES:
// - Potentiometer at minimum: ~0 (0V)
// - Potentiometer at center: ~512 (2.5V)
// - Potentiometer at maximum: ~1023 (5V)
//
// CUSTOMIZATION:
// - Change READ_DELAY to adjust update speed (lower = faster updates)
// - Add Serial.print() statements to log data for plotting
// - Use Arduino Serial Plotter (Tools → Serial Plotter) for visual graphs
//
// ============================================================================
