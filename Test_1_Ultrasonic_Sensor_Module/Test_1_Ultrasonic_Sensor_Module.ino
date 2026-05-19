float checkdistance() {
  digitalWrite(9, LOW);
  delayMicroseconds(2);
  digitalWrite(9, HIGH);
  delayMicroseconds(10);
  digitalWrite(10, LOW);
  float distance = pulseIn(10, HIGH) / 20.00;
  delay(3);
  return distance;
}
void Ultrasonic_Sensor_Module() {
  int Distance = 0;
  Distance = checkdistance();
  Serial.print("I can see:");
  Serial.print(Distance);
  Serial.println("CM");
  delay(5);
}
void setup(){
  Serial.begin(9400);
  pinMode(12, OUTPUT);
  pinMode(13, INPUT);
}
void loop(){
  Ultrasonic_Sensor_Module();

}
