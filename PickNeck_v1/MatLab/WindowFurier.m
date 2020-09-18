clear Spektr_x Spektr_y Spektr_z
close all
N = 32;     %Window Length 
M = N/2;    %Window cross
len = size(time);
len = ceil(len(1)/M)*M;
K = (len-N)/M+1;                  %Number of iteration
time(len)=0;
acc_G(len,:)=0;

y = 0:N-1;
x_t = time(N/2:M:len-N/2);
[X,Y] = meshgrid(x_t,y);

w(y+1) = 0.5*(1-cos(2*pi*(y+1)/N));
i = 1;
for k = 1:K

    Spektr_x(:,k)=fft(w'.*acc_G(i:i+N-1,1));
    Spektr_y(:,k)=fft(w'.*acc_G(i:i+N-1,2));
    Spektr_z(:,k)=fft(w'.*acc_G(i:i+N-1,3));
    i = i+M;
end
Spektr_x(:,:) = abs (Spektr_x(:,:));
Spektr_y(:,:) = abs (Spektr_y(:,:));
Spektr_z(:,:) = abs (Spektr_z(:,:));

Furie_x = figure('Name','Accelerometr X Furier Transform Complex','NumberTitle','off');
mesh(X,Y,Spektr_x);
title ({'Accelerometr X Furier Transform';'Комплекс упражнений'});
xlabel({'Time','(in seconds)'});
ylabel('n');
zlabel('Magnitude');
dim = [.2 .1 .9 .7];
if M == N/2
    str = {['Window size ',num2str(N)]; 'WindowCross 1/2'};
end
if M == N/4
    str = {['Window size ',num2str(N)]; 'WindowCross 3/4'};
end
annotation('textbox',dim,'String',str,'FitBoxToText','on');

Furie_y = figure('Name','Accelerometr Y Furier Transform Complex','NumberTitle','off');
mesh(X,Y,Spektr_y);
title ({'Accelerometr Y Furier Transform';'Комплекс упражнений'});
xlabel({'Time','(in seconds)'});
ylabel('n');
zlabel('Magnitude');
dim = [.2 .1 .9 .7];
if M == N/2
    str = {['Window size ',num2str(N)]; 'WindowCross 1/2'};
end
if M == N/4
    str = {['Window size ',num2str(N)]; 'WindowCross 3/4'};
end
annotation('textbox',dim,'String',str,'FitBoxToText','on');

Furie_z = figure('Name','Accelerometr Z Furier Transform Complex','NumberTitle','off');
mesh(X,Y,Spektr_z);
title ({'Accelerometr Z Furier Transform';'Комплекс упражнений'});
xlabel({'Time','(in seconds)'});
ylabel('n');
zlabel('Magnitude');
dim = [.2 .1 .9 .7];
if M == N/2
    str = {['Window size ',num2str(N)]; 'WindowCross 1/2'};
end
if M == N/4
    str = {['Window size ',num2str(N)]; 'WindowCross 3/4'};
end
annotation('textbox',dim,'String',str,'FitBoxToText','on');


