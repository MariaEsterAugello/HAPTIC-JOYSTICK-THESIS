
%to run only after a new test

%------------------------NORM AND TIME-------------------------%
meanNorm = mean(ToWorkspace.norm_min_valid_aircraft2path.signals.values);
varNorm = sqrt(var(ToWorkspace.norm_min_valid_aircraft2path.signals.values));
%flight time
RawsColumns = size(ToWorkspace.norm_min_valid_aircraft2path.signals.values);
flight_time = (-ToWorkspace.flight_time.signals.values(1) + ToWorkspace.flight_time.signals.values(RawsColumns(1)))/60;
%------------------------ERRORE LATERALE-------------------------%
%out of compute error
intersection2pathSegnoMeanVector = mean(ToWorkspace.intersection2pathSegnoFiltered.signals.values);
intersection2pathSegnoVar = sqrt(var(ToWorkspace.intersection2pathSegnoFiltered.signals.values));
%measure used to compute feedback
sMeanVector = mean(ToWorkspace.s.signals.values);
sVar = sqrt(var(ToWorkspace.s.signals.values));
%filtered signal s
distRollFiltered_mean = mean(ToWorkspace.distRollFiltered.signals.values); 
distRollFiltered_var = sqrt(var(ToWorkspace.distRollFiltered.signals.values)); 
%-----------------------ERRORE ALTEZZA---------------------------%
%sporco
hMeanVector = mean(ToWorkspace.h.signals.values);
hVar = sqrt(var(ToWorkspace.h.signals.values));
%filtrato
distPitchFiltered_mean = mean(ToWorkspace.distPitchFiltered.signals.values); 
distPitchFiltered_var = sqrt(var(ToWorkspace.distPitchFiltered.signals.values)); 
%-----------------------AIRCRAFT2PATH---------------------------%

xMean = mean(ToWorkspace.aircraft2pathX.signals.values);
xVar = var(ToWorkspace.aircraft2pathX.signals.values);
yMean = mean(ToWorkspace.aircraft2pathY.signals.values);
yVar = var(ToWorkspace.aircraft2pathY.signals.values);
zMean = mean(ToWorkspace.aircraft2pathZ.signals.values);
zVar = var(ToWorkspace.aircraft2pathZ.signals.values);



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% STRUCT %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%RICORDATI
%to clean them before real tests session starts
load('pilotperf.mat')
load('performance.mat')

% load('performance.mat') 
% load('pilotperf.mat')

%scrivi la variabile problemi RISOLVI DEVE ESSERE SALVATA LA VARIABILE
%PROBLEMI 
reply3 = convertCharsToStrings(questdlg('Any problems?'));
if reply3 == "Yes"
  problem = inputdlg('Please explain briefly');
else 
  clear problem
end  

%variables needed struct
problemExistance = exist('problem');
NumeroPilota = find(strcmp(participants, reply1)); %AVRà PROBLEMI CON STRINGHE
date = str2double(datestr(now,'ddmmyyyyHHMMSS'));

%domande
fprintf(' alto = 1, basso = 2, noAid = 3, non lo so = 0 ')
domandaA = str2double(inputdlg('Which one do you prefer? 1,2,3? Digit 0 if you have just started', 'domandaA')); %s serve per prenderlo come carattere 
while domandaA ~= 0 & domandaA ~= 1 & domandaA ~= 2 & domandaA ~= 3 
    fprintf('error')
    domandaA = str2double(inputdlg('Which one do you prefer? 1,2,3? Digit 0 if you have just started', 'domandaA'));
end
if isempty(domandaA) ==1
   domandaA = 0;
end 

domandaB = str2double(inputdlg('Which condition you think it was more physical demanding? 1,2,3? Digit 0 if you have just started', 'domandaB'));
while domandaB ~= 0 & domandaB ~= 1 & domandaB ~= 2 & domandaB ~= 3 
    fprintf('error')
    domandaB = str2double(inputdlg('Which condition you think it was more physical demanding? 1,2,3? Digit 0 if you have just started', 'domandaB'));
end
if isempty(domandaB) ==1
   domandaB = 0;
end 

domandaC = str2double(inputdlg('In which condition you think you had the best perfomance?', 'domandaC'));
while domandaC ~= 0 & domandaC ~= 1 & domandaC ~= 2 & domandaC ~= 3 
    fprintf('error')
    domandaC = str2double(inputdlg('In which condition you think you had the best perfomance?', 'domandaC'));
end
if isempty(domandaC) ==1
   domandaC = 0;
end 

domandaD = str2double(inputdlg('Under which condition you think you had the best control of the aircraft?', 'domandaD'));
while domandaD ~= 0 & domandaD ~= 1 & domandaD ~= 2 & domandaD ~= 3 
    fprintf('error')
    domandaD = str2double(inputdlg('Under which condition you think you had the best control of the aircraft?', 'domandaD'));
end
if isempty(domandaD) ==1
   domandaD = 0;
end 

domandaE = str2double(inputdlg('Which condition you think it was more mentally demanding?', 'domandaE'));
while domandaE ~= 0 & domandaE ~= 1 & domandaE ~= 2 & domandaE ~= 3 
    fprintf('error')
    domandaE = str2double(inputdlg('Which condition you think it was more mentally demanding?', 'domandaE'));
end
if isempty(domandaE) ==1
   domandaE = 0;
end 

imshow('CooperHarper.jpg');
domandaF = str2double(inputdlg('CooperHarper Rate', 'domandaF'));
while domandaF ~= 1 & domandaF ~= 2 & domandaF ~= 3 ...
      & domandaF ~= 4 & domandaF ~= 5 & domandaF ~= 6 ...
      & domandaF ~= 7 & domandaF ~= 8  & domandaF ~= 9 & domandaF ~= 10 ...
    fprintf('error')
    domandaF = str2double(inputdlg('', 'domandaF'));
end 
if isempty(domandaF) ==1
   domandaF = 0;
end 

PerformanceValues = [meanNorm; varNorm; flight_time; intersection2pathSegnoMeanVector; ...
                     intersection2pathSegnoVar; sMeanVector; sVar; hMeanVector; hVar; ...
                     xMean; xVar; yMean; yVar; zMean; zVar; problemExistance; NumeroPilota;...
                     distRollFiltered_mean; distRollFiltered_var; distPitchFiltered_mean; distPitchFiltered_var;...
                     domandaA; domandaB; domandaC; domandaD; domandaE; domandaF; date]; %testNumero];
fn = fieldnames(performance.DHA.alto); %ATTENZIONE HO AGGIUNTO CAMPI

%PERFORMANCE = STRUCT PER TIPO DI AIUTO
%per crearlo
%  for j = 1 : length(fn)
%             fields = fn{j};
%             performance.IHA.noAid.(fields) = 0;
%             %per tabella devo usare preallocazione perché ho spazi vuoti
%             %non ha senso testNumero = size(performance.DHA.alto.meanNorm, 2);
%  end
if isequal(configuration,'no_aid_DHAgroup') || isequal(configuration, 'direct_alto')...
     || isequal(configuration, 'direct_basso') %AVRA PROBLEMI CON STRINGHE
%gruppo aiuti diretti 
%performance.DHA. 
    if isequal(configuration, 'direct_alto')
        for j = 1 : length(fn)
            fields = fn{j};
            performance.DHA.alto.(fields)(end+1) = PerformanceValues(j);
            %per tabella devo usare preallocazione perché ho spazi vuoti
            %non ha senso testNumero = size(performance.DHA.alto.meanNorm, 2);
        end
    end
    
    if isequal(configuration, 'direct_basso')
        for j = 1 : length(fn)
            fields = fn{j};
            performance.DHA.basso.(fields)(end+1) = PerformanceValues(j);
            %testNumero = size(performance.DHA.basso.meanNorm, 2);
        end
    end
    
    if isequal(configuration,'no_aid_DHAgroup')
        for j = 1 : length(fn)
            fields = fn{j};
            performance.DHA.noAid.(fields)(end+1) = PerformanceValues(j);
            %testNumero = size(performance.DHA.noAid.meanNorm, 2);
        end   
    end


else 
%gruppo aiuti indiretti
%performance.IHA.
    if isequal(configuration, 'indirect_alto')
        for j = 1 : length(fn)
            fields = fn{j};
            performance.IHA.alto.(fields)(end+1) = PerformanceValues(j);
            %testNumero = size(performance.IHA.alto.meanNorm, 2);           
        end     
    end
    
    if isequal(configuration, 'indirect_basso')
        for j = 1 : length(fn)
            fields = fn{j};
            performance.IHA.basso.(fields)(end+1) = PerformanceValues(j);
            %testNumero = size(performance.IHA.basso.meanNorm, 2);
        end
    end
    
    if isequal(configuration,'no_aid_IHAgroup')
        for j = 1 : length(fn)
            fields = fn{j};
            performance.IHA.noAid.(fields)(end+1) = PerformanceValues(j);
            %testNumero = size(performance.IHA.noAid.meanNorm, 2);
        end
    end 
    %TROVA IL TUO PILOTA NELLO STRUCT performance
    %indexes = find(performance.IHA.basso.NumeroPilota == NumeroPilota);
    %bar(performance.IHA.basso.meanNorm(indexes))     
end


%STRUCT PER NOME%%%%%%%%%%%%%%%%%%%%%%
%costruisce i sottocampi per tutti i campi
% for j=1:1:length(participants)
%     pilot = participants{j};
%     for i = 1 : 1: length(fn) 
%         field = fn{i};
%         pilotperf.(pilot).(field) = 0;
%     end  
% end  

%detecta piloti nuovi e in caso aggiunge campo
if isfield(pilotperf, eval('reply1')) == 0
   for i = 1: 1: length(fn)
       field = fn{i};
       pilotperf.(reply1).(field) = 0;
   end 
end
%if first value is zero, it writes 
%if it's fifferent from zero it writes in end+1
if pilotperf.(reply1).(fn{1})(end) == 0
   for i = 1: 1: length(fn)
       field = fn{i};
       pilotperf.(reply1).(field)(end) = PerformanceValues(i);
   end   
else
   %pilotperf.(reply1)(end+1) = PerformanceValues(1); % provo a salvare solo meanNorm per ora
   for i = 1: 1: length(fn)
       field = fn{i};
       pilotperf.(reply1).(field)(end+1) = PerformanceValues(i);
   end  
end


%%%% MULTIPLOT %%%%%
% fai un tiledlayout
figure = tiledlayout(2,2);
% assegna ogni sottografico ad una variabile
plt_1 = nexttile([1,1]);
plt_2 = nexttile([1,1]);
plt_3 = nexttile([1,1]);
plt_4 = nexttile([1,1]);

% poi per stampare sul grafico i-esimo fai (es sul plt_1)
axes(plt_1) % così lo selezioni
bar([pilotperf.(reply1).(fn{1})])
% hold on
% bar([pilotperf.(reply1).(fn{2})])

axes(plt_2) 
plot3([coord_clock_Struct.Trajectory.x],[coord_clock_Struct.Trajectory.y],[coord_clock_Struct.Trajectory.z])
hold on
plot3(0, 0, 536, 'o')
plot3([ToWorkspace.PosAircraftXnwu.signals.values], [ToWorkspace.PosAircraftYnwu.signals.values] , [ToWorkspace.PosAircraftZnwu.signals.values], 'r')
grid
hold off
leg2 = legend({'coordPathXYZ', 'start', 'posAircraftXYZ'}, 'Location', 'northeastoutside'); %'northeastoutside');
%leg2.Layout.Tile = 'northeast';

axes(plt_3)
plot([coord_clock_Struct.Trajectory.x],[coord_clock_Struct.Trajectory.y]) 
hold on
grid
plot(0, 0, 'o')
plot([ToWorkspace.PosAircraftXnwu.signals.values], [ToWorkspace.PosAircraftYnwu.signals.values])
%plot([out.intersectionPointX.signals.values], [out.intersectionPointY.signals.values])
hold off
leg3 = legend({'coordPathXY', 'start', 'posAircraftXY'}, 'Location', 'northeastoutside');
%leg3.Layout.Tile = 'southeast';

axes(plt_4)
%ATTENZIONE il tempo qui si riferisce al tempo della simulazione(ora real time)
plot([ToWorkspace.norm_min_valid_aircraft2path.time], [ToWorkspace.coordclockovertimeZ.signals.values], 'k')
hold on
grid
plot([ToWorkspace.norm_min_valid_aircraft2path.time], [ToWorkspace.PosAircraftZnwu.signals.values])
hold off
leg4 = legend({'PathStepsCalledZ', 'posAircraftZ'}, 'Location', 'northeastoutside');
%leg4.Layout.Tile = 'southeast';

figName = strcat(datestr(now,'dd_mm_yyyy_HH_MM_SS'), reply1, '_', reply2, '.fig');
path_fig = fullfile('C:\Users\LAR\Desktop\SENSOSTICK_ESPERIMENTI7_09\', reply1, figName);
saveas(figure, path_fig, 'fig')
%export se vuoi in altri formati

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% RICORDA %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
save('performance.mat', 'performance')
save('pilotperf.mat', 'pilotperf')

%%%%%%%%%%%%%%%%%%% PROBLEMA TEMPI ANALISI %%%%%%%%%%%%%%%%%%%
% figure;
% ciao0 = ToWorkspace.simtime.data;
% ciao2 = ToWorkspace.flight_time.signals.values-ToWorkspace.flight_time.signals.values(1);
% ciao3 = ciao0- ciao2;
% plot(ciao0- ciao2);
% save('c.mat', 'ciao3')
% clear all
% load('b.mat')
% load('a.mat')
% load('c.mat')
% plot(ciao1)
% hold on
% plot(ciao2)
% hold on
% plot(ciao3)

