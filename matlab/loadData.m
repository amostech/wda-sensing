function [x,y,z] = processData( filename )
%PROCESSDATA Summary of this function goes here
%   Detailed explanation goes here

    data = importdata(filename,',');
    x = data(:,1)
    y = data(:,2)
    z = data(:,3)
end

