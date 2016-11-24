function fft( filename, fs)
%FFT Summary of this function goes here
%   Detailed explanation goes here
    [x,y,z] = loadData(filename);
    figure;
    
    subplot(1,3,1)
    individualFFT(x,800)
    
    subplot(1,3,2)
    individualFFT(y,800)
    
    subplot(1,3,3)
    individualFFT(z,800)
    

end

function individualFFT(arr,fs)
    NFFT = 1024;
    L=length(arr);
    
    X=fft(arr,NFFT);	 	 
    Px=X.*conj(X)/(NFFT*L); %Power of each freq components	 	 
    fVals=fs*(0:NFFT/2-1)/NFFT;	 	 
    plot(fVals,10*log10(Px(1:NFFT/2)),'r','LineSmoothing','on','LineWidth',1);	 	 
    title('One Sided Power Spectral Density');	 	 
    xlabel('Frequency (Hz)')	 	 
    ylabel('PSD');
end