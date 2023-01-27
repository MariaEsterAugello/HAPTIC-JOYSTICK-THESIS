loop = 1; 

%cell array of char
configurations = {'da'; 'db'; 'nd'; 'ia';...
                  'ib'; 'ni'}; %cell array, meglio no stringa

%participants : if there's some new pilot add their name here AGGIUNGI IN FONDO!!!
participants = {'chiara'; 'antonio'; 'andrea'; 'pollini'; 'augello'; 'tedeschi'; ...
                'matteo'; 'francesca'; 'ruscio'; 'costanzi'; 'bruno'; ...
                'marco'; 'bertocchini'; 'sensostick'; 'pincopalla'; 'ilaria'; ...
                'martina'; 'cristian'; 'giovanni'; 'michele'; 'federico'; 'iannizzotto';...
                'valentina'; 'rocco'; 'mugnaini'; 'pierpaolo'; 'carlos'; 'caselli';...
                'leonardo'; 'paone'; 'bonato'; 'susini'; 'manconi'; 'cristofani';...
                'gentili'; 'suada'; 'calzaretta'; 'pastore'; 'francesconi'; 'maugeri';...
                'benedetti'; 'francesco'; 'mattia'; 'nannola'; 'guerci'; 'cannoni'; 'bini'; ...
                'marchiani'; 'roberta'; 'emanuela'; 'riccardo'; 'simone'}; %cell array
%aggiunge le cartelle in caso di persone nuove
for i = 1 : length(participants) 
    statusFolders = mkdir(participants{i}); 
end

fprintf('%s\n','<strong>watch out</strong>: if something goes wrong after flying logfile.mat is in working folder') 
fprintf('logfile.mat contains last simulation signals, it overwrites at each simulation start\n')
    
%questions and inputs
reply0 = convertCharsToStrings(questdlg('Do you want to start?')); %domanda inizializzazione reply0 
while reply0 == "Yes"   %while loop for test that follows one another, specify reply0 
                        %opposite case it starts with N 
      clearvars -except loop configurations participants reply0 %answer 
      run('new_path_local_frame4_05_2022');
      %le domande così come la partenza della simulazione matlab devono
      %stare dentro il while. per quanto riguarda il run dello script
      %invece questa cosa non è possibile quindi mi limiterò a fare il run
      %fuori e aggiornare la configurazione di riferimento una volta
      %chiesta la domanda relativa ad essa reply2. Creato script
      %configurazione separato.
      fprintf('<strong>Right now our pilots are: </strong>\n')
      fprintf('%s  %s  %s  %s  %s  %s  %s  %s\n', participants{:})
      
      reply1 = char(inputdlg('Who is playing?')); %s serve per prenderlo come carattere 
      if isempty(reply1) == 1
          break
      end
      while strcmp(reply1, participants(:)) == zeros(length(participants), 1) %comparison character vectors
          fprintf('error\n')
          %explain why
          fprintf('Right now our pilots are:')
          fprintf('%s  %s  %s  %s  %s  %s  %s  %s\n', participants{:})
          %explain how to recover
          fprintf('\n Do we have a new player? Add their name at line 19 \n then play again\n') 
          %ask again till he/she answers correctly
          reply1 = char(inputdlg('Who is playing?')); 
          if isempty(reply1) == 1
            break
          end
      end
      %controlla se c'è un modo più intelligente
      if isempty(reply1) == 1
         break
      end

      fprintf('\n Do we have a new player? Add their name at line 19 \n then press ctrl+c and run again\n') 
      fprintf('<strong> to interrupt the simulation click "e" on keyboard</strong>\n do not press anything else while flying\n')
      fprintf('<strong> wait for sound before giving power</strong>\n')
      fprintf(' remember : if you cant get the first gate dont rush for it, aim at the next one, follow a smooth path\n')
      fprintf('\n%c%c', configurations{:})
      
      reply2 = char(inputdlg('Ready?! What kind of help?')); 
      if isempty(reply2) == 1
            fprintf('\n error')
            break
      end
      while strcmp(reply2, configurations(:)) == zeros(6, 1) %compearison character vectors  (strcmp Compare strings or character vectors)
         fprintf('error\n')
         %ask again till he/she answers correrectly
         reply2 = char(inputdlg('Ready?! What kind of help?', 'Configuration'));
         fprintf('\n%c%c\n', configurations{:})
         %controlla se c'è un modo più intelligente
         if isempty(reply2) == 1
            fprintf('error')
            break
         end
      end
      %controlla se c'è un modo più intelligente
      if isempty(reply2) == 1
         fprintf('error')
         break
      end
      
      %associa configurazione per script configuration
      if reply2 == 'da'
         configuration = 'direct_alto';
      end
      if reply2 == 'db'
         configuration = 'direct_basso';
      end
      if reply2 == 'nd'
         configuration = 'no_aid_DHAgroup';
      end          
      if reply2 == 'ia'
         configuration = 'indirect_alto';
      end
      if reply2 == 'ib'
         configuration = 'indirect_basso';
      end      
      if reply2 == 'ni'
         configuration = 'no_aid_IHAgroup';
      end       
          
      run('Configuration_13_09_22FINESTRELLE')  %importante nel caso in cui si facessero due consecutivi


      % activate profiler:
      load_system('model_16_09_22')
      set_param('model_16_09_22','Profile','on');  
      open_system('model_16_09_22')
      warning('off','Simulink:DataStores:DSReadDiagnosticsNotReported') 
      % start simulation  
      %accelbuild model_16_09_22
      %stop 
      %ToWorkspace = sim('model_16_09_22');
      %sim('model_16_09_22','StartTime','0','StopTime','60','FixedStep','0.01');
      ToWorkspace = sim('model_16_09_22','SimulationMode','accelerator');

      
      logfileLoaded = load('logfile.mat'); %save logfile as a workspace var
      
      
      
      
      %%%%%%% IMPORTANTE
      run('DataAnalysis21_09_22FINESTRELLE')
      %%%%%%%IMPORTANTE 
      
      
      
    
      %carico i risultati nel workspace che poi salvo
      fileName = strcat(datestr(now,'dd_mm_yyyy_HH_MM_SS'), reply1, '_', reply2);
      path_test = fullfile('C:\Users\LAR\Desktop\SENSOSTICK_ESPERIMENTI7_09', reply1, fileName);
      save(path_test) %save workspace in folder 
      

      reply4 = convertCharsToStrings(questdlg('Play again?')); 
      if reply4 == "No"
         break
      end
      
      loop = loop + 1;

end %end of loop
fprintf('\n <strong>thank you for your precious help!!!</strong>') 

%end of file

      
