clear all;
fileID = fopen ('~/../Положение стоя 1 мин - Шапель.txt','r');
Data = textscan(fileID,'%f,%f,%f,%f,%f,%f,%f,%f,%f,%f');

acc_L = [Data{1}, Data{2}, Data{3}];
gir_L = [Data{4}, Data{5}, Data{6}];
mag_L = [Data{7}, Data{8}, Data{9}];
time =  Data{10}/1000000;
fclose(fileID);
S(1,:)=acc_L(1,:);
F = eye(3);
H = eye(3);
rms = 8*10^-3;
Q = eye(3)*rms^2;
R = eye(3)*400*rms^2;
P(:,:,1) = zeros(3);

for i=2:1031
    S(i,:) = S(i-1,:)*F;
    P(:,:,i)=F*P(:,:,i-1)*transpose(F)+Q;
    K = P(:,:,i)*transpose(H)*inv(H*P(:,:,i)*transpose(H)+R);
    S(i,:) = S(i,:)+(acc_L(i,:)-S(i,:)*H)*K;
    P(:,:,i) = (eye(3)-K*H)*P(:,:,i);
end
figure
subplot (2,1,1);
plot(time,S(:,1), 'k', 'LineWidth' ,2);
subplot (2,1,2);
plot(time,acc_L(:,1), 'r');
figure;
hold on
plot(time,S(:,1), 'k', 'LineWidth' ,2);
plot(time,acc_L(:,1), 'r');
hold off