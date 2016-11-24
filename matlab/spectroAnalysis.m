function spectroAnalysis( filename )
%FFT Summary of this function goes here
%   Detailed explanation goes here
    [x,y,z] = loadData(filename);
    figure;
    subplot(1,3,1)
    spectrogram(x)
    
    subplot(1,3,2)
    spectrogram(z)
    
    subplot(1,3,3)
    spectrogram(y)

end