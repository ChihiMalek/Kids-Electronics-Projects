/*
  =====================================================
   KIDS PROJECT #1 : ARDUINO RADAR
  =====================================================

  How it works:
  An ultrasonic sensor mounted on a servo motor sweeps
  from left to right, like a real radar.
    - Nothing in front of the sensor -> green LED ON
    - An obstacle too close          -> red LED ON

  Components:
    - 1 Arduino Uno board
    - 1 SG90 servo motor
    - 1 HC-SR04 ultrasonic sensor
    - 1 red LED + 1 green LED
    - 2 x 220 ohm resistors (they protect the LEDs)
    - Jumper wires and a breadboard

  Wiring:
    HC-SR04 : VCC -> 5V | GND -> GND | Trig -> 7 | Echo -> 8
    Servo   : Red -> 5V | Brown -> GND | Yellow -> 10
    Red LED   : pin 5 -> resistor -> long leg (+) | short leg (-) -> GND
    Green LED : pin 6 -> resistor -> long leg (+) | short leg (-) -> GND

  Author: Malek Chihi
*/

#include <Servo.h>   // Built-in Arduino library to control a servo

// ---------- Pins ----------
const int TRIG_PIN  = 7;    // The sensor sends the sound
const int ECHO_PIN  = 8;    // The sensor receives the echo
const int SERVO_PIN = 10;   // The motor
const int RED_LED   = 5;
const int GREEN_LED = 6;

// ---------- Settings ----------
const int ALERT_DISTANCE = 20;   // In cm: below this value, the red LED turns ON

Servo motor;   // Create a "motor" object to send it commands


/*
  Measure the distance (in cm)

  How does it work?
  The sensor sends a very high-pitched sound (ultrasound, we cannot hear it).
  The sound bounces off the obstacle and comes back: this is the ECHO.
  We measure the round-trip time, then calculate the distance.

  The calculation:
    - Sound travels at 0.0343 cm per microsecond (343 m/s).
    - distance = time x speed
    - We divide by 2, because the sound goes THERE and BACK.
*/
int measureDistance() {
  // 1. Send a 10 microsecond pulse on TRIG:
  //    this tells the sensor "send a sound!"
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 2. pulseIn() measures how long the ECHO pin stays HIGH.
  //    This time = the round trip of the sound (in microseconds).
  //    30000 = wait no longer than 30 ms (after that: nothing detected).
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // 3. If duration = 0, no echo came back: nothing in front.
  if (duration == 0) return 0;

  // 4. Calculate the distance.
  return duration * 0.0343 / 2;
}


void setup() {
  Serial.begin(9600);   // To display measurements in the Serial Monitor

  pinMode(TRIG_PIN, OUTPUT);   // Arduino SENDS a signal to the sensor
  pinMode(ECHO_PIN, INPUT);    // Arduino RECEIVES the signal from the sensor
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  motor.attach(SERVO_PIN);   // Tell the library which pin the motor uses
  motor.write(10);           // Start position: 10 degrees
  delay(1000);               // Give the motor time to get there
}


void loop() {
  // The motor turns from 10 to 170 degrees, one degree at a time...
  for (int angle = 10; angle <= 170; angle++) {
    scan(angle);
  }

  // ...then comes back from 170 to 10 degrees.
  for (int angle = 170; angle >= 10; angle--) {
    scan(angle);
  }
}


// One radar step: rotate, measure, turn on the right LED
void scan(int angle) {
  motor.write(angle);   // Move the motor to this angle
  delay(30);            // Short pause: the motor needs time to move

  int distance = measureDistance();

  // distance > 0  : an echo was received
  // distance < 20 : the obstacle is too close
  if (distance > 0 && distance < ALERT_DISTANCE) {
    digitalWrite(RED_LED, HIGH);    // Danger: red
    digitalWrite(GREEN_LED, LOW);
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);  // All clear: green
  }

  // Display the result in the Serial Monitor (9600 baud)
  Serial.print("Angle : ");
  Serial.print(angle);
  Serial.print("  |  Distance : ");
  Serial.print(distance);
  Serial.println(" cm");
}
