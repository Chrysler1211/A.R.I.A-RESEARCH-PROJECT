
#include <Servo.h>

int claw_degress;
int arm_degress;
int base_degress;

Servo myservo1;//servo of claw
Servo myservo2;//servo of arm
Servo myservo3;//servo of base

void setup(){
  claw_degress = 90;//initialize the angle value of claw servo 1
  arm_degress = 135;//initialize the angle value of arm servo 2
  base_degress = 90;//initialize the angle value of base servo 3
  myservo1.attach(9);//claw Servo 1 is connected to D9
  myservo2.attach(10);//arm Servo 2 is connected to D10
  myservo3.attach(11);//base Servo 3 is connected to D11
}
void loop(){
  myservo1.write(claw_degress);//make claw servo 1 rotate to 90°
  delay(200);
  myservo2.write(arm_degress);//make arm servo 2 rotate to 135°
  delay(200);
  myservo3.write(base_degress);//make base servo 3 rotate to 90°
  delay(200);
}
