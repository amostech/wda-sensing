import matplotlib.pyplot as plt
import time
import threading
import random
import serial
import numpy as np

ser = serial.Serial('/dev/cu.usbmodem1411', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,timeout=0)

s = ""

#tArr1 = [1,2,6,6.25,6.5,6,75,7,7.5,8,8.5,9,10]
#tArr2 = [1,2,2.25,2.5,2.75,3,3.25,3.5,3.75,4]

tArr1 = np.arange(1,10,0.5)
tArr2 = np.arange(1,4,0.5)

x = []
y = []
z = []

with open('../../simulation-of-distance-with-racing-track/30counts_snr-1_20.csv') as f:
	for line in f:
                arr = line.split(',')
                x.append(arr[0])
                y.append(arr[1])
                z.append(arr[2])

curr = 0
while "CALIBRATION_ACK" not in s:
	s = ser.readline()
	if s:
		print s
        if "READ_REQUEST" in s:
		ser.write(x[curr] + ",")
        	ser.write(y[curr] + ",")
        	ser.write(z[curr])
       		ser.write('\n')
        	curr += 1

for t1 in tArr1:
    for t2 in tArr2:
	if t2 > t1:
		continue
	while "THRESHOLD_ACK" not in s:
		ser.write("THRESHOLD_SET\n")
        	ser.write(str(t1) + "," + str(t2)+"\n")
		s = ser.readline()
		print 'Waiting for THRESHOLD_ACK(' + s + ')'
	
	print 'THRESHOLD_ACK RECEIVED'	
	#NOW THAT WE CALIBRATED! GO FOR IT!
	for i in range(len(x)):
                #WAIT for READ_REQ command
                while "READ_REQUEST" not in s:
                    s = ser.readline()
                print "<< " + line + "\n"
                ser.write(x[i] + ",")
                ser.write(y[i] + ",")
                ser.write(z[i])
                ser.write('\n')
		#time.sleep(0.01)


        while "COUNT_ACK" not in s:
            ser.write("GET_COUNT\n")
            s = ser.readline()
            if s:
                print s
	
	f = open('accuracy.dat', 'a')
	f.write(s)
	 
        #raw_input("Press Enter to continue... Experiment Result: " + s )
	

        
'''
while True:
    #s = ser.readline()
    ser.write()
    if s:
        #lst = s.split(',')
        #if len(lst) == 4:
        with open('data.csv','a') as f:
            f.write(s)
if lst[0]:
f.write(lst[0])
if lst[1]:
f.write(lst[1])
if lst[2]:
f.write(lst[2])
''' 
