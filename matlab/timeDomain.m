function timeDomain( filename )
%ANALYZE Summary of this function goes here
%   Detailed explanation goes here

    [x,y,z] = loadData(filename);
    
    figure;
    
    subplot(3,1,1)
    plot(x)
    
    subplot(3,1,2)
    plot(y)
    
    subplot(3,1,3)
    plot(z)
    
    figure;
    scatter3(x,y,z,'filled')
    
end

