#include <Encoder.h>

#define enA 4
#define in1 5
#define in2 6
int rotDirection = 2;
Encoder myEnc(2,3);
long oldPosition  = -999;

void setup() {
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  setWheelSpeed(100);
  Serial.begin(9600);
}
void loop() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
  }
  int incomingCode = 0;
  if (Serial.available() > 0) {
    incomingCode = Serial.read();
    incomingCode -= 48;
    switch(incomingCode) {
      case 0:
        Serial.println("No change.");
        break;
      case 1:
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        rotDirection = 1;
        Serial.println("Motor changed to direction 1");
        break;
      case 2:
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        rotDirection = 2;
        Serial.println("Motor changed to direction 2");
        break;
      case 3:
        digitalWrite(enA, LOW);
        Serial.println("Engine stopped.");
        break;
      case 4:
        digitalWrite(enA, HIGH);
        Serial.println("Engine started.");
        break;
      case 5:
        Serial.print("Position: ");
        Serial.println(getPosition());
        Serial.print("Rotation: ");
        Serial.println(getDirection());
        break;
      default:
        Serial.println("Unknown code. Try 0 through 5.");
    }
    delay(500);
  }
}


int getPosition() {
  return oldPosition;
}

String getDirection() {
  String rotation = "Clockwise";
  if (rotDirection == 1) {
    rotation = "Counter Clockwise";
  }
  return rotation;
}

void setWheelSpeed(int x) {
  digitalWrite(enA, x);
}
