from spacegrantmines.rotateXRevolutions.RotateTest import WheelController as wc
#do we need to have event checker things?

def main():
    test = wc.Wheel(0x04, smbus.SMBus(1), setDirection=True)#calibration test

    #setup
    #run loop (threading?)
    
    #The process
    #get all inputs
    #Get beacon direction
    #get the map/update the map
    #if way ahead is clear, turn towards beacon and move forward
    #otherwise, update the map with new obstacle and path find a way around the obstacle to the beacon direction



def turn_wheel(int wheel, float angle):

def rotate_wheel(int wheel, float distance):

def turn_robot(float angle):

def move_robot(float distance):

def get_beacon_angle():

def get_heading():

def get_beacon_direction():

def get_laser_dist():

def update_map():#we want to be near the center of the map

main()
