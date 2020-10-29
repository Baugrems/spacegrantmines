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
 *  
 *  UPDATE: Added functionality to get I2C data in and control the motor via that
 *  
 */

#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <Wire.h>
#include <math.h>

#define encoderA 2
#define encoderB 3
#define motorIn1 4
#define motorIn2 5
#define motorPWM 6

#define servoEncoderA 2
#define servoEncoderB 3
#define servoIn1 4
#define servoIn2 5
#define servoPWM 6

#define ADDRESS 4

//Amount of encoder pulses per revolution
//This number is experimentally found and probably not accurate but I cant find a specs sheet on the encoder for the big motor so this is what im working with
#define ticksPerRevolution 10200
#define servoTicksPerRevolution 10200
#define servoGearRatio 26.5936/464.10877 //From CAD

Encoder* enc = new Encoder(encoderA, encoderB);
Encoder* servoEnc = enc;// = new Encoder(servoEncoderA, servoEncoderB);

//startTick and stopTick are used to figure out when the motor started turning and when it should stop
long startTick = 0;
long stopTick = 0;

long servoStartTick = 0;
long servoStopTick = 0;

long requestQueue;

//This is used to know what information the pi is requesting when it gets a request 
int8_t requestType = -1;

bool turnDirection;

//Function rotates the drive motor a specified number of revolutions
void rotate(float revolutions, bool direction = 0, float speed = 1.0){
  analogWrite(motorPWM, (int)(speed*255));
  if(direction){
    digitalWrite(motorIn1, HIGH);
    digitalWrite(motorIn2, LOW);
  }else{
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, HIGH);
  }

  //stopTick is how many ticks need to pass to stop the motor
  //startTick is the position the encoder was at when it started moving
  stopTick = revolutions*ticksPerRevolution;
  startTick = enc->read();
}

//Extremely similar to the rotate function except it takes in degrees instead of revolutions and calculates how many revolutions the motor needs to turn to get the wheel to turn that amount of degrees
void turnWheel(float deg, bool direction = 0){
  analogWrite(servoPWM, 255);
  if(direction){
    digitalWrite(servoIn1, HIGH);
    digitalWrite(servoIn2, LOW);
  }else{
    digitalWrite(servoIn1, LOW);
    digitalWrite(servoIn2, HIGH);
  }

  servoStopTick = deg*servoTicksPerRevolution/(servoGearRatio*360);
  servoStartTick = servoEnc->read();
}

void setup() {
  //Init pins
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorPWM, OUTPUT);
  pinMode(servoIn1, OUTPUT);
  pinMode(servoIn2, OUTPUT);
  pinMode(servoPWM, OUTPUT);

  //Join I2C with ADDRESS
  Wire.begin(ADDRESS);

  //Set the receive and request events
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //Start serial bus
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  //Stop the drive motor if enough ticks have passed
  if(stopTick && abs(startTick - enc->read()) > abs(stopTick)){
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, LOW);
    analogWrite(motorPWM, 255);

    stopTick = 0;
  }

  //Stop the servo motor if enough ticks have passed
  if(servoStopTick && abs(servoStartTick - servoEnc->read()) > abs(servoStopTick)){
    digitalWrite(servoIn1, LOW);
    digitalWrite(servoIn2, LOW);
    analogWrite(servoPWM, 255);

    servoStopTick = 0;
  }

  //Serial.println(requestQueue);
}

//Made this a function because all of this was repeated in the turn wheel and drive wheel functions. Already made the comments and didnt feel like changing them or variable names so pretend this is in the Rotate Wheel function of the recieveEvent where its called
void getFloatAndDir(int numBytes, byte* input, float* outputFloat, bool* outputBool){
  //Set the default values for amount of revolutions and direction if not specified by input
  *outputFloat = 1;
  *outputBool = 0;

  //I2C cannot transmit floats/doubles (at least without some pre and post processing) so they have to be converted into an int and an exponent of 10
  //revolutionsBase is the int and revolutionsExponent is the exponent of 10, so the final float is revolutionsBase*10^revolutionsExponent
  //It would have been easier to just get a refrence to the float, cast int* to the pointer and derefrence it to get an integer and send that int over but python cant do that so we have to deal with this method.
  int revolutionsBase;
  int8_t revolutionsExponent;
  if(numBytes > 1){ //Make sure message included the amount of revolutions
    byte intIn[4]; //An int is 4 bytes so make an array of those 4 bytes

    //The sender may have sent the base in more or less than 4 bytes so we transpose those bytes into the new array with max length of 4
    //The first byte specifies how many bytes the int is stored in. Anything beyond 4 bytes will be truncated but I doubt were ever gonna tell it to rotate 2147483.647 times
    uint8_t intInLength = input[0]; 
    for(uint8_t i = 0; i < min(4, intInLength); i++){
      intIn[i] = input[1 + i];
    }

    //Convert the byte array to an int
    revolutionsBase = *(int*)intIn;
    revolutionsExponent = 0;

    if(numBytes >= intInLength + 3){ //If enough arguments exist, the last one should be the direction and should be the last argument so set the direction to that
      *outputBool = (bool)input[intInLength + 2];
    }
    if(numBytes >= intInLength + 2){ //If enough arguments exist, this one should be the exponent for the revolutionsBase
      revolutionsExponent = input[intInLength + 1];
    }

    //Swap direction if the revolutions is negative
    if(revolutionsBase < 0) {
      *outputBool = !(*outputBool);
      revolutionsBase *= -1;
    }

    //Calculate the amount of revolutions as a float
    *outputFloat = (float)revolutionsBase*pow(10, revolutionsExponent);
  }
}

//Called whenever the Pi sends something over I2C
//numBytes is how many bytes were transmitted
void receiveEvent(int numBytes){
  //Create an array of all the bytes the Pi sent over
  //first is the first byte the Pi sent over, which is the command, and everything in the array is extra information needed
  byte input[numBytes - 1];
  byte first = Wire.read();

  if(first == 1){
    requestQueue = (long)(1000*enc->read()/ticksPerRevolution);
    return;
  }else if(first == 8){
    requestQueue = (long)(1000*servoEnc->read()*360*servoGearRatio/servoTicksPerRevolution);
  }
  for(int i = 0; Wire.available(); i++){
    input[i] = Wire.read();
  }

  //This is basically just a switch statement for each command the Pi could send over
  //I realize this should be a switch statement but for whatever reason doing it as one refused to work so its an if ... else if ... statement
  //Command number : command
  //  0 : Rotate drive wheel
  //  1 : Request drive motor position in revolutions
  //  2 : Turn drive motor on until stop command is sent
  //  3 : Turn drive motor off
  //  4 : Turn the wheel the specified amount of degrees
  if(first == 0){ //Rotate Wheel
    // Arguments should be in <Amount of bytes in base int, base int(should be 4 bytes), exponent, direction> format exponent and direction are optional and will be set to 0 if not set

    float revolutions;
    bool direction;
    getFloatAndDir(numBytes, input, &revolutions, &direction);

    Serial.println(revolutions);
    
    //Call the rotate function
    rotate(revolutions, direction);
  }else if(first == 1){ //Request position
    requestQueue = (int)(1000*enc->read()/ticksPerRevolution); //Set the request type, the request function should automatically be run immediately
  }else if(first == 2){ //On
    //Only argument for this is the direction (0 or 1) and is optional
    bool dir = 0;

    //If direction was specified set the direction to that
    if(numBytes > 1) dir = input[0];

    //Turn the motor on
    analogWrite(motorPWM, 255);
    if(dir){
      digitalWrite(motorIn1, HIGH);
      digitalWrite(motorIn2, LOW);
    }else{
      digitalWrite(motorIn1, LOW);
      digitalWrite(motorIn2, HIGH);
    }
  }else if(first == 3){ //Off
    //Turn the drive motor off (no arguments needed)
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, LOW);
  }else if(first == 4){//Turn wheel (Basically same thing as the rotate function but runs a different function at the end)
    // Arguments should be in <Amount of bytes in base int, base int(should be 4 bytes), exponent, direction> format exponent and direction are optional and will be set to 0 if not set

    float revolutions;
    bool direction;
    getFloatAndDir(numBytes, input, &revolutions, &direction);
    
    //Call the rotate function
    turnWheel(revolutions, direction);
  }else if(first == 5){ //Return to position 0 and calibrate the encoder
    //Figure out which direction corresponds to positive or negative encoder output
    long initialPos = servoEnc->read();
    digitalWrite(servoIn1, HIGH);
    digitalWrite(servoIn2, LOW);
    delay(100);
    turnDirection = servoEnc->read() > initialPos;
    digitalWrite(servoIn1, LOW);
    if(servoEnc->read() <= initialPos + 1 and servoEnc->read() >= initialPos - 1){
      Serial.println("Warning: Encoder output between -1 and 1. There is probably an error.");
    }

    //Return back to position 0
    if(turnDirection == servoEnc->read() > 0){
      digitalWrite(servoIn2, HIGH);
    }else{
      digitalWrite(servoIn1, HIGH);
    }
    servoStartTick = servoEnc->read();
    servoStopTick = servoEnc->read();
  }else if(first == 6){ //Return to position 0 without calibrating the encoder
    if(servoEnc->read() <= 1 && servoEnc->read() >= -1){
      return;
    }

    if(servoEnc->read() > 0 == turnDirection){
      digitalWrite(servoIn1, LOW);
      digitalWrite(servoIn2, HIGH);
    }else{
      digitalWrite(servoIn1, HIGH);
      digitalWrite(servoIn2, LOW);
    }

    servoStartTick = servoEnc->read();
    servoStopTick = servoEnc->read();
  }else if(first == 7){ //Rotate to a specified number of degrees

    float revolutions;
    bool direction;
    getFloatAndDir(numBytes, input, &revolutions, &direction);
    
    if(servoEnc->read() > (2*(direction) - 1)*revolutions*servoTicksPerRevolution/(servoGearRatio*360) == turnDirection){
      digitalWrite(servoIn1, LOW);
      digitalWrite(servoIn2, HIGH);
    }else{
      digitalWrite(servoIn1, HIGH);
      digitalWrite(servoIn2, LOW);
    }
    
    servoStartTick = servoEnc->read();
    servoStopTick = abs(servoEnc->read() - (2*(direction) - 1)*revolutions*servoTicksPerRevolution/(servoGearRatio*360));

    
  }else{//Function not found
    Serial.print("No function: ");
    Serial.println(first);
  }
}

//Called when the Pi requests data
//requestType should have been set in the recieveEvent function just before this is called
void requestEvent(){
  Wire.write((byte*)&requestQueue, sizeof(requestQueue));
}
