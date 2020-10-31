import smbus, time, struct, threading

#This script contains the class Wheel which contains information and functions so that the wheels can be controlled at a high level.
#The constructor requires the byte adress of the wheel and an smbus.SMBus object used for the I2C bus.
#    It also has two optional arguments (forwardDir and rightDir) which can be either 0 or 1.
#    These are just used to determine which direction is "forward" on the wheel motor and which direction turns the wheel to the right
#    What these need to be is really just trial and error for each wheel since it depends on how they were wired up
#
#    IMPORTANT: setDirection in the constructor is True by default. If it is set to True it will call the resetRotation function with the setDirection argument set to true.
#    resetRotation(True) tells the arduino to figure out which direction corresponds to a positive or negative encoder output
#    but the arduino must be connected AND on in order for this to do anything.
#    If the constructor is called before hooking up the arduino (in which case setDirection should be False) OR if the arduino is reset resetRotation(True) MUST BE CALLED AGAIN.
#    If resetRotation(True) is not called setRotation() and resetRotation() will have a 50% chance of going the correct direction.
#
#Functions this class contatins:
#rotate(revolutions, (optional)direction):
#    Rotates the wheel the specified number of rotations (can be positive or negative)
#    direction can be either "Forward" or "Backward". It just reverses the direction it rotates although I would recommend just making revolutions negative for reverse
#
#on((optional)direction):
#    Turns the wheel motor on.
#    By default direction is "Forward" but it can be set to "Backward" to move in reverse.
#
#off():
#    Turns the wheel motor off.
#
#getPosition():
#    Returns the wheels position in revolutions.
#
#turnWheel(deg, (optional)direction):
#    Turns the wheel the specified number of degrees
#    deg is the number of degrees to turn the wheel (can be positive or negative)
#    direction can be either "Right" or "Left". It just reverses the direction it rotates although I would recommend just making deg negative for left
#
#resetRotation((optional)setDirection):
#    Turns the wheel back to its starting location.
#    If setDirection is set to True it also tells the arduino to figure out which direction corresponds to a positive encoder output.
#    THIS MUST BE CALLED WITH setDirection = True BEFORE RUNNING THE NEXT TURNING FUCTIONS (If the arduino is reset since the constructor should already do this)
#
#setRotation(deg, (optional)direction):
#    Rotates the wheel to the specified number of degrees from the starting position.
#    deg is the degrees to rotate to (can be negative or positive).
#    direction can be "Left" or "Right", "Right" by default.
#
#getRotation():
#    Returns the rotation of the wheel in degrees from the starting position (Positive is to the right).


class Wheel:
    def __init__(this, address, bus, forwardDir = 0, rightDir = 0, setDirection = True):
        this.bus = bus
        this.address = address
        this.forwardDir = forwardDir
        this.rightDir = rightDir
        
        this.responseFailed = False
        this.wheelMotorResponded = False
        this.turnMotorResponded = False
        
        if setDirection:
            this.resetRotation(setDirection = True, responseFcn = lambda a: None)
        
    def rotate(this, revolutions, direction = "Forward", responseFcn = 0):
        lowerDir = this.forwardDir
        
        if revolutions < 0:
            lowerDir = not lowerDir
            revolutions *= -1
        
        if direction == "Backward":
            lowerDir = not lowerDir
            
        if revolutions == revolutions//1:
            revolutionsBase = list(revolutions.to_bytes(4, 'little'))
            revolutionsExponent = 0
        else:
            revolutionsBase = list(int(1000*revolutions).to_bytes(4, 'little'))
            revolutionsExponent = -3
        
        
        this.bus.write_i2c_block_data(this.address, 0x00, [len(revolutionsBase)] + revolutionsBase + [revolutionsExponent, lowerDir])
        
        threading.Thread(target = this.waitForResponse, args = (revolutions, responseFcn,)).start()
        
    def on(this, direction = "Forward"):
        
        lowerDir = this.forwardDir
        
        if direction == "Backward":
            lowerDir = not this.forwardDir
            
        this.bus.write_i2c_block_data(this.address, 0x02, [lowerDir])
        
    def off(this):
        this.bus.write_i2c_block_data(this.address, 0x03, [])
        
    def turnWheel(this, deg, direction = "Right", responseFcn = 0):
        lowerDir = this.rightDir
        
        if deg < 0:
            deg *= -1
            lowerDir = not lowerDir
        
        if direction == "Left":
            lowerDir = not lowerDir
            
        if deg == deg//1:
            degBase = list(deg.to_bytes(4, 'little'))
            degExp = 0
        else:
            degBase = list(int(1000*deg).to_bytes(4, 'little'))
            degExp = -3
            
        this.bus.write_i2c_block_data(this.address, 0x04, [len(degBase)] + degBase + [degExp, lowerDir])
        
        threading.Thread(target = this.waitForResponse, args = (deg*0.5, responseFcn,)).start()
        
    def resetRotation(this, setDirection = False, responseFcn = 0):
        if setDirection:
            this.bus.write_i2c_block_data(this.address, 0x04, [4] + [0, 0, 0, 255, 0, 1])
            time.sleep(.1)
            this.bus.write_i2c_block_data(this.address, 0x05, [])
        else:
            this.bus.write_i2c_block_data(this.address, 0x06, [])
            
        threading.Thread(target = this.waitForResponse, args = (20, responseFcn,)).start()
            
    def setRotation(this, deg, direction = "Right", responseFcn = 0):
        lowerDir = this.rightDir
        
        if deg < 0:
            deg *= -1
            lowerDir = not lowerDir
        
        if direction == "Left":
            lowerDir = not lowerDir
            
        if deg == deg//1:
            degBase = list(deg.to_bytes(4, 'little'))
            degExp = 0
        else:
            degBase = list(int(1000*deg).to_bytes(4, 'little'))
            degExp = -3
            
        this.bus.write_i2c_block_data(this.address, 0x07, [len(degBase)] + degBase + [degExp, lowerDir])
        
        threading.Thread(target = this.waitForResponse, args = (deg*.5, responseFcn,)).start()
        
    def waitForResponse(this, timeout = 2, finishFcn = 0):
        timestep = 0.05
        while True:
            try:
                data = this.bus.read_i2c_block_data(this.address, 0x09, 1)
                data = int.from_bytes(data, byteorder = 'little', signed = True)
                if data == 0 or data == 1:
                    break
            except:
                if isinstance(finishFcn, int):
                    this.responseFailed = True
                else:
                    finishFcn(True)
                return
                continue
            if(timeout <= 0):
                if isinstance(finishFcn, int):
                    this.responseFailed = True
                else:
                    finishFcn(True)
                return
            
            time.sleep(timestep)
            timeout -= timestep
        if isinstance(finishFcn, int):
            if data:
                this.wheelMotorResponded = True
            else:
                this.turnMotorResponded = True
        else:
            finishFcn(False)
        
        
        
    def getPosition(this):
        data = this.bus.read_i2c_block_data(this.address, 1, 4)
        
        data = int.from_bytes(data, byteorder='little', signed = True)/1000
        
        if not this.forwardDir:
            data *= -1
            
        return data
    
    def getRotation(this):
        data = this.bus.read_i2c_block_data(this.address, 8, 4)
        
        data = int.from_bytes(data, byteorder='little', signed = True)/1000
        
        if not this.rightDir:
            data *= -1
        
        return data
    
    def isWheelMotorDone(this):
        temp = this.wheelMotorResponded
        this.wheelMotorResponded = False
        return temp
    
    def isTurnMotorDone(this):
        temp = this.turnMotorResponded
        this.turnMotorResponded = False
        return temp
    
    def responseHasFailed(this):
        temp = this.responseFailed
        this.responseFailed = False
        return temp
test = Wheel(0x04, smbus.SMBus(1))
