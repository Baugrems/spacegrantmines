#include <Encoder.h>

#define enA 4
#define in1 5
#define in2 6
int rotDirection = 0;
Encoder myEnc(2,3);
long oldPosition  = -999;

void setup() {
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(enA, HIGH);
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}
void loop() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }
}
