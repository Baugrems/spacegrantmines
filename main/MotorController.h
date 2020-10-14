#ifndef DRIVE_MOTOR_CONTROLLER
#define DRIVE_MOTOR_CONTROLLER

long driveStartTick = 0;
long driveStopTick = 0;

long servoStartTick = 0;
long servoStopTick = 0;

void initMotors();

void rotate(Encoder *enc, uint8_t motor, float revolutions, uint8_t direction = 1, float speed = 1.0);

void checkStopCondition(Encoder *driveEnc, Encoder *servoEnc);

#endif
