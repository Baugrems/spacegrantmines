/*  Arduino DC Motor Control - PWM | H-Bridge | L298N  -  Example 01
    by Dejan Nedelkovski, www.HowToMechatronics.com
*/
#define enA 4
#define in1 2
#define in2 3
int rotDirection = 0;
void setup() {
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(enA, HIGH);
}
void loop() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(2000);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  delay(2000);
}
