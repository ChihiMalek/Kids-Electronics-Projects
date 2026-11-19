/*
  TEST 1 : THE ARDUINO BOARD
  --------------------------
  Goal: check that the board receives the code and works.
  Wiring: nothing, just the USB cable.

  Expected result:
    - The small "L" LED on the board (pin 13) blinks.
    - The Serial Monitor (9600 baud) shows "Test number : 1, 2, 3..."

  Upload error "not in sync"? Check the board
  (Tools > Board > Arduino Uno) and the port (Tools > Port > COMx).
*/

int counter = 0;

void setup() {
  Serial.begin(9600);      // Open communication with the PC
  pinMode(13, OUTPUT);     // The "L" LED is connected to pin 13
  Serial.println("The Arduino board works!");
}

void loop() {
  counter++;
  Serial.print("Test number : ");
  Serial.println(counter);

  digitalWrite(13, HIGH);  // LED ON
  delay(500);
  digitalWrite(13, LOW);   // LED OFF
  delay(500);
}
