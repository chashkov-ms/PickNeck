clear all;
fileID = fopen ('~/../Положение стоя 1 мин - Шапель.txt','r');
Data = textscan(fileID,'%f,%f,%f,%f,%f,%f,%f,%f,%f,%f');

acc_L = [Data{1}, Data{2}, Data{3}];
gir_L = [Data{4}, Data{5}, Data{6}];
mag_L = [Data{7}, Data{8}, Data{9}];
time =  Data{10}/1000000;
fclose(fileID);
G_const = [0;0;-9.807];

S(1,:)=acc_L(1,:);
F = eye(3);
H = eye(3);
rms = 8*10^-3;
Q = eye(3)*rms^2;
R = eye(3)*rms^2;
P(:,:,1) = zeros(3);

for i=2:10
    S(i,:) = S(i-1,:)*F;
    P(:,:,i)=F*P(:,:,i-1)*F'+Q;
    K = P(:,:,i)*H'*inv(H*P(:,:,i)*H'+R);
    S(i,:) = S(i,:)+(acc_L(i,:)-S(i,:)*H)*K;
    P(:,:,i) = (eye(3)-K*H)*P(:,:,i);
end


clear F H Q R P;
W(1,:)=gir_L(1,:);
d_Acc(1,1:3)=0;
Acc_Body(1,1:3)=0;
B(1,1:3)=0;
D_q(1) =  quaternion(cos(norm(W(1,:))/2*0),W(1,1)/norm(W(1,:))*sin(norm(W(1,:))/2*0),...
        W(1,2)/norm(W(1,:))*sin(norm(W(1,:))/2*0), W(1,3)/norm(W(1,:))*sin(norm(W(1,:))/2*0));
orient_GL(1) = FindOrient(S(10,:),mag_L(1,:),G_const);
%P_LG = rotmat(conj(orient_LG(1)), 'point');
P_GL = rotmat(orient_GL(1), 'point');
G_L(1,:) = (P_GL*G_const)'+Acc_Body(1,:);

rms_r = 1*10^-4;
rms_w = 1*10^-4;
rms_a = 5*10^-4;
Q_11 = eye(3)*rms_r^2;
Q_12 = zeros(3);
Q_21 = Q_12';
Q_22 = eye(3)*rms_w^2;
Q_33 = eye(3)*rms_a^2;
Q = [Q_11,Q_12, zeros(3);Q_21,Q_22, zeros(3); zeros(3),zeros(3), Q_33];
P(:,:,1) = zeros(9);
R = Q_33;

Dimension = size(acc_L);
for i=2:Dimension(1)
    DeltaT = time(i)-time(i-1);
    B(i,:) = B(i-1,:);
    W(i,:) = gir_L(i,:)-B(i,:);
%     Acc_Body(i,:) = Acc_Body(i-1,:);
       
    D_q(i) = quaternion(cos(norm(W(i,:))/2*DeltaT),W(i,1)/norm(W(i,:))*sin(norm(W(i,:))/2*DeltaT),...
            W(i,2)/norm(W(i,:))*sin(norm(W(i,:))/2*DeltaT),W(i,3)/norm(W(i,:))*sin(norm(W(i,:))/2*DeltaT));
    orient_GL(i) =  D_q(i)*orient_GL(i-1);

%     d_Acc(i,:) = Acc_Body(i,:)-acc_L(i,:)+rotatepoint(orient_GL(i),G_const');
        d_Acc(i,:) = 0;
    
    w_x = [0,-W(i,3),W(i,2);W(i,3),0,-W(i,1);-W(i,2),W(i,1),0];
    Teta = eye(3)-DeltaT*w_x+DeltaT^2/2*(W(i,:)'*W(i,:)-norm(W(i,:))^2*eye(3));
    Psi = -eye(3)*DeltaT+DeltaT^2/2*w_x-DeltaT^3/6*(W(i,:)'*W(i,:)-norm(W(i,:))^2*eye(3));
    F = [Teta, Psi, zeros(3); zeros(3), eye(3), zeros(3); zeros(3), zeros(3), eye(3)];
    Q_11 = eye(3)*rms_r^2*DeltaT+rms_w^2*(eye(3)*DeltaT^3/3+2*DeltaT^5/120*(W(i,:)'*W(i,:)-norm(W(i,:))^2*eye(3)));
    Q_12 = -rms_w^2*(eye(3)*DeltaT^2/2-DeltaT^3/6*w_x+DeltaT^4/24*(W(i,:)'*W(i,:)-norm(W(i,:))^2*eye(3)));
    Q_21 = Q_12';
    Q_22 = eye(3)*rms_w^2*DeltaT;
    Q_33 = eye(3)*rms_a^2*DeltaT;
    Q = [Q_11,Q_12, zeros(3);Q_21,Q_22, zeros(3); zeros(3), zeros(3),Q_33];
    P(:,:,i)=F*P(:,:,i-1)*F'+Q;    
%     G_G = G_const'+D_Acc(i,:);
    
    rot = rotatepoint(orient_GL(i),G_const');
    matr = [0, -rot(3), rot(2);rot(3), 0, -rot(1); -rot(2), rot(1), 0];
    H = [matr, zeros(3), eye(3)];
    
    x_err = [W(i,1)*DeltaT,W(i,2)*DeltaT,W(i,3)*DeltaT,B(i,1),B(i,2),B(i,3),d_Acc(i,1),d_Acc(i,2),d_Acc(i,3)]';
    r = H*x_err;
    S = H*P(:,:,i)*H'+R;
    K = P(:,:,i)*H'*inv(S);
    x_err = K*r;
   
    if norm(x_err(1:3)) < 1
        D_q(i) =  quaternion(sqrt(1-norm(x_err(1:3))^2),x_err(1)/2,x_err(2)/2,x_err(3)/2);
    else
        D_q(i) =  sqrt(1+norm(x_err(1:3))^2)*quaternion(1,x_err(1)/2,x_err(2)/2,x_err(3)/2);
    end
    B(i,:) = B(i,:) + x_err(4:6)';
%     Acc_Body(i,:) = Acc_Body(i,:) -x_err(7:9)';
    W(i,:) = W(i,:) - B(i,:);
    P(:,:,i) = (zeros(9)-K*H)*P(:,:,i)*(zeros(9)-K*H)'+K*R*K';
    
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

figure
title('The graphic of Body Acceleration X '); 
plot(time,Acc_Body(:,1), 'r');
figure
title('The graphic of Body Acceleration Y ');
plot(time,Acc_Body(:,2), 'r');
figure
title('The graphic of Body Acceleration Z '); 
plot(time,Acc_Body(:,3), 'r');

function res = FindOrient(acc, mag, g_G)
ang = acos(dot(g_G,acc')/norm(g_G)/norm(acc));
vect = cross(g_G, acc');
vect = vect/norm(vect)*sin(ang/2);
Q_GL = quaternion(cos(ang/2),vect(1),vect(2), vect(3));
vect_m = rotatepoint(conj(Q_GL),mag);
vect_cplx = complex(vect_m(1),vect_m(2));
ang = angle(vect_cplx);
Q_M_LG = quaternion(cos(ang/2),0,0,-sin(ang/2));
res = conj(Q_M_LG*conj(Q_GL));
end



