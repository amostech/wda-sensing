import matplotlib.pyplot as plt
import time
import threading
import random
import serial
import numpy as np

ser = serial.Serial('/dev/cu.usbmodem1411', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,timeout=0)

s = ""

avgBufferSizes = np.arange(25,4025,25)
#tArr2 = np.arange(1,4,0.5)
#avgBufferSizes = [ 4000 ]

x = []
y = []
z = []

with open('trimmed-lsm-303-ycount-20-10cm.csv') as f:
	for line in f:
                arr = line.split(',')
                x.append(arr[1])
                y.append(arr[2])
                z.append(arr[3])

for buffsize in avgBufferSizes:
	while "AVG_BUFFSIZE_ACK" not in s:
		ser.write("AVG_BUFFSIZE_SET\n")
        	ser.write(str(buffsize) + "\n")
		s = ser.readline()
		print 'Waiting for AVG_BUFFSIZE_ACK(' + s + ')'
	
	print 'AVG_BUFFSIZE_ACK_ RECEIVED'	
	#NOW THAT WE SET THE BUFFER SIZE! GO FOR IT: RUN ENTIRE FILE!
	for i in range(buffsize):
                #WAIT for READ_REQ command
        	while "READ_REQUEST" not in s:
                	s = ser.readline()
                print "<< " + line + "\n"
                ser.write(x[i] + ",")
                ser.write(y[i] + ",")
                ser.write(z[i])
                ser.write('\n')
		#time.sleep(0.01)

        while "FINALAVG_ACK" not in s:
            ser.write("GET_FINALAVG\n")
            s = ser.readline()
            if s:
                print s
	
	f = open('avgaccuracy.dat', 'a')
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
