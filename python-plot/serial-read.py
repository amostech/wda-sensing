import matplotlib.pyplot as plt
import time
import threading
import random
import serial

ser = serial.Serial('/dev/cu.usbmodem1411', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,timeout=0)

f = open('data.csv', 'w')
f.close()

while True:
    s = ser.readline()
    if s:
        #lst = s.split(',')
        #if len(lst) == 4:
        with open('data.csv','a') as f:
            f.write(s)
'''
if lst[0]:
f.write(lst[0])
if lst[1]:
f.write(lst[1])
if lst[2]:
f.write(lst[2])
''' 
