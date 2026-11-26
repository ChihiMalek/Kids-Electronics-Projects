/*
  TEST 2 : THE 9 LEDS
  -------------------
  Goal: check that all 9 LEDs are wired correctly.

  Wiring:
    LED 1 to LED 9 -> D10, D11, D12, D13, A5, A1, A2, A3, A4
    Each LED: pin -> 220 ohm resistor -> long leg (+) | short leg (-) -> GND

  Expected result: the LEDs turn ON one by one (1 -> 9),
  then all turn OFF, and it starts again.
  One LED stays OFF? It is probably reversed: flip it.
*/

const int NUM_LEDS = 9;
const int ledPins[NUM_LEDS] = {10, 11, 12, 13, A5, A1, A2, A3, A4};

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // Turn ON the LEDs one by one
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(300);
  }

  delay(1000);

  // Turn OFF all the LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  delay(1000);
}
