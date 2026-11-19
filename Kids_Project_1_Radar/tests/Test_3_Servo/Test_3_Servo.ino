/*
  TEST 3 : THE SG90 SERVO MOTOR
  -----------------------------
  Goal: check that the motor turns and find its positions.

  Wiring:
    Red wire    -> 5V
    Brown wire  -> GND
    Yellow wire -> pin 10 (signal)

  How does it work? A servo does not spin continuously:
  you give it an ANGLE (from 0 to 180 degrees) and it moves there.

  Expected result: 0 deg -> 90 deg -> 180 deg -> 90 deg, in a loop.
  Tip: attach the small white arm (horn) to see the movement clearly.
  The servo shakes? Power is too weak: disconnect the other components.
*/

#include <Servo.h>   // Built-in Arduino library

Servo motor;

void setup() {
  Serial.begin(9600);
  motor.attach(10);   // The yellow wire is on pin 10
}

void loop() {
  motor.write(0);     Serial.println("0 degree");     delay(1500);
  motor.write(90);    Serial.println("90 degrees");   delay(1500);
  motor.write(180);   Serial.println("180 degrees");  delay(1500);
  motor.write(90);    Serial.println("90 degrees");   delay(1500);
}
