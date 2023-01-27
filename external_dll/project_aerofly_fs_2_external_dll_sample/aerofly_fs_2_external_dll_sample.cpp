///////////////////////////////////////////////////////////////////////////////////////////////////
//
// file aerofly_fs_2_external_dll_sample.cpp
//
// PLEASE NOTE:  THE INTERFACE IN THIS FILE AND ALL DATA TYPES COULD BE SUBJECT TO SUBSTANTIAL
//               CHANGES WHILE AEROFLY FS 2 IS STILL RECEIVING UPDATES
//
// FURTHER NOTE: This sample just shows you how to read and send messages from the simulation
//               Some sample code is provided so see how to read and send messages
//
// 2019/12/19 - th/mb
//
// ---------------------------------------------------------------------------
//
// copyright (C) 2005-2017, Dr. Torsten Hans, Dr. Marc Borchers
// All rights reserved.
//
// Redistribution  and  use  in  source  and  binary  forms,  with  or  without
// modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of  source code must  retain the above  copyright notice,
//    this list of conditions and the disclaimer below.
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
//    documentation and/or other materials provided with the distribution.
//  - Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
// ARE  DISCLAIMED. 
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(WIN32) || defined(WIN64)
  #if defined(_MSC_VER)
    #pragma warning ( disable : 4530 )  // C++ exception handler used, but unwind semantics are not enabled
    #pragma warning ( disable : 4577 )  // 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc
  #endif
#endif

#include "../shared/input/tm_external_message.h"

#include <windows.h>
#include <thread>
#include <vector>
#include <mutex>

static HINSTANCE global_hDLLinstance = NULL;




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// some ugly macros. we use this to be able to translate from string hash id to string
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#define TM_MESSAGE( a1, a2, a3, a4, a5, a6, a7 )       static tm_external_message Message##a1( ##a2, a3, a4, a5, a6 );
#define TM_MESSAGE_NAME( a1, a2, a3, a4, a5, a6, a7 )  a2,




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// list of messages that can be send/received
// to easy the interpretation of the messages, type, access flags and units are specified
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#define MESSAGE_LIST(F) \
F( AircraftAltitude               , "Aircraft.Altitude"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Meter                        , "altitude as measured by altimeter                                                                            " ) \
F( AircraftVerticalSpeed          , "Aircraft.VerticalSpeed"          , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "vertical speed                                                                                               " ) \
F( AircraftPitch                  , "Aircraft.Pitch"                  , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Radiant                      , "pitch angle                                                                                                  " ) \
F( AircraftBank                   , "Aircraft.Bank"                   , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Radiant                      , "bank angle                                                                                                   " ) \
F( AircraftIndicatedAirspeed      , "Aircraft.IndicatedAirspeed"      , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "indicated airspeed                                                                                           " ) \
F( AircraftGroundSpeed            , "Aircraft.GroundSpeed"            , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "ground speed                                                                                                 " ) \
F( AircraftMagneticHeading        , "Aircraft.MagneticHeading"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Radiant                      , "                                                                                                             " ) \
F( AircraftTrueHeading            , "Aircraft.TrueHeading"            , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Radiant                      , "                                                                                                             " ) \
F( AircraftLatitude               , "Aircraft.Latitude"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Radiant                      , "                                                                                                             " ) \
F( AircraftLongitude              , "Aircraft.Longitude"              , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Radiant                      , "                                                                                                             " ) \
F( AircraftHeight                 , "Aircraft.Height"                 , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Meter                        , "                                                                                                             " ) \
F( AircraftPosition               , "Aircraft.Position"               , tm_msg_data_type::Vector3d     , tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Meter                        , "                                                                                                             " ) \
F( AircraftOrientation            , "Aircraft.Orientation"            , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "                                                                                                             " ) \
F( AircraftVelocity               , "Aircraft.Velocity"               , tm_msg_data_type::Vector3d     , tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "velocity vector         in body system if 'Body' flag is set, in global system otherwise                     " ) \
F( AircraftAngularVelocity        , "Aircraft.AngularVelocity"        , tm_msg_data_type::Vector3d     , tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::RadiantPerSecond             , "angular velocity        in body system if 'Body' flag is set (roll rate pitch rate yaw rate) in global system" ) \
F( AircraftAcceleration           , "Aircraft.Acceleration"           , tm_msg_data_type::Vector3d     , tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecondSquared        , "aircraft acceleration   in body system if 'Body' flag is set, in global system otherwise                     " ) \
F( AircraftGravity                , "Aircraft.Gravity"                , tm_msg_data_type::Vector3d     , tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecondSquared        , "gravity acceleration    in body system if 'Body' flag is set                                                 " ) \
F( AircraftWind                   , "Aircraft.Wind"                   , tm_msg_data_type::Vector3d     , tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "wind vector at current aircraft position                                                                     " ) \
F( AircraftRateOfTurn             , "Aircraft.RateOfTurn"             , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::RadiantPerSecond             , "rate of turn                                                                                                 " ) \
F( AircraftMachNumber             , "Aircraft.MachNumber"             , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "mach number                                                                                                  " ) \
F( AircraftGear                   , "Aircraft.Gear"                   , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "current gear position, zero is up, one is down, in between in transit                                        " ) \
F( AircraftFlaps                  , "Aircraft.Flaps"                  , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "flags                                                                                                        " ) \
F( AircraftThrottle               , "Aircraft.Throttle"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "current throttle setting                                                                                     " ) \
F( AircraftAirBrake               , "Aircraft.AirBrake"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "                                                                                                             " ) \
F( AircraftRadarAltitude          , "Aircraft.RadarAltitude"          , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Meter                        , "                                                                                                             " ) \
F( AircraftName                   , "Aircraft.Name"                   , tm_msg_data_type::String8    ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "current aircraft short name ( name of folder in aircraft directory, eg c172 )                                " ) \
F( AircraftNearestAirport         , "Aircraft.NearestAirport"         , tm_msg_data_type::String8    ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "                                                                                                             " ) \
F( AircraftCategoryJet            , "Aircraft.Category.Jet"           , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "                                                                                                             " ) \
F( AircraftCategoryGlider         , "Aircraft.Category.Glider"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "                                                                                                             " ) \
F( AircraftOnGround               , "Aircraft.OnGround"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "set if aircraft is on ground                                                                                 " ) \
F( AircraftOnRunway               , "Aircraft.OnRunway"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "set if aircraft is on ground and on a runway                                                                 " ) \
F( AircraftCrashed                , "Aircraft.Crashed"                , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "                                                                                                             " ) \
F( PerformanceSpeedVS0            , "Performance.Speed.VS0"           , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "minimum speed with flaps down, lower end of white arc                                                        " ) \
F( PerformanceSpeedVS1            , "Performance.Speed.VS1"           , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "minimum speed with flaps retracted, lower end of green arc                                                   " ) \
F( PerformanceSpeedVFE            , "Performance.Speed.VFE"           , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "maximum speed with flaps extended, upper end of white arc                                                    " ) \
F( PerformanceSpeedVNO            , "Performance.Speed.VNO"           , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "maneuvering speed, lower end of yellow arc                                                                   " ) \
F( PerformanceSpeedVNE            , "Performance.Speed.VNE"           , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "never exceed speed, red line                                                                                 " ) \
F( NavigationNAV1Frequency        , "Navigation.NAV1Frequency"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "NAV1 receiver active frequency                                                                               " ) \
F( NavigationNAV1StandbyFrequency , "Navigation.NAV1StandbyFrequency" , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "NAV1 receiver standby frequency                                                                              " ) \
F( NavigationNAV1FrequencySwap    , "Navigation.NAV1FrequencySwap"    , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "swaps frequencies on NAV1 receiver                                                                           " ) \
F( NavigationDMEFrequency1        , "Navigation.DMEFrequency1"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "DME1 active frequency                                                                                        " ) \
F( NavigationNAV2Frequency        , "Navigation.NAV2Frequency"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "NAV2 receiver active frequency                                                                               " ) \
F( NavigationNAV2StandbyFrequency , "Navigation.NAV2StandbyFrequency" , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "NAV2 receiver standby frequency                                                                              " ) \
F( NavigationNAV2FrequencySwap    , "Navigation.NAV2FrequencySwap"    , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "swaps frequencies on NAV2 receiver                                                                           " ) \
F( NavigationADF1Frequency        , "Navigation.ADF1Frequency"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "ADF1 receiver active frequency                                                                               " ) \
F( NavigationADF1StandbyFrequency , "Navigation.ADF1StandbyFrequency" , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "ADF1 receiver standby frequency                                                                              " ) \
F( NavigationCOM1Frequency        , "Navigation.COM1Frequency"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "COM1 transceiver active frequency                                                                            " ) \
F( NavigationCOM1StandbyFrequency , "Navigation.COM1StandbyFrequency" , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "COM1 transceiver standby frequency                                                                           " ) \
F( NavigationCOM1FrequencySwap    , "Navigation.COM1FrequencySwap"    , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "swaps frequencies on COM1 transceiver                                                                        " ) \
F( NavigationCOM2Frequency        , "Navigation.COM2Frequency"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "COM2 transceiver active frequency                                                                            " ) \
F( NavigationCOM2StandbyFrequency , "Navigation.COM2StandbyFrequency" , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Hertz                        , "COM2 transceiver standby frequency                                                                           " ) \
F( NavigationCOM2FrequencySwap    , "Navigation.COM2FrequencySwap"    , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "swaps frequencies on COM1 transceiver                                                                        " ) \
F( NavigationSelectedCourse1      , "Navigation.SelectedCourse1"      , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Radiant                      , "OBS / selected radial for NAV1 receiver                                                                      " ) \
F( NavigationSelectedCourse2      , "Navigation.SelectedCourse2"      , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Radiant                      , "OBS / selected radial for NAV1 receiver                                                                      " ) \
F( AutopilotMaster                , "Autopilot.Master"                , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( AutopilotDisengage             , "Autopilot.Disengage"             , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "disengage all autopilots                                                                                     " ) \
F( AutopilotHeading               , "Autopilot.Heading"               , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::Radiant                      , "                                                                                                             " ) \
F( AutopilotVerticalSpeed         , "Autopilot.VerticalSpeed"         , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::MeterPerSecond               , "                                                                                                             " ) \
F( AutopilotSelectedSpeed         , "Autopilot.SelectedSpeed"         , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::MeterPerSecond               , "                                                                                                             " ) \
F( AutopilotSelectedAirspeed      , "Autopilot.SelectedAirspeed"      , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::MeterPerSecond               , "autopilot/flight director selected airspeed, speed bug                                                       " ) \
F( AutopilotSelectedHeading       , "Autopilot.SelectedHeading"       , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Radiant                      , "autopilot/flight director selected heading, heading bug                                                      " ) \
F( AutopilotSelectedAltitude      , "Autopilot.SelectedAltitude"      , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::Meter                        , "autopilot/flight director selected altitude                                                                  " ) \
F( AutopilotSelectedVerticalSpeed , "Autopilot.SelectedVerticalSpeed" , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::MeterPerSecond               , "autopilot/flight director selected vertical speed                                                            " ) \
F( AutopilotEngaged               , "Autopilot.Engaged"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "set if autopilot is engaged                                                                                  " ) \
F( AutopilotActiveLateralMode     , "Autopilot.ActiveLateralMode"     , tm_msg_data_type::String8    ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "internal name of the active lateral autopilot/flight director mode                                           " ) \
F( AutopilotArmedLateralMode      , "Autopilot.ArmedLateralMode"      , tm_msg_data_type::String8    ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "internal name of the armed lateral autopilot/flight director mode                                            " ) \
F( AutopilotActiveVerticalMode    , "Autopilot.ActiveVerticalMode"    , tm_msg_data_type::String8    ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "internal name of the active vertical autopilot/flight director mode                                          " ) \
F( AutopilotArmedVerticalMode     , "Autopilot.ArmedVerticalMode"     , tm_msg_data_type::String8    ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "internal name of the armed lateral autopilot/flight director mode                                            " ) \
F( AutopilotActiveAutoThrottleMode, "Autopilot.ActiveAutoThrottleMode", tm_msg_data_type::String8    ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "internal name of the active autothrottle mode                                                                " ) \
F( FlightDirectorPitch            , "FlightDirector.Pitch"            , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Radiant                      , "flight director pitch angle relative to current pitch                                                        " ) \
F( FlightDirectorBank             , "FlightDirector.Bank"             , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Radiant                      , "flight director bank angle relative to current bank                                                          " ) \
F( CopilotHeading                 , "Copilot.Heading"                 , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Radiant                      , "                                                                                                             " ) \
F( CopilotAltitude                , "Copilot.Altitude"                , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::Meter                        , "                                                                                                             " ) \
F( CopilotAirspeed                , "Copilot.Airspeed"                , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "                                                                                                             " ) \
F( CopilotVerticalSpeed           , "Copilot.VerticalSpeed"           , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::MeterPerSecond               , "                                                                                                             " ) \
F( CopilotAileron                 , "Copilot.Aileron"                 , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CopilotElevator                , "Copilot.Elevator"                , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CopilotThrottle                , "Copilot.Throttle"                , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CopilotAutoRudder              , "Copilot.AutoRudder"              , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Read     , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsSpeed                  , "Controls.Speed"                  , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "ignore/do not use  combined throttle, brake and reverse, copilot splits into other                           " ) \
F( ControlsThrottle1              , "Controls.Throttle1"              , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "throttle position for engine 1                                                                               " ) \
F( ControlsThrottle2              , "Controls.Throttle2"              , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "throttle position for engine 2                                                                               " ) \
F( ControlsThrottle3              , "Controls.Throttle3"              , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "throttle position for engine 3                                                                               " ) \
F( ControlsThrottle4              , "Controls.Throttle4"              , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "throttle position for engine 4                                                                               " ) \
F( ControlsThrottle1Move          , "Controls.Throttle1"              , tm_msg_data_type::Double     ,   tm_msg_flag::Move  , tm_msg_access::Write    , tm_msg_unit::PerSecond                    , "throttle rate of change for engine 1                                                                         " ) \
F( ControlsThrottle2Move          , "Controls.Throttle2"              , tm_msg_data_type::Double     ,   tm_msg_flag::Move  , tm_msg_access::Write    , tm_msg_unit::PerSecond                    , "throttle rate of change for engine 2                                                                         " ) \
F( ControlsThrottle3Move          , "Controls.Throttle3"              , tm_msg_data_type::Double     ,   tm_msg_flag::Move  , tm_msg_access::Write    , tm_msg_unit::PerSecond                    , "throttle rate of change for engine 3                                                                         " ) \
F( ControlsThrottle4Move          , "Controls.Throttle4"              , tm_msg_data_type::Double     ,   tm_msg_flag::Move  , tm_msg_access::Write    , tm_msg_unit::PerSecond                    , "throttle rate of change for engine 4                                                                         " ) \
F( ControlsPitchInput             , "Controls.Pitch.Input"            , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsPitchInputOffset       , "Controls.Pitch.Input"            , tm_msg_data_type::Double     ,   tm_msg_flag::Offset, tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsRollInput              , "Controls.Roll.Input"             , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsRollInputOffset        , "Controls.Roll.Input"             , tm_msg_data_type::Double     ,   tm_msg_flag::Offset, tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsYawInput               , "Controls.Yaw.Input"              , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsYawInputActive         , "Controls.Yaw.Input"              , tm_msg_data_type::Double     ,   tm_msg_flag::Active, tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsFlaps                  , "Controls.Flaps"                  , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsFlapsEvent             , "Controls.Flaps"                  , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsGear                   , "Controls.Gear"                   , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsGearToggle             , "Controls.Gear"                   , tm_msg_data_type::Double     ,   tm_msg_flag::Toggle, tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsWheelBrakeLeft         , "Controls.WheelBrake.Left"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsWheelBrakeRight        , "Controls.WheelBrake.Right"       , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsWheelBrakeLeftActive   , "Controls.WheelBrake.Left"        , tm_msg_data_type::Double     ,   tm_msg_flag::Active, tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsWheelBrakeRightActive  , "Controls.WheelBrake.Right"       , tm_msg_data_type::Double     ,   tm_msg_flag::Active, tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsAirBrake               , "Controls.AirBrake"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsAirBrakeActive         , "Controls.AirBrake"               , tm_msg_data_type::Double     ,   tm_msg_flag::Active, tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsGliderAirBrake         , "Controls.GliderAirBrake"         , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsPropellerSpeed1        , "Controls.PropellerSpeed1"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsPropellerSpeed2        , "Controls.PropellerSpeed2"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsPropellerSpeed3        , "Controls.PropellerSpeed3"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsPropellerSpeed4        , "Controls.PropellerSpeed4"        , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsMixture1               , "Controls.Mixture1"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsMixture2               , "Controls.Mixture2"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsMixture3               , "Controls.Mixture3"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsMixture4               , "Controls.Mixture4"               , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsThrustReverse1         , "Controls.ThrustReverse1"         , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsThrustReverse2         , "Controls.ThrustReverse2"         , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsThrustReverse3         , "Controls.ThrustReverse3"         , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsThrustReverse4         , "Controls.ThrustReverse4"         , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsTrim                   , "Controls.Trim"                   , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsTrimStep               , "Controls.Trim"                   , tm_msg_data_type::Double     ,   tm_msg_flag::Step  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsTrimMove               , "Controls.Trim"                   , tm_msg_data_type::Double     ,   tm_msg_flag::Move  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsAileronTrim            , "Controls.AileronTrim"            , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsRudderTrim             , "Controls.RudderTrim"             , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsTiller                 , "Controls.Tiller"                 , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsPedalsDisconnect       , "Controls.PedalsDisconnect"       , tm_msg_data_type::Double     ,   tm_msg_flag::Toggle, tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsNoseWheelSteering      , "Controls.NoseWheelSteering"      , tm_msg_data_type::Double     ,   tm_msg_flag::Toggle, tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsLightingPanel          , "Controls.Lighting.Panel"         , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ControlsLightingInstruments    , "Controls.Lighting.Instruments"   , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewDisplayName                , "View.DisplayName"                , tm_msg_data_type::String8    ,   tm_msg_flag::None  , tm_msg_access::Read     , tm_msg_unit::None                         , "name of current view                                                                                         " ) \
F( ViewInternal                   , "View.Internal"                   , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "set view to last internal view                                                                               " ) \
F( ViewFollow                     , "View.Follow"                     , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "set view to last follow view                                                                                 " ) \
F( ViewExternal                   , "View.External"                   , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "set view to last external view                                                                               " ) \
F( ViewCategory                   , "View.Category"                   , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "change to next / previous view category (internal,follow,external), set last view in this category           " ) \
F( ViewMode                       , "View.Mode"                       , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "set next / previous view in current category                                                                 " ) \
F( ViewZoom                       , "View.Zoom"                       , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewPanHorizontal              , "View.Pan.Horizontal"             , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewPanHorizontalMove          , "View.Pan.Horizontal"             , tm_msg_data_type::Double     ,   tm_msg_flag::Move  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewPanVertical                , "View.Pan.Vertical"               , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewPanVerticalMove            , "View.Pan.Vertical"               , tm_msg_data_type::Double     ,   tm_msg_flag::Move  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewPanCenter                  , "View.Pan.Center"                 , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewLookHorizontal             , "View.Look.Horizontal"            , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "momentarily look left / right                                                                                " ) \
F( ViewLookVertical               , "View.Look.Vertical"              , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "momentarily look up / down                                                                                   " ) \
F( ViewRoll                       , "View.Roll"                       , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewOffsetX                    , "View.OffsetX"                    , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "offset (forward/backward) from view's default position                                                       " ) \
F( ViewOffsetXMove                , "View.OffsetX"                    , tm_msg_data_type::Double     ,   tm_msg_flag::Move  , tm_msg_access::Write    , tm_msg_unit::None                         , "change offset (forward/backward) from view's default position                                                " ) \
F( ViewOffsetY                    , "View.OffsetY"                    , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "lateral offset from view's default position                                                                  " ) \
F( ViewOffsetYMove                , "View.OffsetY"                    , tm_msg_data_type::Double     ,   tm_msg_flag::Move  , tm_msg_access::Write    , tm_msg_unit::None                         , "change lateral offset from view's default position                                                           " ) \
F( ViewOffsetZ                    , "View.OffsetZ"                    , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "vertical offset from view's default position                                                                 " ) \
F( ViewOffsetZMove                , "View.OffsetZ"                    , tm_msg_data_type::Double     ,   tm_msg_flag::Move  , tm_msg_access::Write    , tm_msg_unit::None                         , "change vertical offset from view's default position                                                          " ) \
F( ViewPosition                   , "View.Position"                   , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewDirection                  , "View.Direction"                  , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewUp                         , "View.Up"                         , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewFieldOfView                , "View.FieldOfView"                , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewAspectRatio                , "View.AspectRatio"                , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewFreePosition               , "View.FreePosition"               , tm_msg_data_type::Vector3d     , tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::Meter                        , "the following 4 messages allow you to implement your own view                                                " ) \
F( ViewFreeLookDirection          , "View.FreeLookDirection"          , tm_msg_data_type::Vector3d     , tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewFreeUp                     , "View.FreeUp"                     , tm_msg_data_type::Vector3d     , tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( ViewFreeFieldOfView            , "View.FreeFieldOfView"            , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::Radiant                      , "                                                                                                             " ) \
F( SimulationPause                , "Simulation.Pause"                , tm_msg_data_type::Double     ,   tm_msg_flag::Toggle, tm_msg_access::Write    , tm_msg_unit::None                         , "toggle pause on/off                                                                                          " ) \
F( SimulationFlightInformation    , "Simulation.FlightInformation"    , tm_msg_data_type::Double     ,   tm_msg_flag::Toggle, tm_msg_access::Write    , tm_msg_unit::None                         , "show/hide the flight information at the top of the screen                                                    " ) \
F( SimulationMovingMap            , "Simulation.MovingMap"            , tm_msg_data_type::Double     ,   tm_msg_flag::Toggle, tm_msg_access::Write    , tm_msg_unit::None                         , "show/hide the moving map window                                                                              " ) \
F( SimulationSound                , "Simulation.Sound"                , tm_msg_data_type::Double     ,   tm_msg_flag::Toggle, tm_msg_access::Write    , tm_msg_unit::None                         , "toggle sound on/off                                                                                          " ) \
F( SimulationLiftUp               , "Simulation.LiftUp"               , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "lift up the aircraft from current position                                                                   " ) \
F( SimulationSettingPosition      , "Simulation.SettingPosition"      , tm_msg_data_type::Vector3d     , tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::Meter                        , "                                                                                                             " ) \
F( SimulationSettingOrientation   , "Simulation.SettingOrientation"   , tm_msg_data_type::Vector4d     , tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( SimulationSettingVelocity      , "Simulation.SettingVelocity"      , tm_msg_data_type::Vector3d     , tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::MeterPerSecond               , "                                                                                                             " ) \
F( SimulationSettingSet           , "Simulation.SettingSet"           , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( SimulationTimeChange           , "Simulation.TimeChange"           , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "change time of day                                                                                           " ) \
F( SimulationVisibility           , "Simulation.Visibility"           , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "                                                                                                             " ) \
F( SimulationTime                 , "Simulation.Time"                 , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "                                                                                                             " ) \
F( SimulationUseMouseControl      , "Simulation.UseMouseControl"      , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "                                                                                                             " ) \
F( SimulationPlaybackStart        , "Simulation.PlaybackStart"        , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "start playback if simulation is paused                                                                       " ) \
F( SimulationPlaybackStop         , "Simulation.PlaybackStop"         , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "stop playback                                                                                                " ) \
F( SimulationPlaybackSetPosition  , "Simulation.PlaybackPosition"     , tm_msg_data_type::Double     ,   tm_msg_flag::None  , tm_msg_access::Write    , tm_msg_unit::None                         , "set playback position 0 - 1                                                                                  " ) \
F( SimulationExternalPosition     , "Simulation.ExternalPosition"     , tm_msg_data_type::Vector3d     , tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::Meter                        , "                                                                                                             " ) \
F( SimulationExternalOrientation  , "Simulation.ExternalOrientation"  , tm_msg_data_type::Vector4d     , tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CommandExecute                 , "Command.Execute"                 , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CommandBack                    , "Command.Back"                    , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CommandUp                      , "Command.Up"                      , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CommandDown                    , "Command.Down"                    , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CommandLeft                    , "Command.Left"                    , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CommandRight                   , "Command.Right"                   , tm_msg_data_type::Double     ,   tm_msg_flag::Event , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CommandMoveHorizontal          , "Command.MoveHorizontal"          , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CommandMoveVertical            , "Command.MoveVertical"            , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CommandRotate                  , "Command.Rotate"                  , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( CommandZoom                    , "Command.Zoom"                    , tm_msg_data_type::Double     ,   tm_msg_flag::Value , tm_msg_access::Write    , tm_msg_unit::None                         , "                                                                                                             " ) \
F( AircraftAileron                      , "Aircraft.Aileron"                       , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.Aileron                                                                                             " ) \
F( AircraftAileronForceFree             , "Aircraft.AileronForceFree"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.AileronForceFree                                                                                    " ) \
F( AircraftAileronTrim                  , "Aircraft.AileronTrim"                   , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.AileronTrim                                                                                         " ) \
F( AircraftAngleOfAttack                , "Aircraft.AngleOfAttack"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.AngleOfAttack                                                                                       " ) \
F( AircraftAngularAcceleration          , "Aircraft.AngularAcceleration"           , tm_msg_data_type::Vector3d     , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.AngularAcceleration                                                                                 " ) \
F( AircraftBrakePosition                , "Aircraft.BrakePosition"                 , tm_msg_data_type::Double       , tm_msg_flag::Toggle , tm_msg_access::Write    , tm_msg_unit::None                         , "Aircraft.BrakePosition                                                                                       " ) \
F( AircraftCollective                   , "Aircraft.Collective"                    , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.Collective                                                                                          " ) \
F( AircraftElevator                     , "Aircraft.Elevator"                      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.Elevator                                                                                            " ) \
F( AircraftElevatorForceFree            , "Aircraft.ElevatorForceFree"             , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.ElevatorForceFree                                                                                   " ) \
F( AircraftElevatorForceGradient        , "Aircraft.ElevatorForceGradient"         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.ElevatorForceGradient                                                                               " ) \
F( AircraftIndicatedAirspeedTrend       , "Aircraft.IndicatedAirspeedTrend"        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.IndicatedAirspeedTrend                                                                              " ) \
F( AircraftMass                         , "Aircraft.Mass"                          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.Mass                                                                                                " ) \
F( AircraftPowerSetting                 , "Aircraft.PowerSetting"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.PowerSetting                                                                                        " ) \
F( AircraftRudder                       , "Aircraft.Rudder"                        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.Rudder                                                                                              " ) \
F( AircraftRudderTrim                   , "Aircraft.RudderTrim"                    , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.RudderTrim                                                                                          " ) \
F( AircraftThrottleLimit                , "Aircraft.ThrottleLimit"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.ThrottleLimit                                                                                       " ) \
F( AircraftTrim                         , "Aircraft.Trim"                          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.Trim                                                                                                " ) \
F( AircraftTrimValue                    , "Aircraft.TrimValue"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.TrimValue                                                                                           " ) \
F( AircraftYawDamperEnabled             , "Aircraft.YawDamperEnabled"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Aircraft.YawDamperEnabled                                                                                    " ) \
F( AutopilotAileron                     , "Autopilot.Aileron"                      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Aileron                                                                                            " ) \
F( AutopilotAltitude                    , "Autopilot.Altitude"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Altitude                                                                                           " ) \
F( AutopilotAltitudeArm                 , "Autopilot.AltitudeArm"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.AltitudeArm                                                                                        " ) \
F( AutopilotAltitudeHold                , "Autopilot.AltitudeHold"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.AltitudeHold                                                                                       " ) \
F( AutopilotAltitudeIntervention        , "Autopilot.AltitudeIntervention"         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.AltitudeIntervention                                                                               " ) \
F( AutopilotAltitudeScale               , "Autopilot.AltitudeScale"                , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.AltitudeScale                                                                                      " ) \
F( AutopilotApproach                    , "Autopilot.Approach"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Approach                                                                                           " ) \
F( AutopilotArmedApproachMode           , "Autopilot.ArmedApproachMode"            , tm_msg_data_type::String8      , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.ArmedApproachMode                                                                                  " ) \
F( AutopilotBackCourse                  , "Autopilot.BackCourse"                   , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Autopilot.BackCourse                                                                                         " ) \
F( AutopilotBankLimit                   , "Autopilot.BankLimit"                    , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.BankLimit                                                                                          " ) \
F( AutopilotDN                          , "Autopilot.DN"                           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.DN                                                                                                 " ) \
F( AutopilotElevator                    , "Autopilot.Elevator"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Elevator                                                                                           " ) \
F( AutopilotExpedite                    , "Autopilot.Expedite"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Expedite                                                                                           " ) \
F( AutopilotFlightDirector              , "Autopilot.FlightDirector"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.FlightDirector                                                                                     " ) \
F( AutopilotFlightDirector0             , "Autopilot.FlightDirector0"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.FlightDirector0                                                                                    " ) \
F( AutopilotFlightDirector1             , "Autopilot.FlightDirector1"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.FlightDirector1                                                                                    " ) \
F( AutopilotFlightDirectorClear         , "Autopilot.FlightDirectorClear"          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.FlightDirectorClear                                                                                " ) \
F( AutopilotFlightDirectorClear0        , "Autopilot.FlightDirectorClear0"         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.FlightDirectorClear0                                                                               " ) \
F( AutopilotFlightDirectorClear1        , "Autopilot.FlightDirectorClear1"         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.FlightDirectorClear1                                                                               " ) \
F( AutopilotFlightLevelChange           , "Autopilot.FlightLevelChange"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.FlightLevelChange                                                                                  " ) \
F( AutopilotHeadingHold                 , "Autopilot.HeadingHold"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Autopilot.HeadingHold                                                                                        " ) \
F( AutopilotLocalizer                   , "Autopilot.Localizer"                    , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Localizer                                                                                          " ) \
F( AutopilotManagedAltitude             , "Autopilot.ManagedAltitude"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.ManagedAltitude                                                                                    " ) \
F( AutopilotManagedHeading              , "Autopilot.ManagedHeading"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Autopilot.ManagedHeading                                                                                     " ) \
F( AutopilotManagedSpeed                , "Autopilot.ManagedSpeed"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.ManagedSpeed                                                                                       " ) \
F( AutopilotMaster0                     , "Autopilot.Master0"                      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Master0                                                                                            " ) \
F( AutopilotMaster1                     , "Autopilot.Master1"                      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Master1                                                                                            " ) \
F( AutopilotMaster2                     , "Autopilot.Master2"                      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Master2                                                                                            " ) \
F( AutopilotNavigation                  , "Autopilot.Navigation"                   , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Navigation                                                                                         " ) \
F( AutopilotNavigationSourceCopilot     , "Autopilot.NavigationSourceCopilot"      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.NavigationSourceCopilot                                                                            " ) \
F( AutopilotNavigationSourcePilot       , "Autopilot.NavigationSourcePilot"        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.NavigationSourcePilot                                                                              " ) \
F( AutopilotReverse                     , "Autopilot.Reverse"                      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Reverse                                                                                            " ) \
F( AutopilotRudder                      , "Autopilot.Rudder"                       , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Rudder                                                                                             " ) \
F( AutopilotSelectedAltitudeScale       , "Autopilot.SelectedAltitudeScale"        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.SelectedAltitudeScale                                                                              " ) \
F( AutopilotSelectedChange              , "Autopilot.SelectedChange"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.SelectedChange                                                                                     " ) \
F( AutopilotSelectedMachNumber          , "Autopilot.SelectedMachNumber"           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.SelectedMachNumber                                                                                 " ) \
F( AutopilotSelectedVerticalSpeedZero   , "Autopilot.SelectedVerticalSpeedZero"    , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.SelectedVerticalSpeedZero                                                                          " ) \
F( AutopilotSpeed                       , "Autopilot.Speed"                        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Speed                                                                                              " ) \
F( AutopilotSpeedIntervention           , "Autopilot.SpeedIntervention"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.SpeedIntervention                                                                                  " ) \
F( AutopilotSpeedManaged                , "Autopilot.SpeedManaged"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.SpeedManaged                                                                                       " ) \
F( AutopilotTakeOffGoAround             , "Autopilot.TakeOffGoAround"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.TakeOffGoAround                                                                                    " ) \
F( AutopilotThrottle                    , "Autopilot.Throttle"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Throttle                                                                                           " ) \
F( AutopilotThrottleArm                 , "Autopilot.ThrottleArm"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.ThrottleArm                                                                                        " ) \
F( AutopilotThrottleCommand             , "Autopilot.ThrottleCommand"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Autopilot.ThrottleCommand                                                                                    " ) \
F( AutopilotThrottleDisengage           , "Autopilot.ThrottleDisengage"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.ThrottleDisengage                                                                                  " ) \
F( AutopilotThrottleEngaged             , "Autopilot.ThrottleEngaged"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.ThrottleEngaged                                                                                    " ) \
F( AutopilotThrust                      , "Autopilot.Thrust"                       , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Thrust                                                                                             " ) \
F( AutopilotTouchControlSteering        , "Autopilot.TouchControlSteering"         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.TouchControlSteering                                                                               " ) \
F( AutopilotType                        , "Autopilot.Type"                         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Type                                                                                               " ) \
F( AutopilotUP                          , "Autopilot.UP"                           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.UP                                                                                                 " ) \
F( AutopilotUseFlightPathAngle          , "Autopilot.UseFlightPathAngle"           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.UseFlightPathAngle                                                                                 " ) \
F( AutopilotUseMachNumber               , "Autopilot.UseMachNumber"                , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.UseMachNumber                                                                                      " ) \
F( AutopilotUseTrack                    , "Autopilot.UseTrack"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.UseTrack                                                                                           " ) \
F( AutopilotUseTrackFlightPathAngle     , "Autopilot.UseTrackFlightPathAngle"      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.UseTrackFlightPathAngle                                                                            " ) \
F( AutopilotVertical                    , "Autopilot.Vertical"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.Vertical                                                                                           " ) \
F( AutopilotVerticalNavigation          , "Autopilot.VerticalNavigation"           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Autopilot.VerticalNavigation                                                                                 " ) \
F( CommandSelectHorizontal              , "Command.SelectHorizontal"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Command.SelectHorizontal                                                                                     " ) \
F( CommandSelectVertical                , "Command.SelectVertical"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Command.SelectVertical                                                                                       " ) \
F( CommandVirtualRealityCenter          , "Command.VirtualRealityCenter"           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Command.VirtualRealityCenter                                                                                 " ) \
F( CommunicationATCGo                   , "Communication.ATCGo"                    , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Communication.ATCGo                                                                                          " ) \
F( CommunicationATCNegative             , "Communication.ATCNegative"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Communication.ATCNegative                                                                                    " ) \
F( CommunicationCOM1Frequency           , "Communication.COM1Frequency"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Communication.COM1Frequency                                                                                  " ) \
F( CommunicationCOM1FrequencySwap       , "Communication.COM1FrequencySwap"        , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Communication.COM1FrequencySwap                                                                              " ) \
F( CommunicationCOM1StandbyFrequency    , "Communication.COM1StandbyFrequency"     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Communication.COM1StandbyFrequency                                                                           " ) \
F( CommunicationCOM2Frequency           , "Communication.COM2Frequency"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Communication.COM2Frequency                                                                                  " ) \
F( CommunicationCOM2FrequencySwap       , "Communication.COM2FrequencySwap"        , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Communication.COM2FrequencySwap                                                                              " ) \
F( CommunicationCOM2StandbyFrequency    , "Communication.COM2StandbyFrequency"     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Communication.COM2StandbyFrequency                                                                           " ) \
F( CommunicationTransponderClear        , "Communication.TransponderClear"         , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Communication.TransponderClear                                                                               " ) \
F( CommunicationTransponderCode         , "Communication.TransponderCode"          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Communication.TransponderCode                                                                                " ) \
F( CommunicationTransponderCursor       , "Communication.TransponderCursor"        , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Communication.TransponderCursor                                                                              " ) \
F( CommunicationTransponderDigit        , "Communication.TransponderDigit"         , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Communication.TransponderDigit                                                                               " ) \
F( CommunicationTransponderVFR          , "Communication.TransponderVFR"           , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Communication.TransponderVFR                                                                                 " ) \
F( ControlsAirBrakeArm                  , "Controls.AirBrake.Arm"                  , tm_msg_data_type::Double       , tm_msg_flag::Toggle , tm_msg_access::Write    , tm_msg_unit::None                         , "Controls.AirBrake.Arm                                                                                        " ) \
F( ControlsAutoBrakeRejectedTakeOff     , "Controls.AutoBrake.RejectedTakeOff"     , tm_msg_data_type::Double       , tm_msg_flag::Toggle , tm_msg_access::Write    , tm_msg_unit::None                         , "Controls.AutoBrake.RejectedTakeOff                                                                           " ) \
F( ControlsAutoBrakeStrength            , "Controls.AutoBrake.Strength"            , tm_msg_data_type::Double       , tm_msg_flag::Toggle , tm_msg_access::Write    , tm_msg_unit::None                         , "Controls.AutoBrake.Strength                                                                                  " ) \
F( ControlsClutch                       , "Controls.Clutch"                        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Clutch                                                                                              " ) \
F( ControlsCollective                   , "Controls.Collective"                    , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Collective                                                                                          " ) \
F( ControlsCoolingCowl                  , "Controls.CoolingCowl"                   , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.CoolingCowl                                                                                         " ) \
F( ControlsCyclicPitch                  , "Controls.CyclicPitch"                   , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.CyclicPitch                                                                                         " ) \
F( ControlsCyclicRoll                   , "Controls.CyclicRoll"                    , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.CyclicRoll                                                                                          " ) \
F( ControlsFuelPump                     , "Controls.FuelPump"                      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.FuelPump                                                                                            " ) \
F( ControlsGovernor                     , "Controls.Governor"                      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Governor                                                                                            " ) \
F( ControlsHelicopterThrottle           , "Controls.HelicopterThrottle"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.HelicopterThrottle                                                                                  " ) \
F( ControlsHook                         , "Controls.Hook"                          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Hook                                                                                                " ) \
F( ControlsLightingBeacon               , "Controls.Lighting.Beacon"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Lighting.Beacon                                                                                     " ) \
F( ControlsLightingInboardLanding       , "Controls.Lighting.InboardLanding"       , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Lighting.InboardLanding                                                                             " ) \
F( ControlsLightingLanding              , "Controls.Lighting.Landing"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Lighting.Landing                                                                                    " ) \
F( ControlsLightingNavigation           , "Controls.Lighting.Navigation"           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Lighting.Navigation                                                                                 " ) \
F( ControlsLightingOutboardLanding      , "Controls.Lighting.OutboardLanding"      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Lighting.OutboardLanding                                                                            " ) \
F( ControlsLightingStrobe               , "Controls.Lighting.Strobe"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Lighting.Strobe                                                                                     " ) \
F( ControlsLightingTaxi                 , "Controls.Lighting.Taxi"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Lighting.Taxi                                                                                       " ) \
F( ControlsMagnetos                     , "Controls.Magnetos"                      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.Magnetos                                                                                            " ) \
F( ControlsMirrorTiltLeftRight          , "Controls.MirrorTiltLeftRight"           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.MirrorTiltLeftRight                                                                                 " ) \
F( ControlsMirrorTiltUpDown             , "Controls.MirrorTiltUpDown"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.MirrorTiltUpDown                                                                                    " ) \
F( ControlsParkingBrake                 , "Controls.ParkingBrake"                  , tm_msg_data_type::Double       , tm_msg_flag::Toggle , tm_msg_access::Write    , tm_msg_unit::None                         , "Controls.ParkingBrake                                                                                        " ) \
F( ControlsPitotHeat                    , "Controls.PitotHeat"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.PitotHeat                                                                                           " ) \
F( ControlsRemoteHook                   , "Controls.RemoteHook"                    , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.RemoteHook                                                                                          " ) \
F( ControlsRotorBrake                   , "Controls.RotorBrake"                    , tm_msg_data_type::Double       , tm_msg_flag::Toggle , tm_msg_access::Write    , tm_msg_unit::None                         , "Controls.RotorBrake                                                                                          " ) \
F( ControlsSteeringRange                , "Controls.SteeringRange"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.SteeringRange                                                                                       " ) \
F( ControlsTailRotor                    , "Controls.TailRotor"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.TailRotor                                                                                           " ) \
F( ControlsTailWheelSteering            , "Controls.TailWheelSteering"             , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Controls.TailWheelSteering                                                                                   " ) \
F( CopilotAutoAirBrake                  , "Copilot.AutoAirBrake"                   , tm_msg_data_type::Double       , tm_msg_flag::Toggle , tm_msg_access::Write    , tm_msg_unit::None                         , "Copilot.AutoAirBrake                                                                                         " ) \
F( CopilotAutoBrake                     , "Copilot.AutoBrake"                      , tm_msg_data_type::Double       , tm_msg_flag::Toggle , tm_msg_access::Write    , tm_msg_unit::None                         , "Copilot.AutoBrake                                                                                            " ) \
F( CopilotAutoCheckList                 , "Copilot.AutoCheckList"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.AutoCheckList                                                                                        " ) \
F( CopilotAutoEngine                    , "Copilot.AutoEngine"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.AutoEngine                                                                                           " ) \
F( CopilotAutoFlap                      , "Copilot.AutoFlap"                       , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.AutoFlap                                                                                             " ) \
F( CopilotAutoGear                      , "Copilot.AutoGear"                       , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.AutoGear                                                                                             " ) \
F( CopilotAutoTrim                      , "Copilot.AutoTrim"                       , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.AutoTrim                                                                                             " ) \
F( CopilotAutoTuning                    , "Copilot.AutoTuning"                     , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.AutoTuning                                                                                           " ) \
F( CopilotAutoWarningMuteTime           , "Copilot.AutoWarningMuteTime"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.AutoWarningMuteTime                                                                                  " ) \
F( CopilotBank                          , "Copilot.Bank"                           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Bank                                                                                                 " ) \
F( CopilotCenter                        , "Copilot.Center"                         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Center                                                                                               " ) \
F( CopilotDown                          , "Copilot.Down"                           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Down                                                                                                 " ) \
F( CopilotEngaged                       , "Copilot.Engaged"                        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Engaged                                                                                              " ) \
F( CopilotFaster                        , "Copilot.Faster"                         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Faster                                                                                               " ) \
F( CopilotLanding                       , "Copilot.Landing"                        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Landing                                                                                              " ) \
F( CopilotLandingAvailable              , "Copilot.LandingAvailable"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.LandingAvailable                                                                                     " ) \
F( CopilotLateralMode                   , "Copilot.LateralMode"                    , tm_msg_data_type::String8      , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.LateralMode                                                                                          " ) \
F( CopilotLeft                          , "Copilot.Left"                           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Left                                                                                                 " ) \
F( CopilotMaster                        , "Copilot.Master"                         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Master                                                                                               " ) \
F( CopilotPitch                         , "Copilot.Pitch"                          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Pitch                                                                                                " ) \
F( CopilotProfi                         , "Copilot.Profi"                          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Profi                                                                                                " ) \
F( CopilotRight                         , "Copilot.Right"                          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Right                                                                                                " ) \
F( CopilotRoute                         , "Copilot.Route"                          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Route                                                                                                " ) \
F( CopilotRouteAvailable                , "Copilot.RouteAvailable"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.RouteAvailable                                                                                       " ) \
F( CopilotRudder                        , "Copilot.Rudder"                         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Rudder                                                                                               " ) \
F( CopilotRudderAssist                  , "Copilot.RudderAssist"                   , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.RudderAssist                                                                                         " ) \
F( CopilotSelectedAirspeed              , "Copilot.SelectedAirspeed"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.SelectedAirspeed                                                                                     " ) \
F( CopilotSelectedAltitude              , "Copilot.SelectedAltitude"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.SelectedAltitude                                                                                     " ) \
F( CopilotSelectedHeading               , "Copilot.SelectedHeading"                , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Copilot.SelectedHeading                                                                                      " ) \
F( CopilotSelectedVerticalSpeed         , "Copilot.SelectedVerticalSpeed"          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.SelectedVerticalSpeed                                                                                " ) \
F( CopilotSlower                        , "Copilot.Slower"                         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Slower                                                                                               " ) \
F( CopilotTakeoff                       , "Copilot.Takeoff"                        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Takeoff                                                                                              " ) \
F( CopilotTakeoffAvailable              , "Copilot.TakeoffAvailable"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.TakeoffAvailable                                                                                     " ) \
F( CopilotThrottleMode                  , "Copilot.ThrottleMode"                   , tm_msg_data_type::String8      , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.ThrottleMode                                                                                         " ) \
F( CopilotUp                            , "Copilot.Up"                             , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.Up                                                                                                   " ) \
F( CopilotVerticalMode                  , "Copilot.VerticalMode"                   , tm_msg_data_type::String8      , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Copilot.VerticalMode                                                                                         " ) \
F( DescriptionHintAileronGain           , "Description.Hint.AileronGain"           , tm_msg_data_type::String8      , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Description.Hint.AileronGain                                                                                 " ) \
F( DescriptionHintElevatorGain          , "Description.Hint.ElevatorGain"          , tm_msg_data_type::String8      , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Description.Hint.ElevatorGain                                                                                " ) \
F( DescriptionHintRudderGain            , "Description.Hint.RudderGain"            , tm_msg_data_type::String8      , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Description.Hint.RudderGain                                                                                  " ) \
F( DescriptionPerformanceMaximumSpeed   , "Description.Performance.MaximumSpeed"   , tm_msg_data_type::String8      , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Description.Performance.MaximumSpeed                                                                         " ) \
F( DescriptionPerformanceMinimumSpeed   , "Description.Performance.MinimumSpeed"   , tm_msg_data_type::String8      , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Description.Performance.MinimumSpeed                                                                         " ) \
F( GameCalibration                      , "Game.Calibration"                       , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.Calibration                                                                                             " ) \
F( GameDeveloperRepositionHeading       , "Game.Developer.Reposition.Heading"      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Game.Developer.Reposition.Heading                                                                            " ) \
F( GameDeveloperRepositionR             , "Game.Developer.Reposition.R"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.Developer.Reposition.R                                                                                  " ) \
F( GameDeveloperRepositionSpeed         , "Game.Developer.Reposition.Speed"        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.Developer.Reposition.Speed                                                                              " ) \
F( GameDeveloperShowGeometry            , "Game.Developer.ShowGeometry"            , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Game.Developer.ShowGeometry                                                                                  " ) \
F( GameFlightDirectorAltitude           , "Game.FlightDirector.Altitude"           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDirector.Altitude                                                                                 " ) \
F( GameFlightDirectorBank               , "Game.FlightDirector.Bank"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDirector.Bank                                                                                     " ) \
F( GameFlightDirectorBrake              , "Game.FlightDirector.Brake"              , tm_msg_data_type::Double       , tm_msg_flag::Toggle , tm_msg_access::Write    , tm_msg_unit::None                         , "Game.FlightDirector.Brake                                                                                    " ) \
F( GameFlightDirectorFlaps              , "Game.FlightDirector.Flaps"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDirector.Flaps                                                                                    " ) \
F( GameFlightDirectorGear               , "Game.FlightDirector.Gear"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDirector.Gear                                                                                     " ) \
F( GameFlightDirectorHeading            , "Game.FlightDirector.Heading"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Game.FlightDirector.Heading                                                                                  " ) \
F( GameFlightDirectorPitch              , "Game.FlightDirector.Pitch"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDirector.Pitch                                                                                    " ) \
F( GameFlightDirectorPower              , "Game.FlightDirector.Power"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDirector.Power                                                                                    " ) \
F( GameFlightDirectorSpeed              , "Game.FlightDirector.Speed"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDirector.Speed                                                                                    " ) \
F( GameFlightDisplayAltitude            , "Game.FlightDisplay.Altitude"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDisplay.Altitude                                                                                  " ) \
F( GameFlightDisplayAttitude            , "Game.FlightDisplay.Attitude"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDisplay.Attitude                                                                                  " ) \
F( GameFlightDisplayFocus               , "Game.FlightDisplay.Focus"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDisplay.Focus                                                                                     " ) \
F( GameFlightDisplaySpeed               , "Game.FlightDisplay.Speed"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.FlightDisplay.Speed                                                                                     " ) \
F( GameMission                          , "Game.Mission"                           , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.Mission                                                                                                 " ) \
F( GameMissionControl                   , "Game.Mission.Control"                   , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.Mission.Control                                                                                         " ) \
F( GameMissionFinished                  , "Game.Mission.Finished"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.Mission.Finished                                                                                        " ) \
F( GameMissionHideAutopilot             , "Game.Mission.HideAutopilot"             , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Game.Mission.HideAutopilot                                                                                   " ) \
F( GameMissionHideControls              , "Game.Mission.HideControls"              , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Game.Mission.HideControls                                                                                    " ) \
F( GameMissionShowHelpButton            , "Game.Mission.ShowHelpButton"            , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Game.Mission.ShowHelpButton                                                                                  " ) \
F( GamePause                            , "Game.Pause"                             , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.Pause                                                                                                   " ) \
F( GameTiltControl                      , "Game.TiltControl"                       , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Game.TiltControl                                                                                             " ) \
F( NavigationADF1FrequencySwap          , "Navigation.ADF1FrequencySwap"           , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Navigation.ADF1FrequencySwap                                                                                 " ) \
F( NavigationADF2Frequency              , "Navigation.ADF2Frequency"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Navigation.ADF2Frequency                                                                                     " ) \
F( NavigationADF2FrequencySwap          , "Navigation.ADF2FrequencySwap"           , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Navigation.ADF2FrequencySwap                                                                                 " ) \
F( NavigationADF2StandbyFrequency       , "Navigation.ADF2StandbyFrequency"        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Navigation.ADF2StandbyFrequency                                                                              " ) \
F( NavigationDMEFrequency               , "Navigation.DMEFrequency"                , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "DME transceiver active frequency                                                                             " ) \
F( NavigationDMEStandbyFrequency        , "Navigation.DMEStandbyFrequency"         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "DME transceiver standby frequency (appears to be same as active in c172                                      " ) \
F( NavigationDME1Distance               , "Navigation.DME1Distance"                , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Navigation.DME1Distance                                                                                      " ) \
F( NavigationDME2Distance               , "Navigation.DME2Distance"                , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Navigation.DME2Distance                                                                                      " ) \
F( NavigationILS1Course                 , "Navigation.ILS1Course"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Navigation.ILS1Course                                                                                        " ) \
F( NavigationILS1Frequency              , "Navigation.ILS1Frequency"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Navigation.ILS1Frequency                                                                                     " ) \
F( NavigationILS1FrequencySwap          , "Navigation.ILS1FrequencySwap"           , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Navigation.ILS1FrequencySwap                                                                                 " ) \
F( NavigationILS1StandbyFrequency       , "Navigation.ILS1StandbyFrequency"        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Navigation.ILS1StandbyFrequency                                                                              " ) \
F( NavigationILS2Course                 , "Navigation.ILS2Course"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Navigation.ILS2Course                                                                                        " ) \
F( NavigationILS2Frequency              , "Navigation.ILS2Frequency"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Navigation.ILS2Frequency                                                                                     " ) \
F( NavigationILS2FrequencySwap          , "Navigation.ILS2FrequencySwap"           , tm_msg_data_type::Double       , tm_msg_flag::Event  , tm_msg_access::Write    , tm_msg_unit::None                         , "Navigation.ILS2FrequencySwap                                                                                 " ) \
F( NavigationILS2StandbyFrequency       , "Navigation.ILS2StandbyFrequency"        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::ReadWrite, tm_msg_unit::None                         , "Navigation.ILS2StandbyFrequency                                                                              " ) \
F( NavigationNAV1Identifier             , "Navigation.NAV1Identifier"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Navigation.NAV1Identifier                                                                                    " ) \
F( PerformanceSpeedMaximum              , "Performance.Speed.Maximum"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Performance.Speed.Maximum                                                                                    " ) \
F( PerformanceSpeedMaximumFlapExtension , "Performance.Speed.MaximumFlapExtension" , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Performance.Speed.MaximumFlapExtension                                                                       " ) \
F( PerformanceSpeedMinimum              , "Performance.Speed.Minimum"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Performance.Speed.Minimum                                                                                    " ) \
F( PerformanceSpeedMinimumFlapRetraction, "Performance.Speed.MinimumFlapRetraction", tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Performance.Speed.MinimumFlapRetraction                                                                      " ) \
F( SimulationHUD                        , "Simulation.HUD"                         , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.HUD                                                                                               " ) \
F( SimulationLandmarks                  , "Simulation.Landmarks"                   , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.Landmarks                                                                                         " ) \
F( SimulationMonitoring                 , "Simulation.Monitoring"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.Monitoring                                                                                        " ) \
F( SimulationOverlaySelection           , "Simulation.OverlaySelection"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.OverlaySelection                                                                                  " ) \
F( SimulationParameter0                 , "Simulation.Parameter0"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.Parameter0                                                                                        " ) \
F( SimulationParameter1                 , "Simulation.Parameter1"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.Parameter1                                                                                        " ) \
F( SimulationParameter2                 , "Simulation.Parameter2"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.Parameter2                                                                                        " ) \
F( SimulationParameter3                 , "Simulation.Parameter3"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.Parameter3                                                                                        " ) \
F( SimulationPlaybackBack               , "Simulation.PlaybackBack"                , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.PlaybackBack                                                                                      " ) \
F( SimulationPlaybackPosition           , "Simulation.PlaybackPosition"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.PlaybackPosition                                                                                  " ) \
F( SimulationReloadAircraft             , "Simulation.ReloadAircraft"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.ReloadAircraft                                                                                    " ) \
F( SimulationReposition                 , "Simulation.Reposition"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.Reposition                                                                                        " ) \
F( SimulationShowControlObjects         , "Simulation.ShowControlObjects"          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.ShowControlObjects                                                                                " ) \
F( SimulationTakeSceenshot              , "Simulation.TakeSceenshot"               , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.TakeSceenshot                                                                                     " ) \
F( SimulationTakeScreenshot             , "Simulation.TakeScreenshot"              , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.TakeScreenshot                                                                                    " ) \
F( SimulationTakeScreenshotNoInfo       , "Simulation.TakeScreenshotNoInfo"        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Write    , tm_msg_unit::None                         , "Simulation.TakeScreenshotNoInfo                                                                              " ) \
F( SimulatorOverlayLeft                 , "Simulator.OverlayLeft"                  , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Simulator.OverlayLeft                                                                                        " ) \
F( SimulatorOverlayRight                , "Simulator.OverlayRight"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Simulator.OverlayRight                                                                                       " ) \
F( ViewAltitude                         , "View.Altitude"                          , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "View.Altitude                                                                                                " ) \
F( ViewGroundHeight                     , "View.GroundHeight"                      , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "View.GroundHeight                                                                                            " ) \
F( ViewOption                           , "View.Option"                            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "View.Option                                                                                                  " ) \
F( ViewPostOffset                       , "View.PostOffset"                        , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "View.PostOffset                                                                                              " ) \
F( WarningsMasterWarningPilot           , "Warnings.MasterWarningPilot"            , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Warnings.MasterWarningPilot                                                                                  " ) \
F( WarningsWarningActive                , "Warnings.WarningActive"                 , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Warnings.WarningActive                                                                                       " ) \
F( WarningsWarningMute                  , "Warnings.WarningMute"                   , tm_msg_data_type::Double       , tm_msg_flag::Value  , tm_msg_access::Read     , tm_msg_unit::None                         , "Warnings.WarningMute                                                                                         " )
MESSAGE_LIST( TM_MESSAGE )




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// a small helper function that shows the name of a message as plain text if an ID is passed
//
//////////////////////////////////////////////////////////////////////////////////////////////////
struct tm_message_type
{
  tm_string       String;
  tm_string_hash  StringHash;
  template <tm_uint32 N> constexpr tm_message_type( const char( &str )[N] ) : String{ str }, StringHash{ str } { }
};

static std::vector<tm_message_type> MessageTypeList = 
{
  MESSAGE_LIST( TM_MESSAGE_NAME )
};

static tm_string GetMessageName( const tm_external_message &message )
{
  for( const auto &mt : MessageTypeList )
  {
    if( message.GetID() == mt.StringHash.GetHash() ) { return mt.String; }
  }

  return tm_string( "unknown" );
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// code that opens a window and displays the received messages as text
// its meant just as a simple helper to get started with the DLL
//
// THIS CODE SHOULD NOT BE USED IN A PRODUCTION DLL
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <gdiplus.h>
#pragma message("including lib: gdiplus.lib")
#pragma comment(lib, "gdiplus.lib")

static ULONG_PTR                         Global_DebugOutput_gdiplusToken = NULL;
static std::thread                       Global_DebugOutput_Thread;
static HWND                              Global_DebugOutput_Window = NULL;
static bool                              Global_DebugOutput_WindowCloseMessage = false;

static std::vector<tm_external_message>  MessageListReceive;
static std::vector<tm_external_message>  MessageListCopy;
static std::vector<tm_external_message>  MessageListDebugOutput;
static std::mutex                        MessageListMutex;
static double                            MessageDeltaTime = 0;

const int SAMPLE_WINDOW_WIDTH  = 640;
const int SAMPLE_WINDOW_HEIGHT = 1080;

void DebugOutput_Draw( HDC hDC )
{
  // clear and draw to a bitmap instead to the hdc directly to avoid flicker
  Gdiplus::Bitmap     backbuffer( SAMPLE_WINDOW_WIDTH, SAMPLE_WINDOW_HEIGHT, PixelFormat24bppRGB );
  Gdiplus::Graphics   graphics( &backbuffer );
  Gdiplus::SolidBrush black( Gdiplus::Color( 255, 0, 0, 0 ) );
  Gdiplus::FontFamily fontFamily( L"Courier New" );
  Gdiplus::Font       font( &fontFamily, 11, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel );
  Gdiplus::Color      clearcolor( 255, 220, 232, 244 );

  graphics.Clear( clearcolor );

  {
    std::lock_guard<std::mutex> lock_guard{ MessageListMutex };
    MessageListDebugOutput.swap( MessageListCopy );
  }


  {
    float y = 4;
    WCHAR text1[256];
    WCHAR text2[256];

    static size_t JustACounter = 0;
    _snwprintf_s( &text1[0], 255, _TRUNCATE, L"messages = %llu  dt=%f  counter=%llu", MessageListDebugOutput.size(), MessageDeltaTime, ++JustACounter );
    graphics.DrawString( &text1[0], -1, &font, Gdiplus::PointF( 10, y ), &black );
    y += 16;

    int index = 0;
    for ( auto &message : MessageListDebugOutput )
    {
      switch ( message.GetDataType() )
      {
        case tm_msg_data_type::None:     {                                       _snwprintf_s( &text1[0], 255, _TRUNCATE, L"void" );                                        } break;
        case tm_msg_data_type::Int:      { const auto v = message.GetInt();      _snwprintf_s( &text1[0], 255, _TRUNCATE, L"v=%lld", v );                                   } break;
        case tm_msg_data_type::Double:   { const auto v = message.GetDouble();   _snwprintf_s( &text1[0], 255, _TRUNCATE, L"v=%.3f", v );                                   } break;
        case tm_msg_data_type::Vector2d: { const auto v = message.GetVector2d(); _snwprintf_s( &text1[0], 255, _TRUNCATE, L"v=(%.3f %.3f)", v.x, v.y );                     } break;
        case tm_msg_data_type::Vector3d: { const auto v = message.GetVector3d(); _snwprintf_s( &text1[0], 255, _TRUNCATE, L"v=(%.3f %.3f %.3f)", v.x, v.y, v.z );           } break;
        case tm_msg_data_type::Vector4d: { const auto v = message.GetVector4d(); _snwprintf_s( &text1[0], 255, _TRUNCATE, L"v=(%.3f %.3f %.3f %.3f)", v.x, v.y, v.z, v.w ); } break;
        case tm_msg_data_type::String8:  { const auto v = message.GetString();   _snwprintf_s( &text1[0], 255, _TRUNCATE, L"s='%hs'", v.c_str() );                          } break;
      }
      
      //_snwprintf_s( &text2[0], 255, _TRUNCATE, L"%3u:  size=%u", ++index, message.GetMessageSize() );
      _snwprintf_s( &text2[0], 255, _TRUNCATE, L"%3u:", ++index );
      graphics.DrawString( &text2[0], -1, &font, Gdiplus::PointF(  0, y ), &black );

      graphics.DrawString( &text1[0], -1, &font, Gdiplus::PointF( 40, y ), &black );
      
      auto message_name = GetMessageName( message );
      _snwprintf_s( &text1[0], 255, _TRUNCATE, L"'%hs'  flags=%llu", message_name.c_str(), message.GetFlags().GetFlags() );
      graphics.DrawString( &text1[0], -1, &font, Gdiplus::PointF( 300, y ), &black );
      
      y += 16;
      if ( y > SAMPLE_WINDOW_HEIGHT )
        break;
    }
  }

  // copy 'backbuffer' image to screen
  Gdiplus::Graphics graphics_final( hDC );
  graphics_final.DrawImage( &backbuffer, 0, 0 );
}

LRESULT WINAPI DebugOutput_WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch ( msg )
  {
    case WM_PAINT:
      {
        PAINTSTRUCT ps;
        auto hDC = BeginPaint( hWnd, &ps );
        DebugOutput_Draw( hDC );
        EndPaint( hWnd, &ps );
      }
      return 0;

    case WM_TIMER:
      InvalidateRect( hWnd, 0, FALSE );
      return 0;

    case WM_CLOSE:
      Global_DebugOutput_WindowCloseMessage = true;
      break;

    case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
  }

  return DefWindowProc( hWnd, msg, wParam, lParam );
}

void DebugOutput_CreateWindow( HINSTANCE hInstance )
{
  const char classname[] = "aerofly_external_dll_sample";

  //
  // init gdi+
  //
  Gdiplus::GdiplusStartupInput startupinput;
  auto status = GdiplusStartup( &Global_DebugOutput_gdiplusToken, &startupinput, NULL );

  //
  // fill in window class structure and register the class
  //
  WNDCLASS wc;
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = DebugOutput_WndProc;                // Window Procedure
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hInstance;                          // Owner of this class
  wc.hIcon         = LoadIcon( NULL, IDI_INFORMATION );
  wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
  wc.hbrBackground = (HBRUSH) ( COLOR_BACKGROUND + 1 );      // Default color
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = classname;
  RegisterClass( &wc );

  Global_DebugOutput_WindowCloseMessage = false;

  Global_DebugOutput_Window = CreateWindow( classname, "Aerofly External DLL Sample",
                                            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT,
                                            0, SAMPLE_WINDOW_WIDTH, SAMPLE_WINDOW_HEIGHT,
                                            NULL,       // no parent window
                                            NULL,       // Use the window class menu.
                                            hInstance,  // This instance owns this window
                                            NULL );     // We don't use any extra data


  auto s_width  = GetSystemMetrics( SM_CXSCREEN );
  auto s_height = GetSystemMetrics( SM_CYSCREEN );

  SetWindowPos( Global_DebugOutput_Window, HWND_TOP, s_width - SAMPLE_WINDOW_WIDTH, 0, SAMPLE_WINDOW_WIDTH, SAMPLE_WINDOW_HEIGHT, SWP_SHOWWINDOW );
  // set up timers
  SetTimer( Global_DebugOutput_Window, 0, 500, 0 );

  MSG msg;
  while ( !Global_DebugOutput_WindowCloseMessage && GetMessage( &msg, Global_DebugOutput_Window, 0, 0 ) )
  {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }

  DestroyWindow( Global_DebugOutput_Window );
  Global_DebugOutput_Window = NULL;

  //
  // shutdown gdi+
  //
  Gdiplus::GdiplusShutdown( Global_DebugOutput_gdiplusToken );
}

void DebugOutput_WindowUpdate( const double delta_time, const std::vector<tm_external_message> &message_list_receive )
{
  // this is just for the debug output window
  std::lock_guard<std::mutex> lock_guard{ MessageListMutex };
  MessageListCopy = message_list_receive;
  MessageDeltaTime = delta_time;
}

void DebugOutput_WindowOpen()
{
  Global_DebugOutput_Thread = std::thread( DebugOutput_CreateWindow, global_hDLLinstance );
}

void DebugOutput_WindowClose()
{
  if ( Global_DebugOutput_Window != NULL )
  {
    PostMessage( Global_DebugOutput_Window, WM_QUIT, 0, 0 );
  }
  Global_DebugOutput_Thread.join();
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// the main entry point for the DLL
//
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain( HANDLE hdll, DWORD reason, LPVOID reserved )
{
  switch ( reason )
  {
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_ATTACH:
      global_hDLLinstance = (HINSTANCE) hdll;
      break;
    case DLL_PROCESS_DETACH:
      break;
  }

  return TRUE;
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//
// interface functions to Aerofly FS 2
//
//////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" 
{
  __declspec( dllexport ) int Aerofly_FS_2_External_DLL_GetInterfaceVersion()
  {
    return TM_DLL_INTERFACE_VERSION;
  }

  __declspec( dllexport ) bool Aerofly_FS_2_External_DLL_Init( const HINSTANCE Aerofly_FS_2_hInstance )
  {
    DebugOutput_WindowOpen();
    return true;
  }
  
  __declspec( dllexport ) void Aerofly_FS_2_External_DLL_Shutdown()
  {
    DebugOutput_WindowClose();
  }
  
  __declspec( dllexport ) void Aerofly_FS_2_External_DLL_Update( const tm_double         delta_time,
                                                                 const tm_uint8 * const  message_list_received_byte_stream,
                                                                 const tm_uint32         message_list_received_byte_stream_size,
                                                                 const tm_uint32         message_list_received_num_messages,
                                                                 tm_uint8               *message_list_sent_byte_stream,
                                                                 tm_uint32              &message_list_sent_byte_stream_size,
                                                                 tm_uint32              &message_list_sent_num_messages,
                                                                 const tm_uint32         message_list_sent_byte_stream_size_max )
  {
    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // build a list of messages that the simulation is sending
    //
    MessageListReceive.clear();

    tm_uint32 message_list_received_byte_stream_pos = 0;
    for ( tm_uint32 i = 0; i < message_list_received_num_messages; ++i )
    {
      auto edm = tm_external_message::GetFromByteStream( message_list_received_byte_stream, message_list_received_byte_stream_pos );
      MessageListReceive.emplace_back( edm );
    }


    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // this is just for the debug output window
    //
    if ( !MessageListReceive.empty() )
    {
      DebugOutput_WindowUpdate( delta_time, MessageListReceive );
    }


    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // parse the message list
    //
    tm_vector3d aircraft_position;
    for ( const auto &message : MessageListReceive )
    {
      const auto sh = message.GetStringHash().GetHash();

      if ( sh == MessageAircraftPosition.GetID() )
      {
        aircraft_position = message.GetVector3d();
      }
      else if ( sh == MessageAircraftAltitude.GetID() )  // alternatively you can write -> if( message.GetStringHash() == "Aircraft.Altitude" )
      {
        tm_double altitude = message.GetDouble();
      }
      // and so on....
    }


    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // build a list of message that will be sent back to the simulation
    //
    message_list_sent_byte_stream_size = 0;
    message_list_sent_num_messages     = 0;


    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // example 1: change time of day continously
    //
    //MessageSimulationTimeChange.SetValue( 1.0 );
    //MessageSimulationTimeChange.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );


    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // example 2: implement a free view. in this example the camera just rotates around the aircraft
    //
//    static double time = 0;
//    time += delta_time;
//
//    const auto world_east  = tmcoordinates_GetEastAt( aircraft_position );
//    const auto world_north = tmcoordinates_GetNorthAt( aircraft_position );
//    const auto world_up    = tmcoordinates_GetUpAt( aircraft_position );
//    
//    const auto view_up             = world_up;
//    const auto view_position       = aircraft_position + 15.0 *( sin( time )*world_east + cos( time )*world_north ) + ( 1.0 + cos( time )*cos( time ) )*world_up;
//    const auto view_look_direction = Normalized( aircraft_position - view_position );
//
//    MessageViewFreePosition.SetValue( view_position );
//    MessageViewFreePosition.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//
//    MessageViewFreeLookDirection.SetValue( view_look_direction );
//    MessageViewFreeLookDirection.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//
//    MessageViewFreeUp.SetValue( view_up );
//    MessageViewFreeUp.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//
//    MessageViewFreeFieldOfView.SetValue( tm_helper_deg_to_rad( 100.0 ) );
//    MessageViewFreeFieldOfView.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );


    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // example 3: set aileron input
    //
//    MessageControlsRollInput.SetValue( sin( time ) );
//    MessageControlsRollInput.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );


    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // example 4: reposition the aircraft every 20 seconds to a position near birrfeld
    //
//    static double extime = 0.0; 
//    extime += delta_time;
//
//    if( extime > 20.0 )
//    {
//      extime = 0.0;
//
//      MessageSimulationSettingPosition.SetValue( tm_vector3d( 4264642.1 + 500.0, 616894.2 + 500.0, 4693293.4 + 500.0 ) );
//      
//      tm_matrix3d orientation = tm_matrix3d( -0.213045, -0.691774,  0.689970,
//                                              0.957133, -0.289600,  0.005181,
//                                              0.196231,  0.661497,  0.723818  );
//
//      tm_vector3d velocity = 100.0 * ( orientation * tm_vector3d( 1.0, 0.0, 0.0 ) );
//
//      tm_quaterniond q = tm_MatrixToQuaternion<tm_matrix3d,double>( orientation );
//
//      MessageSimulationSettingOrientation.SetValue( tm_vector4d( q.r, q.x, q.y, q.z ) );
//      MessageSimulationSettingVelocity.SetValue( velocity );
//
//      MessageSimulationSettingPosition.AddToByteStream(    message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//      MessageSimulationSettingOrientation.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//      MessageSimulationSettingVelocity.AddToByteStream(    message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//      MessageSimulationSettingSet.AddToByteStream(         message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//    }


    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // example 5: pause the simulation after 10 seconds, set playback position and start playback 
    //
//    static double time = 0;
//
//    if( time < 10 && time + delta_time >= 10 )
//    {
//      MessageSimulationPause.SetValue( 1.0 );
//      MessageSimulationPause.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//    }
//    
//    if( time < 11 && time + delta_time >= 11 )
//    {
//      MessageSimulationPlaybackSetPosition.SetValue( 0.5 );
//      MessageSimulationPlaybackSetPosition.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//    }
//    
//    if( time < 12 && time + delta_time >= 12 )
//    {
//      MessageSimulationPlaybackStart.SetValue( 1.0 );
//      MessageSimulationPlaybackStart.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//    }
//
//    time += delta_time;


    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // example 6: visual only - set the aircraft's position and orientation from your dynamics model
    //                          example uses longitude/latitude/altitude pitch/bank/heading
//    static double time = 0;
//    time += delta_time;
//
//    //
//    // start at Speck-Fehraltdorf, use time to change position and orientation
//    //
//    //const double longitude = tm_helper_deg_to_rad(  8.76070 );
//    //const double latitude  = tm_helper_deg_to_rad( 47.37500 );
//    //const double altitude  = 0.3048 * 1765.0;
//
//    static tm_vector3d position = tm_vector3d( 0, 0, 0.5 );//tmcoordinates_PositionFromLatLonAlt( latitude, longitude, altitude );
//
//    const auto world_east  = tm_vector3d( 1, 0, 0 );//tmcoordinates_GetEastAt( position );
//    const auto world_north = tm_vector3d( 0, 1, 0 );//tmcoordinates_GetNorthAt( position );
//    const auto world_up    = tm_vector3d( 0, 0, 1 );//tmcoordinates_GetUpAt( position );
//
//    //
//    // aircraft direction and up vector from heading, pitch and bank
//    // 
//    // rotation sequence: heading - pitch - bank
//    //
//    const  double pitch   = tm_helper_deg_to_rad( 0.0 );
//    const  double bank    = tm_helper_deg_to_rad( 2.91 * sin( 0.25 * time ) * sin( 0.25 * time ) );
//    static double heading = 0.0; //tm_helper_deg_to_rad( 90.0 + 56.8 );  // heading counterclockwise starting at east in radians  
//
//    heading += delta_time * 9.81 * bank / 2.0;
//
//    //
//    // orientation vectors from pitch/bank/heading
//    //
//    const auto aircraft_forward         =  cos( pitch   ) * ( cos( heading ) * world_east + sin( heading ) * world_north ) + sin( pitch ) * world_up;
//    const auto aircraft_up_with_pitch   = -sin( pitch   ) * ( cos( heading ) * world_east + sin( heading ) * world_north ) + cos( pitch ) * world_up;
//    const auto aircraft_left_horizontal =                    -sin( heading ) * world_east + cos( heading ) * world_north;
//
//    const auto aircraft_up              =  cos( bank ) * aircraft_up_with_pitch + sin( bank ) * aircraft_left_horizontal;
//    const auto aircraft_left            = -sin( bank ) * aircraft_up_with_pitch + cos( bank ) * aircraft_left_horizontal;
//
//    position = position + 2.0 * delta_time * aircraft_forward;
//
//    //
//    // assemble orientation matrix and quaternion 
//    //
//    tm_matrix3d orientation = tm_matrix3d( aircraft_forward.x, aircraft_left.x, aircraft_up.x,
//                                           aircraft_forward.y, aircraft_left.y, aircraft_up.y,
//                                           aircraft_forward.z, aircraft_left.z, aircraft_up.z  );
//
//    tm_quaterniond q = tm_MatrixToQuaternion<tm_matrix3d,double>( orientation );
//
//    //
//    // send messages
//    //
//    MessageSimulationExternalPosition.SetValue( position );
//    MessageSimulationExternalPosition.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
//    MessageSimulationExternalOrientation.SetValue( tm_vector4d( q.r, q.x, q.y, q.z ) );
//    MessageSimulationExternalOrientation.AddToByteStream( message_list_sent_byte_stream, message_list_sent_byte_stream_size, message_list_sent_num_messages );
  }
}


