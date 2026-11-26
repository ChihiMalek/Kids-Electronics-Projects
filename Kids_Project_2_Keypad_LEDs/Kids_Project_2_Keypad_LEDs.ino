/*
  =====================================================
   KIDS PROJECT #2 : KEYPAD LED BAR + COUNTING GAME
  =====================================================

  How it works:
    FREE MODE (default)
      - Press 1 to 9 -> the same number of LEDs turn ON
      - Press 0      -> all LEDs OFF
      - Press *      -> "chaser" animation (a light runs left and right)
      - Press #      -> start the COUNTING GAME

    COUNTING GAME
      - The Arduino lights a random number of LEDs.
      - Count them and type your answer on the keypad.
      - Right answer -> all LEDs blink, score +1
      - Wrong answer -> the LEDs flash once, try again
      - Press # again to go back to FREE MODE

  The project can also be controlled from the PC with the Python
  interface (interface/keypad_web.py).

  Components:
    - 1 Arduino Uno board
    - 1 keypad 3x4 (membrane, 7 pins)
    - 9 LEDs + 9 x 220 ohm resistors (one per LED)
    - Jumper wires and a breadboard

  Wiring:
    Keypad (pins 1 to 7, from left to right) -> D2, D3, D4, D5, D6, D7, D8
    LED 1 to LED 9 -> D10, D11, D12, D13, A5, A1, A2, A3, A4
      (each LED: pin -> 220 ohm resistor -> long leg (+) | short leg (-) -> GND)

  Library needed: "Keypad" by Mark Stanley and Alexander Brevig
  (Arduino IDE > Library Manager > search "Keypad" > Install)

  Author: Malek Chihi
*/

#include <Keypad.h>

// ---------- Keypad ----------
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};   // Keypad pins 1, 2, 3, 4 (rows)
byte colPins[COLS] = {6, 7, 8};      // Keypad pins 5, 6, 7 (columns)

/*
  How does a matrix keypad work?
  The 12 keys are arranged in a grid of 4 rows x 3 columns.
  Pressing a key connects ONE row to ONE column.
  The library checks each row/column pair very fast to find
  which key is pressed. This way, 7 wires are enough for 12 keys.
*/
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------- LEDs ----------
const int NUM_LEDS = 9;
// Analog pins (A1-A5) can also be used as normal digital outputs
const int ledPins[NUM_LEDS] = {10, 11, 12, 13, A5, A1, A2, A3, A4};

// ---------- Game ----------
bool gameMode = false;   // false = free mode, true = counting game
int answer = 0;          // number of LEDs shown in the game
int score = 0;


/*
  Turn the LEDs ON/OFF with a "mask".
  A mask is a number where each bit is one LED:
    bit 0 = LED 1, bit 1 = LED 2, ... bit 8 = LED 9
  Example: mask = 5 = 000000101 in binary -> LED 1 and LED 3 are ON.
  The line "L:mask" is also sent to the PC for the Python interface.
*/
void setLeds(int mask) {
  for (int i = 0; i < NUM_LEDS; i++) {
    // (mask >> i) & 1  reads bit number i of the mask: 1 = ON, 0 = OFF
    digitalWrite(ledPins[i], (mask >> i) & 1);
  }
  Serial.print("L:");
  Serial.println(mask);
}

// Turn ON the first "count" LEDs
void showCount(int count) {
  // (1 << count) - 1 gives "count" ones in binary.
  // Example: count = 3 -> (1 << 3) - 1 = 8 - 1 = 7 = 000000111 -> 3 LEDs ON
  setLeds((1 << count) - 1);
}


// ---------- Animations ----------

// A light runs from LED 1 to LED 9, then comes back (2 times)
void chaserAnimation() {
  for (int repeat = 0; repeat < 2; repeat++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      setLeds(1 << i);
      delay(70);
    }
    for (int i = NUM_LEDS - 1; i >= 0; i--) {
      setLeds(1 << i);
      delay(70);
    }
  }
  setLeds(0);
}

// Right answer: all LEDs blink 2 times
void winAnimation() {
  for (int i = 0; i < 4; i++) {
    setLeds(i % 2 == 0 ? 511 : 0);   // 511 = 111111111 in binary = all 9 LEDs ON
    delay(180);
  }
  setLeds(0);
}

// Wrong answer: the LEDs flash once (long)
void loseAnimation() {
  setLeds(511);
  delay(500);
  setLeds(0);
  delay(250);
}


// ---------- Counting game ----------

void newQuestion() {
  delay(500);
  answer = random(1, 10);   // random number from 1 to 9
  showCount(answer);
  Serial.println("Q:How many LEDs?");
}

void startGame() {
  gameMode = true;
  score = 0;
  // micros() depends on the exact moment the player pressed #,
  // so the random numbers are different at every game
  randomSeed(micros());
  Serial.println("M:GAME");
  Serial.println("S:0");
  newQuestion();
}

void stopGame() {
  gameMode = false;
  Serial.println("M:FREE");
  setLeds(0);
}


// ---------- What to do when a key is pressed ----------
// Used by the keypad AND by the Python interface
void handleKey(char key) {
  Serial.print("K:");
  Serial.println(key);

  if (key == '#') {                 // Start / stop the game
    if (gameMode) stopGame();
    else startGame();
  }
  else if (key == '*') {            // Chaser animation
    chaserAnimation();
    if (gameMode) showCount(answer);   // show the question again
  }
  else if (key >= '0' && key <= '9') {
    /*
      key is a CHARACTER ('5'), not a number (5).
      Each character has a code: '0' = 48, '1' = 49, ... '5' = 53.
      So '5' - '0' = 53 - 48 = 5. This converts the character into a number.
    */
    int number = key - '0';

    if (gameMode) {
      if (number == answer) {       // Right answer
        score++;
        Serial.println("R:OK");
        Serial.print("S:");
        Serial.println(score);
        winAnimation();
        newQuestion();
      } else {                      // Wrong answer
        Serial.println("R:FAIL");
        loseAnimation();
        showCount(answer);          // show the same question again
      }
    } else {                        // Free mode
      showCount(number);
    }
  }
}


void setup() {
  Serial.begin(9600);

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  setLeds(0);
  Serial.println("M:FREE");
}


void loop() {
  // 1. A key pressed on the real keypad
  char key = keypad.getKey();
  if (key) handleKey(key);

  // 2. A key sent by the PC (Python interface or Serial Monitor)
  if (Serial.available() > 0) {
    char c = Serial.read();
    if ((c >= '0' && c <= '9') || c == '*' || c == '#') {
      handleKey(c);
    }
  }
}
