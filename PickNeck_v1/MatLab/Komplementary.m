clear all;
close all;
name = 'Приседания 10 - Шапель';
full_name = strcat('~/../',name,'.txt');
fileID = fopen (full_name,'r');
Data = textscan(fileID,'%f,%f,%f,%f,%f,%f,%f,%f,%f,%f');

acc_L = [Data{1}, Data{2}, Data{3}];
gir_L = [Data{4}, Data{5}, Data{6}];
mag_L = [Data{7}, Data{8}, Data{9}];
time =  Data{10}/1000000;
fclose(fileID);
G_G = [0;0;-9.807];
Len = size(acc_L);
K=[0.01*eye(3)];

angle = acos(dot(G_G,acc_L(10,:)')/norm(G_G)/norm(acc_L(10,:)));
vect = cross(G_G, acc_L(10,:)');
vect = vect/norm(vect)*sin(angle/2);
Q_GL = quaternion(cos(angle/2),vect(1),vect(2), vect(3));
P_GL = rotmat(Q_GL, 'point');
clear angle

orient(1)=quaternion(1,0,0,0);

for i = 2:Len(1)
    DeltaT = time(i)-time(i-1);
    W(i,:) = gir_L(i,:);
    orient(i) =  quaternion(cos(norm(W(i,:))/2*DeltaT),W(i,1)/norm(W(i,:))*sin(norm(W(i,:))/2*DeltaT),...
            W(i,2)/norm(W(i,:))*sin(norm(W(i,:))/2*DeltaT),W(i,3)/norm(W(i,:))*sin(norm(W(i,:))/2*DeltaT))*orient(i-1);
        
    rot = rotatepoint(orient(i),G_G');
    matr = [0, -rot(3), rot(2);rot(3), 0, -rot(1); -rot(2), rot(1), 0];
    H = P_GL*matr;
    H = [H, zeros(3)];
    x_err = [W(i,1)*DeltaT,W(i,2)*DeltaT,W(i,3)*DeltaT,B(i,1),B(i,2),B(i,3)]';
    r = H*x_err;
    x_err = K*r
    
    if norm(x_err(1:3)) < 1
        orient(i) =  quaternion(sqrt(1-norm(x_err(1:3))^2),x_err(1)/2,x_err(2)/2,x_err(3)/2);
    else
        orient(i) =  sqrt(1+norm(x_err(1:3))^2)*quaternion(1,x_err(1)/2,x_err(2)/2,x_err(3)/2);
    end
    orient(i) =  quaternion(sqrt(1-norm(x_err(1:3))^2),x_err(1)/2,x_err(2)/2,x_err(3)/2);
    B(i,:) = B(i,:) + x_err(4:6)';
    W(i,:) = W(i,:) - B(i,:);
end

vertical_start = rotatepoint(orient(10), acc_L(10,:));

for i = 1:Len(1)
    acc_G(i,:) = rotatepoint(orient(i), acc_L(i,:));    %Reshape accelerometr and
    vertical_now = rotatepoint(orient(i), acc_L(10,:));
    Y = norm(cross(vertical_start,vertical_now))/norm(vertical_start)/norm(vertical_now);
    X = dot(vertical_start,vertical_now)/norm(vertical_start)/norm(vertical_now);
    vert_angle(i,1) = rad2deg(angle(complex(X,Y)));     %вычисляем угол отклонения от вертикали    
end

%*************************************************************************
%************Plot Grafics*************************************************

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
