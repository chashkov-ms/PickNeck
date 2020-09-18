clear all;
close all;
name = 'Комплекс';
full_name = strcat('~/../',name,'.txt');
fileID = fopen (full_name,'r');
Data = textscan(fileID,'%f,%f,%f,%f,%f,%f,%f,%f,%f,%f');

acc_L = [Data{1}, Data{2}, Data{3}];
gir_L = [Data{4}, Data{5}, Data{6}];
mag_L = [Data{7}, Data{8}, Data{9}];
time =  Data{10}/1000000;
fclose(fileID);

FUSE = ahrsfilter;
FUSE.AccelerometerNoise         = 2.9247e-04;    %default
FUSE.MagnetometerNoise          = 0.1;           %default
FUSE.GyroscopeNoise             = 1.1385e-03;    %default
FUSE.LinearAccelerationNoise    = 9.6236e-06;    %default
FUSE.GyroscopeDriftNoise        = 3.0462e-13;    %default

[orient, ang_v] = FUSE(acc_L,gir_L,mag_L); %Kallman filtering
vertical_start = rotatepoint(orient(10), acc_L(10,:));

Len = size(acc_L);
for i = 1:Len(1)
    acc_G(i,:) = rotatepoint(orient(i), acc_L(i,:));    %Reshape accelerometr and
    vertical_now = rotatepoint(orient(i), acc_L(10,:));
    Y = norm(cross(vertical_start,vertical_now))/norm(vertical_start)/norm(vertical_now);
    X = dot(vertical_start,vertical_now)/norm(vertical_start)/norm(vertical_now);
    vert_angle(i,1) = rad2deg(angle(complex(X,Y)));     %вычисляем угол отклонения от вертикали    
end

Acceleration_x = figure('Name','Accelerometr X','NumberTitle','off');
subplot(1,2,1);
plot(time,acc_L(:,1), 'r');
grid on;
title ('Before Kalman');
xlabel({'Time','(in seconds)'});
ylabel('Magnitude');
ylim([min(min(acc_L(:,1)),min(acc_G(:,1))) max(max(acc_L(:,1)),max(acc_G(:,1)))]);

subplot(1,2,2);
plot(time,acc_G(:,1), 'k');
grid on;
title ('After Kalman');
xlabel({'Time','(in seconds)'});
ylabel('Magnitude');
ylim([min(min(acc_L(:,1)),min(acc_G(:,1))) max(max(acc_L(:,1)),max(acc_G(:,1)))]);
sgtitle({'Accelerometr X ';name});

Acceleration_y = figure('Name','Accelerometr Y','NumberTitle','off');
title ({'Accelerometr Y ';name});
subplot(1,2,1);
plot(time,acc_L(:,2), 'r');
grid on;
title ('Before Kalman');
xlabel({'Time','(in seconds)'});
ylabel('Magnitude');
ylim([min(min(acc_L(:,2)),min(acc_G(:,2))) max(max(acc_L(:,2)),max(acc_G(:,2)))]);

subplot(1,2,2);
plot(time,acc_G(:,2), 'k');
grid on;
title ('After Kalman');
xlabel({'Time','(in seconds)'});
ylabel('Magnitude');
ylim([min(min(acc_L(:,2)),min(acc_G(:,2))) max(max(acc_L(:,2)),max(acc_G(:,2)))]);
sgtitle({'Accelerometr Y ';name});

Acceleration_z = figure('Name','Accelerometr Z','NumberTitle','off');
title ({'Accelerometr Z ';name});
subplot(1,2,1);
plot(time,acc_L(:,3), 'r');
grid on;
title ('Before Kalman');
xlabel({'Time','(in seconds)'});
ylabel('Magnitude');
ylim([min(min(acc_L(:,3)),min(acc_G(:,3))) max(max(acc_L(:,3)),max(acc_G(:,3)))]);

subplot(1,2,2);
plot(time,acc_G(:,3), 'k');
grid on;
title ('After Kalman');
xlabel({'Time','(in seconds)'});
ylabel('Magnitude');
ylim([min(min(acc_L(:,3)),min(acc_G(:,3))) max(max(acc_L(:,3)),max(acc_G(:,3)))]);
sgtitle({'Accelerometr Z ';name});

Angle = figure('Name','Body angle','NumberTitle','off');
plot(time,vert_angle(:), 'r');
grid on;
title ({'Body angle';name});
