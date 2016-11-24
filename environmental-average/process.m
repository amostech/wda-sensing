clear;

[t,x,y,z,mf] = loadData('overnight.csv');

figure;

subplot(2,2,1);
plot(t,x);
title('X-Axis(uT) vs. Time(s)');

subplot(2,2,2);
plot(t,y);
title('Y-Axis(uT) vs. Time(s)');

subplot(2,2,3);
plot(t,z);
title('Z-Axis(uT) vs. Time(s)');

subplot(2,2,4);
plot(t,mf);
title('MF(uT) vs. Time(s)');

figure;
%histogram(mf);
histfit(mf,100,'normal');
xlabel('Magnetic Field (uT)')
ylabel('Frequency of Occurrence')
title('Magnetic Field Idle - Facing North - 12h Long Overnight')

mn = mean(mf);
stdv = std(mf);

mnlabel = sprintf('Mean -- %d', mn);   
stdlabel = sprintf('Std Deviation -- %d', stdv);

%%% Create the textbox
h = annotation('textbox',[0.58 0.75 0.1 0.1]);
set(h,'String',{mnlabel,stdlabel});
