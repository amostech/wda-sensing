clear;

fs=100;
f = 10;
nCyl = 5;
NFFT = 1024;

t = 0:1/fs:nCyl/f;
x = sin(2*pi*f*t);

y = awgn(x,2)

figure;
plot(t,y)

figure;
X=fftshift(fft(y,NFFT));	 	 
fVals=fs*(-NFFT/2:NFFT/2-1)/NFFT;	 	 
plot(fVals,abs(X),'b');	 	 
title('Double Sided FFT - with FFTShift');	 	 
xlabel('Frequency (Hz)')	 	 
ylabel('|DFT Values|');