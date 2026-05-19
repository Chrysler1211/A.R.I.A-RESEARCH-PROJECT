#include <Servo.h>

/* ================== SERVO ================== */
Servo clawServo;
Servo armServo;

#define CLAW_PIN 9
#define ARM_PIN 10

#define CLAW_OPEN 60
#define CLAW_CLOSE 140

#define ARM_DOWN 60
#define ARM_UP 130

/* ================== MOTOR ================== */
#define IN1 2
#define IN2 4
#define ENA 5
#define IN3 7
#define IN4 8
#define ENB 6

/* ================== ULTRASONIC ================== */
#define TRIG_PIN 12
#define ECHO_PIN 13

#define DETECT_DISTANCE 20   // cm

/* ================== VARIABLES ================== */
char bt;
int speedCar = 80;
bool objectDetected = false;

/* ================== SETUP ================== */
void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  clawServo.attach(CLAW_PIN);
  armServo.attach(ARM_PIN);

  clawServo.write(CLAW_OPEN);
  armServo.write(ARM_DOWN);

  stopCar();
}

/* ================== LOOP ================== */
void loop() {
  automatedMovement();
  bluetoothControl();
}

/* ================== AUTOMATED MOVEMENT ================== */
void automatedMovement() {
  float dist = getDistance();

  if (dist > 0 && dist < DETECT_DISTANCE) {
    stopCar();
    objectDetected = true;
  } else {
    moveForward(speedCar);
    objectDetected = false;
  }
}

/* ================== BLUETOOTH CONTROL ================== */
void bluetoothControl() {
  if (!Serial.available()) return;

  bt = Serial.read();

  switch (bt) {
    case 'o': clawServo.write(CLAW_OPEN); break;
    case 'c': clawServo.write(CLAW_CLOSE); break;
    case 'u': armServo.write(ARM_UP); break;
    case 'd': armServo.write(ARM_DOWN); break;

    case 'x': speedCar = 60; break;
    case 'y': speedCar = 90; break;
    case 'z': speedCar = 120; break;

    case 's': stopCar(); break;
  }
}

/* ================== ULTRASONIC ================== */
float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 100;
  return duration / 58.0;
}

/* ================== MOTOR ================== */
void moveForward(int spd) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, spd);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, spd);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

