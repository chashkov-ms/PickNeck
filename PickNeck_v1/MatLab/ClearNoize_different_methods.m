clear all;
fileID = fopen ('~/../Комплекс.txt','r');
Data = textscan(fileID,'%f,%f,%f,%f,%f,%f,%f,%f,%f,%f');

acc_L = [Data{1}, Data{2}, Data{3}];
gir_L = [Data{4}, Data{5}, Data{6}];
mag_L = [Data{7}, Data{8}, Data{9}];
time =  Data{10}/1000000;
fclose(fileID);

N = 32;
b = 0.09*sinc(0.09*(-N:N));
b = b.*hamming(65)';            %Find FIR coefficient
tic;
for i = 1:3
    acc_FIR(:,i) = filter(b,1,acc_L(:,i));
    gir_FIR(:,i) = filter(b,1,gir_L(:,i));
    mag_FIR(:,i) = filter(b,1,mag_L(:,i)); 
end
Time_FIR = toc;
tic;
[thr, sh, kp]=ddencmp('den', 'wv', acc_L(:,1));         
[A_XC, CXC, LXC, F0, FL2] = wdencmp('gbl', acc_L(:,1), 'sym4', 2, thr, sh, kp); %It is noise_x clear with weivelet
[thr, sh, kp]=ddencmp('den', 'wv', acc_L(:,2));         
[A_YC, CXC, LXC, F0, FL2] = wdencmp('gbl', acc_L(:,2), 'sym4', 2, thr, sh, kp); %It is noise_x clear with weivelet
[thr, sh, kp]=ddencmp('den', 'wv', acc_L(:,3));         
[A_ZC, CXC, LXC, F0, FL2] = wdencmp('gbl', acc_L(:,3), 'sym4', 2, thr, sh, kp); %It is noise_x clear with weivelet
acc_Weive = [A_XC, A_YC, A_ZC];

[thr, sh, kp]=ddencmp('den', 'wv', gir_L(:,1));         
[G_XC, CXC, LXC, F0, FL2] = wdencmp('gbl', gir_L(:,1), 'sym4', 2, thr, sh, kp); %It is noise_x clear with weivelet
[thr, sh, kp]=ddencmp('den', 'wv', gir_L(:,2));         
[G_YC, CXC, LXC, F0, FL2] = wdencmp('gbl', gir_L(:,2), 'sym4', 2, thr, sh, kp); %It is noise_x clear with weivelet
[thr, sh, kp]=ddencmp('den', 'wv', gir_L(:,3));         
[G_ZC, CXC, LXC, F0, FL2] = wdencmp('gbl', gir_L(:,3), 'sym4', 2, thr, sh, kp); %It is noise_x clear with weivelet
gir_Weive = [G_XC, G_YC, G_ZC];

[thr, sh, kp]=ddencmp('den', 'wv', mag_L(:,1));         
[M_XC, CXC, LXC, F0, FL2] = wdencmp('gbl', mag_L(:,1), 'sym4', 2, thr, sh, kp); %It is noise_x clear with weivelet
[thr, sh, kp]=ddencmp('den', 'wv', mag_L(:,2));         
[M_YC, CXC, LXC, F0, FL2] = wdencmp('gbl', mag_L(:,2), 'sym4', 2, thr, sh, kp); %It is noise_x clear with weivelet
[thr, sh, kp]=ddencmp('den', 'wv', mag_L(:,3));         
[M_ZC, CXC, LXC, F0, FL2] = wdencmp('gbl', mag_L(:,3), 'sym4', 2, thr, sh, kp); %It is noise_x clear with weivelet
mag_Weive = [M_XC, M_YC, M_ZC];
Time_Weive = toc;

for i = 1:3
    acc_FIR_Weive(:,i) = filter(b,1,acc_Weive(:,i));
    gir_FIR_Weive(:,i) = filter(b,1,gir_Weive(:,i));
    mag_FIR_Weive(:,i) = filter(b,1,mag_Weive(:,i));
    
end
Time_Weive_FIR = toc;
