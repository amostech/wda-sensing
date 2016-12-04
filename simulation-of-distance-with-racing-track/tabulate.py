from collections import defaultdict
import numpy as np
f = open("snr-151-steps05.dat")
th2 = np.linspace(1.0, 3.50, 6)
print 0.00,
for i in th2:
    print i,
last = 0.0
d = defaultdict(list)
for l in f:
    if l:
        lst = l.rstrip().split(",")
        #[th1.index(float(lst[1]))][th2.index(float(lst[2]))] = lst[3]
        if float(lst[1]) == last:
            print lst[3],
        else :
            last = float(lst[1])
            print ''
            print lst[1], lst[3],
