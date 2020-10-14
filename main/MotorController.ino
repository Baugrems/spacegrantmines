#include "MotorController.h"

//Initializes the motor pins. Should be called in setup
void initMotors(){
  pinMode(DRIVE_ENCODER_A, INPUT);
  pinMode(DRIVE_ENCODER_B, INPUT);
  pinMode(DRIVE_MOTOR_1, OUTPUT);
  pinMode(DRIVE_MOTOR_2, OUTPUT);
  pinMode(DRIVE_MOTOR_PWM, OUTPUT);

  pinMode(SERVO_ENCODER_A, INPUT);
  pinMode(SERVO_ENCODER_B, INPUT);
  pinMode(SERVO_MOTOR_1, OUTPUT);
  pinMode(SERVO_MOTOR_2, OUTPUT);
  pinMode(SERVO_MOTOR_PWM, OUTPUT);
}

//Rotates a specified number of times if motor == DRIVE and if motor == SERVO revolutions is in degrees instead.
void rotate(Encoder *enc, uint8_t motor, float revolutions, uint8_t direction, float speed){
  if(motor == DRIVE){
    analogWrite(DRIVE_MOTOR_PWM, (int)(speed*255));
    if(direction){
      digitalWrite(DRIVE_MOTOR_1, HIGH);
      digitalWrite(DRIVE_MOTOR_2, LOW);
    }else{
      digitalWrite(DRIVE_MOTOR_1, LOW);
      digitalWrite(DRIVE_MOTOR_2, HIGH);
    }
    driveStopTick = revolutions*DRIVE_MOTOR_TICKS_PER_REVOLUTION;
    driveStartTick = enc->read();
  }else{
    //Account for gear ratio and convert from degrees to revolutions
    revolutions /= SERVO_GEAR_RATIO;
    revolutions /= 360;

    //If the degrees to rotate is negative, swap the direction it moves in.
    if(revolutions < 0){
      direction = !direction;
      revolutions *= -1;
    }
    
    analogWrite(SERVO_MOTOR_PWM, (int)(speed*255));
    if(direction){
      digitalWrite(SERVO_MOTOR_1, HIGH);
      digitalWrite(SERVO_MOTOR_2, LOW);
    }else{
      digitalWrite(SERVO_MOTOR_1, LOW);
      digitalWrite(SERVO_MOTOR_2, HIGH);
    }
    servoStopTick = revolutions*SERVO_MOTOR_TICKS_PER_REVOLUTION;
    servoStartTick = enc->read();
  }
}

void checkStopCondition(Encoder *driveEnc, Encoder *servoEnc){
  if(driveStopTick && abs(driveStartTick -driveEnc->read()) > driveStopTick){
    digitalWrite(DRIVE_MOTOR_1, LOW);
    digitalWrite(DRIVE_MOTOR_2, LOW);
    analogWrite(DRIVE_MOTOR_PWM, 255);

    driveStopTick = 0;
  }

  if(servoStopTick && abs(servoStartTick - servoEnc->read()) > servoStopTick){
    digitalWrite(DRIVE_MOTOR_1, LOW);
    digitalWrite(DRIVE_MOTOR_2, LOW);
    analogWrite(DRIVE_MOTOR_PWM, 255);

    servoStopTick = 0;
  }
}
