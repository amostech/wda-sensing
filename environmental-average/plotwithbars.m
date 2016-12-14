function [] = plotwithbars( x, y, epsilon )
%PLOTWITHBARS Summary of this function goes here
%   Detailed explanation goes here
    x_ref = [ x(1) x(length(x)) ];
    y_ref = [ y(1) y(length(y)) ];
    upper_y = y_ref + epsilon;
    lower_y = y_ref - epsilon;
    plot(x,y, '-ob', x_ref, y_ref, '--',x_ref, upper_y,'-.', x_ref, lower_y, '-.', 'LineWidth', 2);
    legend('Measured SNR','Theoretical Estimation',strcat('Upper Limit +\epsilon=',num2str(epsilon)),strcat('Lower Limit -\epsilon=',num2str(epsilon)))
    set(gca,'fontsize',18)
end

