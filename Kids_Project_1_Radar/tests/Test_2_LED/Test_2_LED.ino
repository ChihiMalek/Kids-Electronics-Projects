/*
  TEST 2 : THE LEDS
  -----------------
  Goal: check that both LEDs are wired correctly.

  Wiring:
    Pin 5 -> 220 ohm resistor -> LONG leg (+) of the red LED
    Pin 6 -> 220 ohm resistor -> LONG leg (+) of the green LED
    SHORT legs (-) of both LEDs -> GND

  Why a resistor? It limits the current.
  Without it, the LED or the Arduino pin can be damaged.

  Expected result: red and green turn ON one after the other.
  One LED stays OFF? It is probably reversed: flip it.
*/

const int RED_LED   = 5;
const int GREEN_LED = 6;

void setup() {
  pinMode(RED_LED, OUTPUT);     // OUTPUT = Arduino sends current
  pinMode(GREEN_LED, OUTPUT);
}

void loop() {
  digitalWrite(RED_LED, HIGH);    // HIGH = 5V = LED ON
  digitalWrite(GREEN_LED, LOW);   // LOW  = 0V = LED OFF
  delay(1000);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(1000);
}
