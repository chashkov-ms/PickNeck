clear all;
fileID = fopen ('~/../Положение стоя 1 мин - Шапель.txt','r');
Data = textscan(fileID,'%f,%f,%f,%f,%f,%f,%f,%f,%f,%f');

acc_L = [Data{1}, Data{2}, Data{3}];
gir_L = [Data{4}, Data{5}, Data{6}];
mag_L = [Data{7}, Data{8}, Data{9}];
time =  Data{10}/1000000;
fclose(fileID);
G_G = [0;0;-9.807];

S(1,:)=acc_L(1,:);
F = eye(3);
H = eye(3);
rms = 8*10^-3;
Q = eye(3)*rms^2;
R = eye(3)*400*rms^2;
P(:,:,1) = zeros(3);

for i=2:10
    S(i,:) = S(i-1,:)*F;
    P(:,:,i)=F*P(:,:,i-1)*F'+Q;
    K = P(:,:,i)*H'*inv(H*P(:,:,i)*H'+R);
    S(i,:) = S(i,:)+(acc_L(i,:)-S(i,:)*H)*K;
    P(:,:,i) = (eye(3)-K*H)*P(:,:,i);
end


clear F H Q R P;
angle = acos(dot(G_G,S(10,:)')/norm(G_G)/norm(S(10,:)));
vect = cross(G_G, S(10,:)');
vect = vect/norm(vect)*sin(angle/2);
Q_GL = quaternion(cos(angle/2),vect(1),vect(2), vect(3));
P_GL = rotmat(Q_GL, 'point');
clear angle vect Q_GL

W(1,:)=gir_L(1,:);
B(1,1:3)=0;
q(1) =  quaternion(cos(norm(W(1,:))/2*0),W(1,1)/norm(W(1,:))*sin(norm(W(1,:))/2*0),...
        W(1,2)/norm(W(1,:))*sin(norm(W(1,:))/2*0), W(1,3)/norm(W(1,:))*sin(norm(W(1,:))/2*0));

rms_r = 8*10^-3;
rms_w = 9*10^-4;
Q_11 = eye(3)*rms_r^2;
Q_12 = zeros(3);
Q_21 = Q_12';
Q_22 = eye(3)*rms_w^2;
Q = [Q_11,Q_12;Q_21,Q_22];
P(:,:,1) = zeros(6);
R = Q_11;
 
for i=2:1031
    DeltaT = time(i)-time(i-1);
    B(i,:) = B(i-1,:);
    W(i,:) = gir_L(i,:)-B(i,:);
    q(i) =  quaternion(cos(norm(W(i,:))/2*DeltaT),W(i,1)/norm(W(i,:))*sin(norm(W(i,:))/2*DeltaT),...
            W(i,2)/norm(W(i,:))*sin(norm(W(i,:))/2*DeltaT),W(i,3)/norm(W(i,:))*sin(norm(W(i,:))/2*DeltaT))*q(i-1);
    w_x = [0,-W(i,3),W(i,2);W(i,3),0,-W(i,1);-W(i,2),W(i,1),0];
    Teta = eye(3)-DeltaT*w_x+DeltaT^2/2*(W(i,:)'*W(i,:)-norm(W(i,:))^2*eye(3));
    Psi = -eye(3)*DeltaT+DeltaT^2/2*w_x-DeltaT^3/6*(W(i,:)'*W(i,:)-norm(W(i,:))^2*eye(3));
    F = [Teta, Psi; zeros(3), eye(3)];
    Q_11 = eye(3)*rms_r^2*DeltaT+rms_w^2*(eye(3)*DeltaT^3/3+2*DeltaT^5/120*(W(i,:)'*W(i,:)-norm(W(i,:))^2*eye(3)));
    Q_12 = -rms_w^2*(eye(3)*DeltaT^2/2-DeltaT^3/6*w_x+DeltaT^4/24*(W(i,:)'*W(i,:)-norm(W(i,:))^2*eye(3)));
    Q_21 = Q_12';
    Q_22 = eye(3)*rms_w^2*DeltaT;
    Q = [Q_11,Q_12;Q_21,Q_22];
    P(:,:,i)=F*P(:,:,i-1)*F'+Q;
    
    rot = rotatepoint(q(i),G_G');
    matr = [0, -rot(3), rot(2);rot(3), 0, -rot(1); -rot(2), rot(1), 0];
    H = P_GL*matr;
    H = [H, zeros(3)];
    x_err = [W(i,1)*DeltaT,W(i,2)*DeltaT,W(i,3)*DeltaT,B(i,1),B(i,2),B(i,3)]';
    r = H*x_err;
    S = H*P(:,:,i)*H'+R;
    K = P(:,:,i)*H'*inv(S);
    x_err = K*r;
  
    q(i) =  quaternion(sqrt(1-norm(x_err(1:3))^2),x_err(1)/2,x_err(2)/2,x_err(3)/2);
    B(i,:) = B(i,:) + x_err(4:6)';
    W(i,:) = W(i,:) - B(i,:);
    P(:,:,i) = (zeros(6)-K*H)*P(:,:,i)*(zeros(6)-K*H)'+K*R*K';
end

figure
subplot (2,1,1);
plot(time,W(:,2), 'k', 'LineWidth' ,2);
subplot (2,1,2);
plot(time,gir_L(:,2), 'r');
figure;
hold on
plot(time,W(:,2), 'k', 'LineWidth' ,2);
plot(time,gir_L(:,2), 'r');
hold off