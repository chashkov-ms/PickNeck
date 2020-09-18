%subplot (2,2,1);
%plot(1:size(acc_FIR_X),acc_FIR_X, 'r');

%fileID = fopen ('Test_Fragment_Prised.txt','wt');
%Read = transpose([acc_FIR_X(340:378,1),acc_FIR_Y(340:378,1),acc_FIR_Z(340:378,1),angle_FIR(340:378,1)]);
%fprintf(fileID, '%.7f,%.7f,%.7f,%.7f\n',Read(1:4,:));
%fclose(fileID);

fileID = fopen ('Test_Fragment_Prised.txt','r');
Data = textscan(fileID,'%f,%f,%f,%f');

acc_corr = [Data{1}, Data{2}, Data{3}];
angle_corr = Data{4};
N = size(Data{1});
N=N(1);
M=size(acc_FIR_X);
M=M(1);
fclose(fileID);
COR = xcorr(acc_corr(:,1),acc_FIR_X(1:N),'normalized');
c1(1) = COR(N);
COR = xcorr(acc_corr(:,2),acc_FIR_Y(1:N),'normalized');
c2(1) = COR(N);
COR = xcorr(acc_corr(:,3),acc_FIR_Z(1:N),'normalized');
c3(1) = COR(N);
COR = xcorr(angle_corr(:,1),angle_FIR(1:N),'normalized');
c4(1) = COR(N);
Evklid (1) = sqrt(c1(1)^2+c2(1)^2+c3(1)^2+c4(1)^2);
for i=2:M-N
    COR = xcorr(acc_corr(:,1),acc_FIR_X(i:N+i-1),'normalized');
    c1(i,1) = COR(N);
    COR = xcorr(acc_corr(:,2),acc_FIR_Y(i:N+i-1),'normalized');
    c2(i,1) = COR(N);
    COR = xcorr(acc_corr(:,3),acc_FIR_Z(i:N+i-1),'normalized');
    c3(i,1) = COR(N);
    COR = xcorr(angle_corr(:,1),angle_FIR(i:N+i-1),'normalized');
    c4(i,1) = COR(N);
    Evklid (i,1) = sqrt(c1(i)^2+c2(i)^2+c3(i)^2+c4(i)^2);
end

subplot (2,2,1);
plot(1:size(c1),c1, 'r');
subplot (2,2,2);
plot(1:size(c1),c2, 'r');
subplot (2,2,3);
plot(1:size(c1),c3, 'r');
subplot (2,2,4);
plot(1:size(c1),c4, 'r');

figure
plot(1:size(c1),Evklid, 'r');

