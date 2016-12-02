
from pyqtgraph.Qt import QtGui, QtCore
import numpy as np
import pyqtgraph as pg
from pyqtgraph.ptime import time
import serial

app = QtGui.QApplication([])

p = pg.plot()
#p.setRange(xRange=[0,100],yRange=[-800,800])
p.setWindowTitle('live plot from serial')
curve = p.plot()

data_mx = [0]
raw=serial.Serial('/dev/cu.usbmodem1411', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,timeout=0)


def update():
    global curve, data_mx
    line = raw.readline()
    if (line):
       line=line.split(",")
       if len(line)==5:#Included vector magnitude value
            data_mx.append(float(line[4]))
            if len(data_mx) > 100:
                data_mx = data_mx[1:]
            xdata = np.array(data_mx, dtype='float64')
            curve.setData(xdata)
            app.processEvents()

#def update():
#    global curve, data_mx
#    data_mx.append(float(np.random.random()))
#    if len(data_mx) > 100:
#        data_mx = data_mx[1:]
#    xdata = np.array(data_mx, dtype='float64')
#    curve.setData(xdata)
#    app.processEvents()

timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(0)

if __name__ == '__main__':
    import sys
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()
