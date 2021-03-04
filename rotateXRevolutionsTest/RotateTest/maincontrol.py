#import WheelController as wc
import time
import numpy as np
#do we need to have event checker things?

def main():
    #test = wc.Wheel(0x04, smbus.SMBus(1), setDirection=True)#calibration test
    #setup
    #run loop (threading?)

    #State machine to choose what to do
    #While loop to execute current state
    #The process
    #get all inputs
    #Get beacon direction
    #get the map/update the map
    #if way ahead is clear, turn towards beacon and move forward
    #otherwise, update the map with new obstacle and path find a way around the obstacle to the beacon direction


    #MESSING AROUND WITH CODE
    
    buffer_dim = 1000 #size of buffer
    frame_dim = 10 #size of viewing and pathing frame
    map_buffer = np.zeros((buffer_dim,buffer_dim)) #make a board
    frame_min = (int)(buffer_dim/2 - frame_dim/2) #viewing frame dimensions
    frame_max = (int)(buffer_dim/2 + frame_dim/2)
    print (map_buffer[frame_min:frame_max,frame_min:frame_max]) #prints viewing frame
    
    run = True
    initial_time = time.clock()
    elapsed_time = 0
    previous_time = 0
    while run:
        #we need a way to keep track of position
        #initially, I'll set speed to be 1, so time is the measure of distance
        #we'll do a tile per second
        previous_time = elapsed_time;
        elapsed_time = int(time.clock() - initial_time);


        if previous_time != elapsed_time:
            previous_time = elapsed_time
            print(elapsed_time)





            
def turn_wheel(wheel, angle):
    #do nothing
    print("hello")
def rotate_wheel(wheel, distance):
    #do nothing
    print("workkkkk")
def turn_robot(angle):
    #do nothing
    print("workkkkk")
def move_robot(distance):
    #do nothing
    print("workkkkk")
def get_beacon_angle():
    #do nothing
    print("workkkkk")
def get_heading():
    #do nothing
    print("workkkkk")
def get_beacon_direction():
    #do nothing
    print("workkkkk")
def get_laser_dist():
    #do nothing
    print("workkkkk")
def update_map():#we want to be near the center of the map
    #do nothing
    print("workkkkk")
main()
