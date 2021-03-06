f1 = '../experiment-1-distance-characterization/30dan10mph.csv'
f2 = '../experiment-1-distance-characterization/30dan20mph.csv'
f3 = '../experiment-1-distance-characterization/30dan30mph.csv'
%f4 = '../experiment-1-distance-characterization/powerdue/30raw10mph.csv'

figure;

[t1,x1,y1,z1,mf1] = loadData(f1);
subplot(3,1,1);
plot(t1,mf1);
%ylim([30 45])
title('Signal Strength 10mph - 30cm')

[t2,x2,y2,z2,mf2] = loadData(f2);
subplot(3,1,2);
t2_shifted = t2
plot(t2_shifted,mf2);
%ylim([30 45])
title('Signal Strength 20mph - 30cm')

[t3,x3,y3,z3,mf3] = loadData(f3);
subplot(3,1,3);
plot(t3,mf3);
%ylim([30 45])
title('Signal Strength 30mph - 30cm')

%[t4,x4,y4,z4,mf4] = loadData(f4);
%subplot(4,1,4);
%plot(t4,mf4);
%ylim([30 45])
%title('Signal Strength 10mph - 120cm')