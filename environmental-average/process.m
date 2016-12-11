clear;

%[t,x,y,z,mf] = loadData('../experiment-1-distance-characterization/60dan10mph.csv');
%filename = '../experiment-1-distance-characterization/60dan10mph.csv'
%filename = '../python-plot/REAL-CARS-SHORELINE.csv'
%filename = '../simulation-of-distance-with-racing-track/30counts_snr-1_20.csv'
%filename = '../python-plot/data-from-lab-test-random.csv'
%filename = '../wda-car-detect/wda-serial-out-averaging/trimmed-lsm-303-ycount-20-10cm.csv';
%filename = '../wda-car-detect/wda-serial-out-averaging/trimmed-lsm-303-ycount-20-10cm.csv'
%filename = 'overnight.csv'
%filename = '../simulation-of-distance-with-racing-track/20counts_snr-1_15.csv'
%filename = '../simulation-of-distance-with-racing-track/20counts_snr-2_19.csv'
%filename = '../python-plot/lsm-303-ycount-20-10cm.csv'
filename = '../python-plot/REAL-CARS-SHORELINE.csv'

[t,x,y,z,mf] = loadData(filename);
%t = t - 5000;
t = t / 1000
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
%plot(t,mf,'b',t,lpfilter20hz(mf),'r');
plot(t,mf,'b');
title('MF(uT) vs. Time(s)');

figure;
NFFT = 1024;
fs = 100;
X=fftshift(fft(mf,NFFT));	 	 
fVals=fs*(-NFFT/2:NFFT/2-1)/NFFT;	 	 
plot(fVals,abs(X),'b');
title('Double Sided FFT - with FFTShift');
xlabel('Frequency (Hz)')	 	 
ylabel('|DFT Values|');

figure;
%histogram(mf);

histfit(mf,18,'normal');
xlabel('Magnetic Field (uT)')
ylabel('Frequency of Occurrence')
title('Magnetic Field Idle - Facing North - 12h Long Overnight')

%cars = 50*ones(100000,1);
%mf = [ mf; cars ];

mn = mean(mf);
stdv = std(mf);
mad = mad(mf,1);
med = median(mf);

magic_mean = mad*1.4826; %BECAUSE GAUSSIAN -> NORMAL!!!!

mnlabel = sprintf('Mean -- %d', mn);
stdlabel = sprintf('Std Deviation -- %d', stdv);
%medianlabel = sprintf('MAD(interval) -- %d',(magic_mean) );

%%% Create the textbox
h = annotation('textbox',[0.58 0.75 0.1 0.1]);
set(h,'String',{mnlabel,stdlabel});
