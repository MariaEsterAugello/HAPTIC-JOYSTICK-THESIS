%---AID CONFIGURATIONS-----------------------------------------------#
%   PreStopConf = 0; 
    desired_index = 0;
    load('FdT_2_08_22')
    load('FILTER_4_08_22')
    load('FILTER_22_09_22')
    load('matlabSounds.mat')
% %-------------------Configuration NO AID----------------------------%
if isequal(configuration, 'no_aid_DHAgroup') || isequal(configuration, 'no_aid_IHAgroup')
    
   indirect = 0; %on 
    
   springRollConst = 35; %550;         %[mNm/deg]
   gainRollTorque = 0 %700;             %[mNm]
   timesRoll = 0
   dampRollConst = 5;                 %[mNm/rpm] 
   
   springPitchConst = 35; %550;           %[mNm/deg]
   gainPitchTorque = 0 %700;            %[mNm]
   timesPitch = 0
   dampPitchConst = 5;                %[mNm/rpm]  
end   
% %-------------------Configuration DHA alto-----------------------------%
if isequal(configuration, 'direct_alto')
   indirect = 0; %off 
   %Nrollext = 0.5*0.5;
   
   springRollConst = 35; %600;            %[mNm/deg]
   gainRollTorque = 1000; %1400;  %800;             %[mNm]
   timesRoll = 0.7
   dampRollConst = 5;                %[mNm/rpm]
   
   springPitchConst = 35; %600;           %[mNm/deg]
   gainPitchTorque = 1000 %1400; %800;            %[mNm]
   timesPitch = 0.7
   dampPitchConst = 5;               %[mNm/rpm]    
end
% %-------------------Configuration IHA alto-----------------------------%
if isequal(configuration, 'indirect_alto')
    
   indirect = 1; %on  
   %Nrollext = 0.5*0.5;
    
   springRollConst = 35; %550;         %[mNm/deg]
   gainRollTorque = -1000 %700;             %[mNm]
   timesRoll = 0.8
   dampRollConst = 5;                 %[mNm/rpm] 
   
   springPitchConst = 35; %550;           %[mNm/deg]
   gainPitchTorque = -1000 %700;            %[mNm]
   timesPitch = 0.8
   dampPitchConst = 5;                %[mNm/rpm]  
end

% %-------------------Configuration DHA basso-----------------------------%
if isequal(configuration, 'direct_basso')
    
   indirect = 0; %on 
   
   springRollConst = 35; %550;         %[mNm/deg]
   gainRollTorque = 1000 %700;             %[mNm]
   timesRoll = 0.5
   dampRollConst = 5;                 %[mNm/rpm] 
   
   springPitchConst = 35; %550;           %[mNm/deg]
   gainPitchTorque = 1000 %700;            %[mNm]
   timesPitch = 0.5
   dampPitchConst = 5;                %[mNm/rpm]  
end
% %-------------------Configuration IHA basso-----------------------------%
if isequal(configuration, 'indirect_basso')
    
   indirect = 1; %on  
   %Nrollext = 0.5*0.5;
    
   springRollConst = 35; %550;         %[mNm/deg]
   gainRollTorque = -1000 %700;             %[mNm]
   timesRoll = 0.6
   dampRollConst = 5;                 %[mNm/rpm] 
   
   springPitchConst = 35; %550;           %[mNm/deg]
   gainPitchTorque = -1000 %700;            %[mNm]
   timesPitch = 0.6
   dampPitchConst = 5;                %[mNm/rpm]  
end
