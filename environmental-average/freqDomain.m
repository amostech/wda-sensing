function freqDomain( filename, fs)
%FFT Summary of this function goes here
%   Detailed explanation goes here
    [t,x,y,z,mf] = loadData(filename);
    figure;
    
    subplot(2,2,1)
    individualFFT(x,fs)
    
    subplot(2,2,2)
    individualFFT(y,fs)
    
    subplot(2,2,3)
    individualFFT(z,fs)
    
    subplot(2,2,4)
    individualFFT(mf,fs)
   

end

function individualFFT(arr,fs)
    NFFT = 1024;
    L=length(arr);
    
    X=fft(arr,NFFT);	 	 
    %Px=X.*conj(X)/(NFFT*L); %Power of each freq components	 	 
    fVals=fs*(0:NFFT/2-1)/NFFT;	 	 
    plot(fVals,abs(X),'r','LineSmoothing','on','LineWidth',1);	 	 
    title('One Sided Power Spectral Density');	 	 
    xlabel('Frequency (Hz)')	 	 
    ylabel('FFT');
end