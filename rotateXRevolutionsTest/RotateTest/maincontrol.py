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
    buffer_dim = 10 #size of buffer
    #frame_dim = 10 #size of viewing and pathing frame
    value_map = np.zeros((buffer_dim,buffer_dim)) #make a board, a map of values that indicate how good a position is
    field_map = np.zeros((buffer_dim,buffer_dim))

    #frame_min = (int)(buffer_dim/2 - frame_dim/2) #viewing frame dimensions
    #frame_max = (int)(buffer_dim/2 + frame_dim/2)
    #print (map_buffer[frame_min:frame_max,frame_min:frame_max]) #prints viewing frame

    #Distance is in robot lengths because the robot can only pass through a square 1 robot wide
    beacon_x = 0
    beacon_y = 0
    value_map[beacon_x][beacon_y] = 100#must be larger than twice the array size since the initial values are zeros
    recursive_map_update(value_map, field_map, beacon_x, beacon_y)
    print(value_map)
    
    #init_pos = (500,500)
    #move    
    run = False
    while run:
        #we need a way to keep track of position
        #initially, I'll set speed to be 1, so time is the measure of distance
        #we'll do a tile per second
        previous_time = elapsed_time;
        elapsed_time = int(time.clock() - initial_time);


        if previous_time != elapsed_time:
            previous_time = elapsed_time
            print(elapsed_time)




def shift_map_update(field_map, beacon_dir):
    print("stuff")
    #remove column add column...
            
def recursive_map_update(value_map, field_map, x, y):
    if x + 1 < len(value_map) and value_map[x][y] - 1 > value_map[x + 1][y]:#if it exists and hasnt been updated
        value_map[x + 1][y] = value_map[x][y] - 1
        recursive_map_update(value_map, field_map, x + 1, y)
        
    if y + 1 < len(value_map) and value_map[x][y] - 1 > value_map[x][y + 1]:#if it exists and hasnt been updated
        value_map[x][y + 1] = value_map[x][y] - 1
        recursive_map_update(value_map, field_map, x, y + 1)
        
    if x - 1 >= 0 and value_map[x][y] - 1> value_map[x - 1][y]:#if it exists and hasnt been updated
        value_map[x - 1][y] = value_map[x][y] - 1
        recursive_map_update(value_map, field_map, x - 1, y)
        
    if y - 1 >= 0 and value_map[x][y] - 1 > value_map[x][y - 1]:#if it exists and hasnt been updated
        value_map[x][y - 1] = value_map[x][y] - 1
        recursive_map_update(value_map, field_map, x, y - 1)

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
    #distance in wheel rotations and angle in 45 degree increments
    #update current position based on direction moved
    #update map (ie delete and add rows)

    #move in small increments and check if there is a collision
        #if there is a collision/interrupt add an x to a location and reroute
    
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
