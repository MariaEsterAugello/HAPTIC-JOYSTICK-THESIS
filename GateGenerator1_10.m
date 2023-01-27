%---Opening tsc file and writing first part-------------------------------%
fid = fopen( 'mygateRadiansRUNAWAY.tsc', 'wt' );
fprintf(fid,      '<[file][][] //1 <[tmsimulator_scenery_place][][]\n');
fprintf(fid,	  '<[string8]   [type]          [object]>\n');
fprintf(fid,	  '<[string8]   [sname]         []>\n');
fprintf(fid,	  '<[string8]   [lname]         []>\n');
fprintf(fid,	  '<[string8]   [icao]          []>\n');
fprintf(fid,	  '<[string8]	 [country]	     [CH]>\n');
fprintf(fid,      '<[tmvector2d][position] [8.7541 47.3779]>\n');
fprintf(fid,      '<[float64]   [height]        [0]>\n');
fprintf(fid,	  '<[float64]   [size]          [1]>\n');
fprintf(fid,	  '<[vector2_float64][tower_position][]>\n');
fprintf(fid,	  '<[float64]   [tower_height]  [0.0]>\n');
fprintf(fid,	  '<[bool]      [autoheight]    [false]>\n');
fprintf(fid,	  '<[string8]   [coordinate_system][lonlat]>\n\n');
fprintf(fid,	  '     <[tmsimulator_scenery_objecttmslist][objects][]\n');

%i = 1;
n=1;
for i = 1 : gates_lenght_step : steps %parte da 1 cosi come gli indici dello struct
            %---from ned to ecef------------------------------------------%
            %taking NED coordinates of each step on the path and
            %converting them TO ECEF
            %we need to use a positive index and degree is starting from 1 
            %but i is starting from 1
            coord_path_nwu = [coord_clock_Struct.Trajectory(i).x ,...
                              coord_clock_Struct.Trajectory(i).y ,...
                              coord_clock_Struct.Trajectory(i).z];
                          
            tan_path_nwu = [coord_clock_Struct.Trajectory(i).dx ,...
                              coord_clock_Struct.Trajectory(i).dy ,...
                              coord_clock_Struct.Trajectory(i).dz];     
                          
%           coord_path_ned = [0, 0, 0];
%trying to print a gate from know position of aerofly when frlying through it and NED coordinates
%of my model (because s is not changing there)
%           coord_path_ned = [-753.6867, 405.5972, -741];
%           coord_path_ned = [-925.9, 3860, -570];
            coord_path_ecef = (RotationMatrix_NWU2ECEF*(coord_path_nwu.') + (coord_0_ecef.')).';
            x_path_ecef = coord_path_ecef(1);
            y_path_ecef = coord_path_ecef(2);
            z_path_ecef = coord_path_ecef(3);
            %---from ecef to geo------------------------------------------%
            %taking ECEF coordinates of each step on the path and
            %converting them TO GEODETIC
            lon = atan2(y_path_ecef, x_path_ecef);
            rho = norm([x_path_ecef, y_path_ecef]);
            lat_old = atan2(z_path_ecef, (1 - ECC^2) * rho);
            lat_err = inf;
            lat = 0;
            alt = 0;  %ALTITUDINE ECEF SYSTEM NON CAMBIA NIENTE??????????????
            %R_n, alt e lat iterative solution
            while abs(lat_err) > 0.000000000001
                R_n = R_E / sqrt(1 - (ECC * sin(lat_old))^2);
                alt = rho / cos(lat_old) - R_n;
                coeff = R_n / (R_n + alt);
                lat = atan2(z_path_ecef, (1 - ECC^2 * coeff) * rho);

                lat_err = lat - lat_old;
                lat_old = lat;
            end
            %aerofly needs degrees
            %GatesGeodeticCoordinates = [lat, lon,altitude set as we want];
            lat = rad2deg(lat);
            lon = rad2deg(lon);
            
            %new_index = 80;
            %when using this option change angle with orientation when
            %printing gates 
            tmpOrientation = 270 - acosd(coord_clock_Struct.Trajectory(i).dy/sqrt((coord_clock_Struct.Trajectory(i).dx)^2 + (coord_clock_Struct.Trajectory(i).dy)^2));
            if coord_clock_Struct.Trajectory(i).dx < 0
                orientation =  360 - tmpOrientation; 
            else
                orientation =  tmpOrientation;
            end
            debug_o(n) = orientation;

%             tan_path = tan_path_nwu(1) + tan_path_nwu(2) + tan_path_nwu(3);
%             angle = atand(tan_path)
            
            n = n+1;
            
			%---printing each object in the text file---------------------%
            fprintf(fid,	  '<[tmsimulator_scenery_object][element][0]\n');
            fprintf(fid,      '<[string8][type][object]>\n');
            fprintf(fid,      '<[string8][geometry][object_obj]>\n');
            %AEROFLY NEEDS LON LAT PAY ATTENTION I´M USING LAT LON JUST FOR TESTING ON GOOGLE MAPS
            % WARNING : DO NOT CHANGE z component must be the same of the
            % path (local frame z component) plus the height of the local
            % frame to provide an oppropriate pitch feedback (to see it in
            % the proper altitude) 
            %fprintf(fid,      '%s%f %f %f%s\n','<[vector3_float64][position][', lon, lat, 537 + coord_clock_Struct.Trajectory(i).z ,']>') 
            %fprintf(fid,       '%s%f %f %f%s\n','<[vector3_float64][position][', lon, lat - 0.00902, - coord_clock_Struct.Trajectory(i).z ,']>'); 
            fprintf(fid,       '%s%f %f %f%s\n','<[vector3_float64][position][', lon, lat, coord_clock_Struct.Trajectory(i).z,']>'); 
            %fprintf(fid,      '<[float64][orientation][write_constant_here]>\n')
            %fprintf(fid,      '%s%f%s\n', '<[float64][orientation][', write_variable_here , ']>')
            %fprintf(fid,      '%s%f%s\n', '<[float64][orientation][', 360 - (i*360)/steps, ']>')if you work with rad
            %pay attention to the orientation you should probably subtract
            %1 because of the two different loops
            fprintf(fid,       '%s%f%s\n', '<[float64][orientation][', orientation, ']>'); 
            fprintf(fid,       '<[int32][autoheight_override][-1]>\n');
            fprintf(fid,       '>\n');
            %i = i + 1 ;
end

            
            
%---Closing text file-----------------------------------------------------%
fclose(fid);