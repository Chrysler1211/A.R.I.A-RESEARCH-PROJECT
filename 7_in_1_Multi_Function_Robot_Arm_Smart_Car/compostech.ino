/*
 * Arduino Robot Diagnostic Test Code
 * 
 * Hardware:
 * - Arduino Uno
 * - Motor Driver Module (L298N or similar)
 * - Ultrasonic Sensor (HC-SR04)
 * - 6x AA batteries
 * 
 * This code tests:
 * 1. Motor forward/backward movement
 * 2. Ultrasonic sensor distance detection
 */

#include <Arduino.h>

// ============ PIN DEFINITIONS ============
// Ultrasonic Sensor Pins
#define TRIG_PIN 12
#define ECHO_PIN 11

// Motor Driver Pins (adjust these based on your actual connections)
// For L298N Motor Driver:
// Motor A (Left motor)
#define MOTOR_A_IN1 7
#define MOTOR_A_IN2 6
#define MOTOR_A_EN 5   // PWM pin for speed control

// Motor B (Right motor)
#define MOTOR_B_IN3 4
#define MOTOR_B_IN4 3
#define MOTOR_B_EN 9   // PWM pin for speed control

// ============ VARIABLES ============
int motorSpeed = 150;  // Speed value (0-255)

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  Serial.println("===== Arduino Robot Test Started =====");
  Serial.println();
  
  // Setup Ultrasonic Sensor Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Setup Motor Pins
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  pinMode(MOTOR_A_EN, OUTPUT);
  
  pinMode(MOTOR_B_IN3, OUTPUT);
  pinMode(MOTOR_B_IN4, OUTPUT);
  pinMode(MOTOR_B_EN, OUTPUT);
  
  // Initial state - motors stopped
  stopMotors();
  
  delay(2000);  // Wait 2 seconds before starting tests
}

void loop() {
  // ===== TEST 1: Ultrasonic Sensor =====
  Serial.println("----- Testing Ultrasonic Sensor -----");
  for (int i = 0; i < 5; i++) {
    long distance = getDistance();
    Serial.print("Distance reading ");
    Serial.print(i + 1);
    Serial.print(": ");
    
    if (distance > 0 && distance < 400) {
      Serial.print(distance);
      Serial.println(" cm");
    } else {
      Serial.println("OUT OF RANGE or ERROR");
    }
    delay(500);
  }
  Serial.println();
  
  // ===== TEST 2: Motor Forward Movement =====
  Serial.println("----- Testing Motors: FORWARD -----");
  moveForward();
  delay(2000);  // Move forward for 2 seconds
  stopMotors();
  delay(1000);  // Stop for 1 second
  
  // ===== TEST 3: Motor Backward Movement =====
  Serial.println("----- Testing Motors: BACKWARD -----");
  moveBackward();
  delay(2000);  // Move backward for 2 seconds
  stopMotors();
  delay(1000);  // Stop for 1 second
  
  Serial.println();
  Serial.println("===== Test Cycle Complete =====");
  Serial.println("Waiting 3 seconds before next cycle...");
  Serial.println();
  delay(3000);
}

// ============ ULTRASONIC SENSOR FUNCTION ============
long getDistance() {
  // Clear the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Send 10 microsecond pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Read the echo pin
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout after 30ms
  
  // Calculate distance in cm
  // Speed of sound = 343 m/s = 0.0343 cm/microsecond
  // Distance = (duration / 2) * 0.0343
  long distance = duration * 0.0343 / 2;
  
  return distance;
}

// ============ MOTOR CONTROL FUNCTIONS ============

void moveForward() {
  Serial.println("Motors: Moving FORWARD");
  
  // Motor A forward
  digitalWrite(MOTOR_A_IN1, HIGH);
  digitalWrite(MOTOR_A_IN2, LOW);
  analogWrite(MOTOR_A_EN, motorSpeed);
  
  // Motor B forward
  digitalWrite(MOTOR_B_IN3, HIGH);
  digitalWrite(MOTOR_B_IN4, LOW);
  analogWrite(MOTOR_B_EN, motorSpeed);
}

void moveBackward() {
  Serial.println("Motors: Moving BACKWARD");
  
  // Motor A backward
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, HIGH);
  analogWrite(MOTOR_A_EN, motorSpeed);
  
  // Motor B backward
  digitalWrite(MOTOR_B_IN3, LOW);
  digitalWrite(MOTOR_B_IN4, HIGH);
  analogWrite(MOTOR_B_EN, motorSpeed);
}

void turnLeft() {
  Serial.println("Motors: Turning LEFT");
  
  // Motor A backward
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, HIGH);
  analogWrite(MOTOR_A_EN, motorSpeed);
  
  // Motor B forward
  digitalWrite(MOTOR_B_IN3, HIGH);
  digitalWrite(MOTOR_B_IN4, LOW);
  analogWrite(MOTOR_B_EN, motorSpeed);
}

void turnRight() {
  Serial.println("Motors: Turning RIGHT");
  
  // Motor A forward
  digitalWrite(MOTOR_A_IN1, HIGH);
  digitalWrite(MOTOR_A_IN2, LOW);
  analogWrite(MOTOR_A_EN, motorSpeed);
  
  // Motor B backward
  digitalWrite(MOTOR_B_IN3, LOW);
  digitalWrite(MOTOR_B_IN4, HIGH);
  analogWrite(MOTOR_B_EN, motorSpeed);
}

void stopMotors() {
  Serial.println("Motors: STOPPED");
  
  // Stop Motor A
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, LOW);
  analogWrite(MOTOR_A_EN, 0);
  
  // Stop Motor B
  digitalWrite(MOTOR_B_IN3, LOW);
  digitalWrite(MOTOR_B_IN4, LOW);
  analogWrite(MOTOR_B_EN, 0);
}
