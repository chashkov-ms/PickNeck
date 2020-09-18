figure;
title('Accelerometer')
for i=1:3
    subplot(3,3,i);
    hold on;
    plot(time,gir_L(:,i), 'r');
    plot(time,gir_FIR(:,i), 'k');
    hold off
    subplot(3,3,3+i);
    hold on;
    plot(time,gir_L(:,i), 'r');
    plot(time,gir_Weive(:,i), 'k');
    hold off
    subplot(3,3,6+i);
    hold on;
    plot(time,gir_L(:,i), 'r');
    plot(time,gir_FIR_Weive(:,i), 'k');
    hold off;
end

subplot(3,3,1);
title({'Gir_X'; ['Time is ', num2str(Time_FIR)]});
subplot(3,3,4);
title(['Time is ', num2str(Time_Weive)]);
subplot(3,3,7);
title(['Time is ', num2str(Time_Weive_FIR)]);
subplot(3,3,2);
title({'Gir_Y';''});
subplot(3,3,3);
title({'Gir_Z';''});


figure
hold on;
plot(time,gir_L(:,1), 'r');
plot(time,gir_FIR(:,1), 'k');
title('Gir\_FIR');
hold off

figure
hold on;
plot(time,gir_L(:,1), 'r');
plot(time,gir_FIR_Weive(:,1), 'k');
title('Gir\_FIR\_Weive');
hold off
