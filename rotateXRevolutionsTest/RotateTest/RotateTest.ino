/*
 * This is a test for a function designed to rotate the motor x rotations
 * When rotate is called it turns on the motor in the direction specified and sets two global variables:
 *  startTick is the pulse number reported by enc.read() that the motors began moving at.
 *  stopTick is how many pulses need to pass before the motor is stopped.
 * 
 * It uses the main loop to determine when to stop the motor since I wanted the arduino free to do other stuff while the motor is running.
 *  I dont really like this solution but cant think of anything else besides using the interrupts from the encoder to check the stopping condition but I feel like that would use up too much of the arduinos processing time.
 *  If someone has a better solution that would be great
 *  
 *  This was tested on the larger motor. The smaller motor probably has a different number of pulses for each revolution but I cant test that cause I think my motors encoder is broken.
 *  
 *  setting the speed dosent currently work and I'm not sure why I'll fix that later.
 */

#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#define encoderA 2
#define encoderB 3
#define motorIn1 4
#define motorIn2 5
#define motorPWM 6

//Amount of encoder pulses per revolution
//This number is experimentally found and probably not accurate but I cant find a specs sheet on the encoder for the big motor so this is what im working with
#define ticksPerRevolution 10200

Encoder enc(encoderA, encoderB);

long startTick = 0;
long stopTick = 0;

void rotate(float revolutions, uint8_t direction = 1, float speed = 1.0){
  analogWrite(motorPWM, (int)(speed*255));
  if(direction){
    digitalWrite(motorIn1, HIGH);
    digitalWrite(motorIn2, LOW);
  }else{
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, HIGH);
  }
  stopTick = revolutions*ticksPerRevolution;
  startTick = enc.read();
}

void setup() {
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorPWM, OUTPUT);
  Serial.begin(9600);

  rotate(1, 0, 0.5);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(stopTick && abs(startTick - enc.read()) > stopTick){
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, LOW);
    analogWrite(motorPWM, 255);

    stopTick = 0;
  }
}
