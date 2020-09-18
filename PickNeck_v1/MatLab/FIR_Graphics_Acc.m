figure;
title('Accelerometer')
for i=1:3
    subplot(3,3,i);
    hold on;
    plot(time,acc_L(:,i), 'r');
    plot(time,acc_FIR(:,i), 'k');
    hold off
    subplot(3,3,3+i);
    hold on;
    plot(time,acc_L(:,i), 'r');
    plot(time,acc_Weive(:,i), 'k');
    hold off
    subplot(3,3,6+i);
    hold on;
    plot(time,acc_L(:,i), 'r');
    plot(time,acc_FIR_Weive(:,i), 'k');
    hold off;
end

subplot(3,3,1);
title({'Acc_X'; ['Time is ', num2str(Time_FIR)]});
subplot(3,3,4);
title(['Time is ', num2str(Time_Weive)]);
subplot(3,3,7);
title(['Time is ', num2str(Time_Weive_FIR)]);
subplot(3,3,2);
title({'Acc_Y';''});
subplot(3,3,3);
title({'Acc_Z';''});

figure
hold on;
plot(time,acc_L(:,i), 'r');
plot(time,acc_FIR(:,i), 'k');
title('Acc\_FIR');
hold off

figure
hold on;
plot(time,acc_L(:,i), 'r');
plot(time,acc_FIR_Weive(:,i), 'k');
title('Acc\_FIR\_Weive');
hold off

