/*
  TEST 4 : THE HC-SR04 ULTRASONIC SENSOR
  --------------------------------------
  Goal: check that the sensor measures distances correctly.

  Wiring:
    VCC  -> 5V
    GND  -> GND
    Trig -> pin 7
    Echo -> pin 8

  How does it work?
    1. Send a pulse on Trig: the sensor emits an ultrasound.
    2. The sound bounces off the obstacle and comes back (the echo).
    3. Echo stays HIGH during the whole round trip of the sound.
    4. distance = time x speed of sound / 2  (divided by 2: there + back)

  Expected result (Serial Monitor, 9600 baud):
  move your hand closer -> the distance decreases.
  Always "Nothing detected"? Check Trig/Echo (swapped?) and the 5V.
*/

const int TRIG_PIN = 7;
const int ECHO_PIN = 8;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);   // We SEND the command to the sensor
  pinMode(ECHO_PIN, INPUT);    // We RECEIVE the answer from the sensor
}

void loop() {
  // 10 microsecond pulse: "sensor, send a sound!"
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Round-trip time in microseconds (0 if no echo within 30 ms)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    Serial.println("Nothing detected");
  } else {
    // Sound travels 0.0343 cm per microsecond
    int distance = duration * 0.0343 / 2;
    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  delay(300);
}
