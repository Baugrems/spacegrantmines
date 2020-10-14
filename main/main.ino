#define ENCODER_OPTIMIZE_INTERRUPTS

#define DRIVE 0
#define SERVO 1

#define DRIVE_ENCODER_A 7
#define DRIVE_ENCODER_B 3
#define DRIVE_MOTOR_1 4
#define DRIVE_MOTOR_2 5
#define DRIVE_MOTOR_PWM 6
#define DRIVE_MOTOR_TICKS_PER_REVOLUTION 10200

#define SERVO_ENCODER_A 7
#define SERVO_ENCODER_B 3
#define SERVO_MOTOR_1 4
#define SERVO_MOTOR_2 5
#define SERVO_MOTOR_PWM 6
#define SERVO_MOTOR_TICKS_PER_REVOLUTION 10200
#define SERVO_GEAR_RATIO 26.5936/464.10877 //From CAD

#include <Encoder.h>
#include "MotorController.h"

Encoder driveEncoder(DRIVE_ENCODER_A, DRIVE_ENCODER_B);
Encoder servoEncoder(SERVO_ENCODER_A, SERVO_ENCODER_B);

void setup() {
  // put your setup code here, to run once:
  initMotors();
  rotate(&servoEncoder, SERVO, 10);
}

void loop() {
  // put your main code here, to run repeatedly:
  checkStopCondition(&driveEncoder, &servoEncoder);
}
