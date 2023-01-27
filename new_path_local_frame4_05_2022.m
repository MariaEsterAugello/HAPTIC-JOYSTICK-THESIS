%---Sensostick Transfer Function------------------------------------------#
%---Geodetic WGS84 System Parameters--------------------------------------#
R_E = 6378137;           % reference ellipsoid: semi-major axis         [m]
ECC = 0.0818191908426;   % reference ellipsoid: eccentricity            [-]

%---Coordinates Local Frame-----------------------------------------------#
lonSpeck = 8.75412479723240780131;
latSpeck = 47.377943363;
lon0 = deg2rad(lonSpeck);  %[rad]                                            
lat0 = deg2rad(latSpeck);  %[rad]                                            
alt0 = 0;                  %[m]
SpeckAirportAltitude = 537.305031521297;
%orientationRunway = 122;

%normal radius
N = R_E / sqrt(1 - (ECC * sin(lat0))^2);

%coordinate transformation
x_0_ecef = (N + alt0) * cos(lat0) * cos(lon0);
y_0_ecef = (N + alt0) * cos(lat0) * sin(lon0);
z_0_ecef = (N * (1 - ECC^2) + alt0) * sin(lat0);
format long
%coord_0_ecef = [4276819, 658580, 4670713];  %[m]%sottraggo l'altitudine di speck perché il mio ref è a zero
coord_0_ecef = [x_0_ecef, y_0_ecef, z_0_ecef]; %they are the same!

%---Path Parameters-------------------------------------------------------#
zmotionrange = 50;                                                     %[m]
h = (SpeckAirportAltitude + 150); %altitude of the path nord weast up center set at (0, R, h)[m]
R = 5000;                                                              %[m]
pathCenter = [- R, 0, h];                                              %[m]
T = 600;     %FIXED                                                    %[s]
Nrounds = 1;
nu = 1/T;                                                              %[Hz]
omega = 2*pi*nu ;                                                    %[rad/s]
ellipticdeformation = 1.3;
twotimesfrequencyamplitude = 800;
threetimesfrequencyamplitude = 350;
fourtimesfrequencyamplitude = 200;
fivetimesfrequencyamplitude = 300;
offsetx = 4650;
offsety = - 800;
takeoffmultiplicator = 500;
takeoffstretcher = 40;
takeoffshifter = 20;
a = 4;
b = 3;
c = 1;
d = 1;

% change as you like, just remember that gates_lenght_step should be an integer
steps = 14400; %at least it should be 14400         %[steps in 360 degrees]
lenght_step = T/steps;                                              %[time]
%number of gates as approaching guides in aerofly
gates = 3600*Nrounds; %do 3600 for test perspective,40 standard conditions
gates_lenght_step = steps/gates;                                 %[degrees]  

%---Rotation matrices for ecef2NWU conversion-----------------------------#
%we are going to use the inverse matrix
RotationMatrix_NWU2ECEF = [-cos(lon0)*sin(lat0),  sin(lon0), cos(lon0)*cos(lat0);
                           -sin(lon0)*sin(lat0), -cos(lon0), sin(lon0)*cos(lat0);
                                      cos(lat0),       0,          sin(lat0)];
                                     
% Rxlon = [1 0 0; 0 cos(lon0) -sin(lon0); 0 sin(lon0) cos(lon0)];
% Rylat = [cos(lat0) 0 -sin(lat0); 0 1 0; sin(lat0) 0 cos(lat0)];
% RotationMatrix_ECEF2NWU = Rxlon * Rylat;
                                     
%---Loop for the path-----------------------------------------------------%
coord_clock_Struct.Steps = steps;
coord_clock_Struct.n90DegCounter = 0;
i = 1;
for pathClock = 0: Nrounds*lenght_step : Nrounds*(T - lenght_step) %we compute them to do two rounds
     %PATH : NORD-EAST-DOWN LOCAL FRAME
     coord_clock_Struct.Trajectory(i).clock = rad2deg(pathClock*omega);
     %coordinates of the path
     %the radius is really big but the path center is shifted so that we
     %can see it from the start of the Speck Faralthof runaway
     coord_clock_Struct.Trajectory(i).x =    - d*R*cos(omega*pathClock) - c*twotimesfrequencyamplitude*sin(2*omega*pathClock)...
                                             + threetimesfrequencyamplitude*cos(3*omega*pathClock)... %+ pathCenter(1); 
                                             - fourtimesfrequencyamplitude*sin(4*omega*pathClock)...
                                             - a*fivetimesfrequencyamplitude*sin(5*omega*pathClock) + offsetx ;  
     coord_clock_Struct.Trajectory(i).y =    - b*ellipticdeformation*R*sin(omega*pathClock) + twotimesfrequencyamplitude*cos(2*omega*pathClock)...
                                             + fourtimesfrequencyamplitude*sin(4*omega*pathClock) + offsety; %+ pathCenter(2) ;
     coord_clock_Struct.Trajectory(i).z =    536 - 239 + (1 + erf((pathClock - takeoffshifter)/takeoffstretcher))*takeoffmultiplicator; %...
                                             %+ 30*sin(2*omega*pathClock); %pathCenter(3);
                                            
     %derivatives, tangents to each point of the path
     coord_clock_Struct.Trajectory(i).dx =   + d*omega*R*sin(omega*pathClock) - c*2*omega*twotimesfrequencyamplitude*cos(2*omega*pathClock)...
                                             - 3*threetimesfrequencyamplitude*omega*sin(3*omega*pathClock)...
                                             - 4*fourtimesfrequencyamplitude*omega*cos(4*omega*pathClock)...
                                             - a*5*omega*fivetimesfrequencyamplitude*cos(5*omega*pathClock);  
     coord_clock_Struct.Trajectory(i).dy =   - b*ellipticdeformation*omega*R*cos(omega*pathClock) ...
                                             - 2*omega*twotimesfrequencyamplitude*sin(2*omega*pathClock)...
                                             + 4*omega*fourtimesfrequencyamplitude*cos(4*omega*pathClock);
     coord_clock_Struct.Trajectory(i).dz =   + (2*takeoffmultiplicator*exp(-(pathClock - takeoffshifter)^2/(takeoffstretcher)^2))/(takeoffstretcher*sqrt(pi)); %+ 60*omega*cos(2*omega*pathClock); 

     i = i + 1;
end

% ---Sending struct to simulink--------------------------------------------%
Simulink.Bus.createObject(coord_clock_Struct);
ExperimentConfigAerofly = slBus1;
clear slBus1;                                                                                                                                          
% ---Creating gates--------------------------------------------------------%
run('GateGenerator1_10.m')



  