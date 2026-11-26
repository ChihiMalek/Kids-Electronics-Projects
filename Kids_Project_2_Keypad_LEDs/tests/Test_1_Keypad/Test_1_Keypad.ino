/*
  TEST 1 : THE 3x4 KEYPAD
  -----------------------
  Goal: check that every key is detected correctly.

  Wiring: keypad pins 1 to 7 (from left to right) -> D2, D3, D4, D5, D6, D7, D8
  Library: "Keypad" (Library Manager > search "Keypad" > Install)

  Expected result (Serial Monitor, 9600 baud):
  each key you press is displayed, e.g. "Key pressed : 7".

  Wrong key displayed (e.g. you press 1 and see 5)?
  The pin order of your keypad is different: swap the numbers
  in rowPins / colPins until every key matches.
*/

#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  Serial.println("Keypad test: press any key");
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key pressed : ");
    Serial.println(key);
  }
}
