void setup(){
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(6, OUTPUT);
}

void loop()
{
  Move_Forward(70);//go forward for 2s
  delay(2000);
  Move_Backward(70);
  delay(2000);
  Rotate_Left(80);
  delay(2000);
  Rotate_Right(80);
  delay(2000);
  Stop();
  delay(2000);
}


void Move_Forward(int speed) //Define the forward function of the input speed
{
  digitalWrite(2,HIGH); //D2 digital I/O port controls the direction of the motor of interface A
  analogWrite(5,speed); //D5 digital I/O port outputs PWM signal to control the speed of the motor of port A.
  digitalWrite(4,LOW);//D4 digital I/O port controls the direction of the motor of interface B
  analogWrite(6,speed);//D6 digital I/O port outputs PWM signal to control the speed of interface B motor.
}

void Rotate_Left(int speed) //Define the left rotation function of the input speed
{
  digitalWrite(2,LOW);
  analogWrite(5,speed);
  digitalWrite(4,LOW);
  analogWrite(6,speed);
}

void Move_Backward(int speed) //Define the back function of the input speed
{
  digitalWrite(2,LOW);
  analogWrite(5,speed);
  digitalWrite(4,HIGH);
  analogWrite(6,speed);
}

void Stop() //Define stop function
{
  digitalWrite(2,LOW);
  analogWrite(5,0);
  digitalWrite(4,HIGH);
  analogWrite(6,0);
}

void Rotate_Right(int speed) //Define the right rotation function that can enter the speed
{
  digitalWrite(2,HIGH);
  analogWrite(5,speed);
  digitalWrite(4,HIGH);
  analogWrite(6,speed);
}
