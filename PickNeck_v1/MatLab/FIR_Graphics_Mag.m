figure;
title('Accelerometer')
for i=1:3
    subplot(3,3,i);
    hold on;
    plot(time,mag_L(:,i), 'r');
    plot(time,mag_FIR(:,i), 'k');
    hold off
    subplot(3,3,3+i);
    hold on;
    plot(time,mag_L(:,i), 'r');
    plot(time,mag_Weive(:,i), 'k');
    hold off
    subplot(3,3,6+i);
    hold on;
    plot(time,mag_L(:,i), 'r');
    plot(time,mag_FIR_Weive(:,i), 'k');
    hold off;
end

subplot(3,3,1);
title({'Mag_X'; ['Time is ', num2str(Time_FIR)]});
subplot(3,3,4);
title(['Time is ', num2str(Time_Weive)]);
subplot(3,3,7);
title(['Time is ', num2str(Time_Weive_FIR)]);
subplot(3,3,2);
title({'Mag_Y';''});
subplot(3,3,3);
title({'Mag_Z';''});
