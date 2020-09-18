fileID = fopen ('Begs_Data_OnlyFIR_After(7).txt','wt');
Read = transpose([acc_FIR_X,acc_FIR_Y,acc_FIR_Z,angle_FIR,time]);

fprintf(fileID, '%.7f,%.7f,%.7f,%.7f,%.7f\n',Read(1:5,:));
fclose(fileID);