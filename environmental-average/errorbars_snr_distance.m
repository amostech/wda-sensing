mpu = [2.272727273	1.406779661	1.358490566	1.152173913	1];
arthur_phone = [2.25	1.392156863	1.314814815	1.039215686	1];
lsm_303 = [3.333333333	2.888888889	2.692307692	1.4568	1];

x = [30 60 90 120 150]

figure;
hold on;
plotwithbars(x, lsm_303, 0.25);
title('Signal to Noise Ratio for LSM-303C')
ylabel('SNR');
xlabel('Distance of car from Magnetometer in cm');

%plotwithbars(x, lsm_303, 0.25);
%plotwithbars(x, arthur_phone, 0.25);

