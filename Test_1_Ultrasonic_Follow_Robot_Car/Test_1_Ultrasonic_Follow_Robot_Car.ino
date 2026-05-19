#include <Servo.h>

/* ================= HARDWARE ================= */
Servo clawServo;
Servo armServo;
Servo baseServo;

// Motor pins
#define IN1 2
#define IN2 4
#define IN3 7
#define IN4 8
#define ENA 5
#define ENB 6

// Ultrasonic
#define TRIG_PIN 12
#define ECHO_PIN 13

// Servo pins
#define CLAW_PIN 9
#define ARM_PIN 10
#define BASE_PIN 11

/* ================= PARAMETERS ================= */
#define DETECT_DISTANCE 30
#define MIN_DISTANCE 3
#define STOP_DISTANCE 12          // INCREASED - Stop further back
#define COOLDOWN_TIME 3000        // REDUCED - Faster reset

// SERVO POSITIONS - ADJUST THESE FOR YOUR ROBOT!
// Test these values manually to find what works for YOUR robot
#define CLAW_OPEN 180            // Try: 0, 45, 90, 135, 180
#define CLAW_CLOSED 90           // Try: 0, 45, 90, 135, 180
#define CLAW_REST 135            // Middle position

#define ARM_DOWN 150             // Try: 0, 30, 60, 90, 120, 150, 180
#define ARM_UP 30                // Try: 0, 30, 60, 90, 120, 150, 180
#define ARM_REST 90              // Middle position

#define BASE_CENTER 90           // Forward-facing
#define BASE_DROP 150            // Turn to drop zone

int currentClawPos = CLAW_REST;
int currentArmPos = ARM_REST;
int currentBasePos = BASE_CENTER;

/* ================= STATE MACHINE ================= */
enum RobotState {
  SEARCHING,
  APPROACHING,
  VALIDATING,
  GRABBING,
  VERIFYING,
  DROPPING,
  COOLDOWN
};

RobotState currentState = SEARCHING;
unsigned long lastActionTime = 0;
unsigned long stateStartTime = 0;

/* ================= SETUP ================= */
void setup() {
  Serial.begin(9600);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Initialize servos in rest position
  Serial.println("Initializing servos...");
  attachServos();
  
  clawServo.write(CLAW_REST);
  delay(500);
  armServo.write(ARM_REST);
  delay(500);
  baseServo.write(BASE_CENTER);
  delay(500);
  
  detachServos();  // Detach to prevent interference
  
  stopCar();
  
  Serial.println("=== ROBOT READY ===");
  Serial.println("Servo positions:");
  Serial.print("  CLAW_OPEN = "); Serial.println(CLAW_OPEN);
  Serial.print("  CLAW_CLOSED = "); Serial.println(CLAW_CLOSED);
  Serial.print("  ARM_DOWN = "); Serial.println(ARM_DOWN);
  Serial.print("  ARM_UP = "); Serial.println(ARM_UP);
  Serial.println("Place litter 10-15cm in front");
  Serial.println();
}

/* ================= LOOP ================= */
void loop() {
  float distance = readDistance();
  
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 300) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" cm | State: ");
    printState(currentState);
    Serial.println();
    lastPrint = millis();
  }
  
  switch (currentState) {
    case SEARCHING:
      if (distance > MIN_DISTANCE && distance < DETECT_DISTANCE) {
        Serial.println(">>> OBJECT DETECTED!");
        stopCar();
        delay(500);
        currentState = APPROACHING;
        stateStartTime = millis();
      } else {
        moveForward(60);
      }
      break;
      
    case APPROACHING:
      if (distance > STOP_DISTANCE && distance < DETECT_DISTANCE) {
        moveForward(40);  // Slower approach
      } else if (distance <= STOP_DISTANCE && distance > MIN_DISTANCE) {
        Serial.println(">>> STOPPED at grab position");
        stopCar();
        delay(800);  // LONGER stabilization
        currentState = VALIDATING;
      } else {
        Serial.println("!!! Lost object");
        stopCar();
        delay(500);
        currentState = SEARCHING;
      }
      
      if (millis() - stateStartTime > 5000) {
        Serial.println("!!! Timeout");
        stopCar();
        currentState = SEARCHING;
      }
      break;
      
    case VALIDATING:
      delay(500);
      distance = readDistance();
      
      Serial.print(">>> Validating... ");
      Serial.print(distance);
      Serial.println(" cm");
      
      if (distance > MIN_DISTANCE && distance < STOP_DISTANCE + 10) {
        Serial.println(">>> VALIDATION OK - Starting grab");
        currentState = GRABBING;
      } else {
        Serial.println("!!! Validation failed");
        currentState = SEARCHING;
      }
      break;
      
    case GRABBING:
      grabLitter();
      currentState = VERIFYING;
      break;
      
    case VERIFYING:
      delay(800);
      distance = readDistance();
      
      Serial.print(">>> Verifying... Distance: ");
      Serial.println(distance);
      
      if (distance > 25) {
        Serial.println(">>> SUCCESS! Dropping...");
        currentState = DROPPING;
      } else {
        Serial.println("!!! Grab failed");
        releaseAndReset();
        lastActionTime = millis();
        currentState = COOLDOWN;
      }
      break;
      
    case DROPPING:
      dropLitter();
      lastActionTime = millis();
      currentState = COOLDOWN;
      break;
      
    case COOLDOWN:
      stopCar();
      if (millis() - lastActionTime > COOLDOWN_TIME) {
        Serial.println("=== COOLDOWN COMPLETE ===");
        Serial.println();
        currentState = SEARCHING;
      }
      break;
  }
  
  delay(100);
}

/* ================= SERVO MANAGEMENT ================= */
void attachServos() {
  if (!clawServo.attached()) clawServo.attach(CLAW_PIN);
  if (!armServo.attached()) armServo.attach(ARM_PIN);
  if (!baseServo.attached()) baseServo.attach(BASE_PIN);
}

void detachServos() {
  if (clawServo.attached()) clawServo.detach();
  if (armServo.attached()) armServo.detach();
  if (baseServo.attached()) baseServo.detach();
}

void safeServoMove(Servo &servo, int targetPos, int delayTime) {
  servo.write(targetPos);
  delay(delayTime);
}

/* ================= SENSOR ================= */
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 100;
  
  return duration / 58.0;
}

/* ================= GRAB SEQUENCE (FIXED) ================= */
void grabLitter() {
  Serial.println("=== GRAB SEQUENCE START ===");
  
  // Make SURE robot is stopped
  stopCar();
  delay(500);
  
  // Attach servos
  Serial.println("Step 1: Attaching servos");
  attachServos();
  delay(200);
  
  // Make sure base is centered (pointing forward)
  Serial.println("Step 2: Centering base");
  safeServoMove(baseServo, BASE_CENTER, 800);
  
  // Open claw FIRST
  Serial.println("Step 3: Opening claw WIDE");
  safeServoMove(clawServo, CLAW_OPEN, 1000);
  
  // Lower arm to ground level
  Serial.println("Step 4: Lowering arm to ground");
  safeServoMove(armServo, ARM_DOWN, 1500);
  
  // SMALL nudge forward to get litter between claw fingers
  Serial.println("Step 5: Small nudge forward");
  moveForward(30);
  delay(300);  // Move for 300ms
  stopCar();
  delay(800);  // Wait for stability
  
  // Close claw FIRMLY around object
  Serial.println("Step 6: CLOSING claw firmly");
  safeServoMove(clawServo, CLAW_CLOSED, 1500);
  
  // Small pause to ensure grip
  Serial.println("Step 7: Ensuring grip");
  delay(500);
  
  // Lift arm UP slowly
  Serial.println("Step 8: Lifting arm UP");
  safeServoMove(armServo, ARM_UP, 1500);
  
  // Extra pause with arm up
  delay(500);
  
  Serial.println("=== GRAB SEQUENCE COMPLETE ===");
  
  // Detach servos to stop seizuring
  detachServos();
}

/* ================= DROP SEQUENCE ================= */
void dropLitter() {
  Serial.println("=== DROP SEQUENCE START ===");
  
  attachServos();
  delay(200);
  
  Serial.println("Step 1: Rotating to drop zone");
  safeServoMove(baseServo, BASE_DROP, 1000);
  
  Serial.println("Step 2: Opening claw to release");
  safeServoMove(clawServo, CLAW_OPEN, 800);
  
  Serial.println("Step 3: Resetting all servos");
  safeServoMove(baseServo, BASE_CENTER, 600);
  safeServoMove(armServo, ARM_REST, 600);
  safeServoMove(clawServo, CLAW_REST, 600);
  
  Serial.println("=== DROP SEQUENCE COMPLETE ===");
  
  detachServos();
}

/* ================= RELEASE AND RESET ================= */
void releaseAndReset() {
  Serial.println("=== RELEASE AND RESET ===");
  
  attachServos();
  delay(200);
  
  safeServoMove(clawServo, CLAW_OPEN, 500);
  safeServoMove(armServo, ARM_REST, 800);
  safeServoMove(baseServo, BASE_CENTER, 800);
  safeServoMove(clawServo, CLAW_REST, 500);
  
  detachServos();
}

/* ================= MOTOR CONTROL ================= */
void moveForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, speed);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

/* ================= UTILITY ================= */
void printState(RobotState state) {
  switch(state) {
    case SEARCHING:   Serial.print("SEARCHING"); break;
    case APPROACHING: Serial.print("APPROACHING"); break;
    case VALIDATING:  Serial.print("VALIDATING"); break;
    case GRABBING:    Serial.print("GRABBING"); break;
    case VERIFYING:   Serial.print("VERIFYING"); break;
    case DROPPING:    Serial.print("DROPPING"); break;
    case COOLDOWN:    Serial.print("COOLDOWN"); break;
  }
}