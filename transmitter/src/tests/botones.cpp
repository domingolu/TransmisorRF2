/*
 * Test de 4 botones con antirrebote (ezButton), pines D5-D8
 *
 * Este código de Arduino fue desarrollado por es.newbiely.com
 * Este código de Arduino se proporciona al público sin ninguna restricción.
 * Para tutoriales completos y diagramas de cableado, visite:
 * https://es.newbiely.com/tutorials/arduino/arduino-button-debounce
 *
 * Hardware: 4 botones entre D5/D6/D7/D8 y GND (con resistencia
 * pull-up externa, o cambiar a INPUT_PULLUP según tu cableado).
 */

#include <Arduino.h>
#include <ezButton.h>

ezButton button1(5);  // create ezButton object that attach to pin 5;
ezButton button2(6);  // create ezButton object that attach to pin 6;
ezButton button3(7);  // create ezButton object that attach to pin 7;
ezButton button4(8);  // create ezButton object that attach to pin 8;


void setup() {
  Serial.begin(9600);
  button1.setDebounceTime(50); // set debounce time to 50 milliseconds
  button2.setDebounceTime(50); // set debounce time to 50 milliseconds
  button3.setDebounceTime(50); // set debounce time to 50 milliseconds
  button4.setDebounceTime(50); // set debounce time to 50 milliseconds

}

void loop() {
  button1.loop(); // MUST call the loop() function first
  button2.loop(); // MUST call the loop() function first
  button3.loop(); // MUST call the loop() function first
  button4.loop(); // MUST call the loop() function first


  if(button1.isPressed())
    Serial.println("The button 1 is pressed");

  if(button1.isReleased())
    Serial.println("The button 1 is released");

  if(button2.isPressed())
    Serial.println("The button 2 is pressed");

  if(button2.isReleased())
    Serial.println("The button 2 is released");

  if(button3.isPressed())
    Serial.println("The button 3 is pressed");

  if(button3.isReleased())
    Serial.println("The button 3 is released");

  if(button4.isPressed())
    Serial.println("The button 4 is pressed");

  if(button4.isReleased())
    Serial.println("The button 4 is released");
}
