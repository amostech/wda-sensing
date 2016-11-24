function [t,x,y,z,mf] = loadData(filename)
%PROCESSDATA Summary of this function goes here
%   Detailed explanation goes here

    D = importdata(filename,',');
    
    t = D.data(:,1);
    x = D.data(:,2);
    y = D.data(:,3);
    z = D.data(:,4);
    mf = D.data(:,5);
end