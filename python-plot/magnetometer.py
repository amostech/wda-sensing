#!/usr/bin/env python
# Plot a graph of Data which is comming in on the fly
# uses pylab
# Author: Norbert Feurle
# Date: 12.1.2012
# License: if you get any profit from this then please share it with me
import pylab
from pylab import *
import serial
import time
import threading
import random

ser = serial.Serial('/dev/cu.usbmodem1411', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,timeout=0)

xAchse=pylab.arange(0,100,1)
yAchse=pylab.array([0]*100)

fig = pylab.figure(1)
ax = fig.add_subplot(111)
ax.grid(True)
ax.set_title("Realtime Waveform Plot")
ax.set_xlabel("Time")
ax.set_ylabel("Amplitude")
ax.axis([0,100,-1.5,1.5])
line1=ax.plot(xAchse,yAchse,'-')

manager = pylab.get_current_fig_manager()

values=[]
values = [0 for x in range(100)]

Ta=0.01
fa=1.0/Ta
fcos=3.5

Konstant=cos(2*pi*fcos*Ta)
T0=1.0
T1=Konstant

def data_listener():
     while True:
        s = ser.readline()
        if s:
            lst = s.split(',')
            if len(lst) == 4:
                #print 'X:' + lst[0] + ',Y:' + lst[1] + ',Z:' + lst[2]
                if lst[0]:
                    values.append(float(lst[0]))

def RealtimePloter(arg):
  global values
  CurrentXAxis=pylab.arange(len(values)-100,len(values),1)
  line1[0].set_data(CurrentXAxis,pylab.array(values[-100:]))
  ax.axis([CurrentXAxis.min(),CurrentXAxis.max(),-1.5,1.5])
  manager.canvas.draw()

timer = fig.canvas.new_timer(interval=20)
timer.add_callback(RealtimePloter, ())

'''
timer2 = fig.canvas.new_timer(interval=20)
timer2.add_callback(SinwaveformGenerator, ())
timer.start()
timer2.start()
'''

thread = threading.Thread(target=data_listener)
thread.daemon = True
thread.start()

pylab.show()