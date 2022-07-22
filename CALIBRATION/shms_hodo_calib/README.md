# Hall C SHMS Hodo Calibration Codes

A set of ROOT scripts to determine the time offset parameters for the SHMS hodoscope.  The next script determines the effective propagation speed in the paddle, the time difference between the positive and negative PMTs and then the relative time difference of all paddles compared to paddle 7 in plane S1X.  This calibration requires at least 1-2M events.


This calibration procedure supercedes an older calibration code. The parameters from the outdated code are
used flag ptofusinginvadc is set to 1.  The ptofusinginvadc has been set to zero by default for at least the past 4 years.

The two codes have different parameters and it is possible to switch between the parameters of the two calibration codes using the  parameter flag ptofusinginvadc.  Do not use the old calibration code unless instructed to by an expert.  

## Instructions

1. Replay the data with SCRIPTS/SHMS/CALIBRATION/replay_hodoscope_shms.C  or SCRIPTS/COIN/CALIBRATION/replay_hodoscope_coin.C depending on the DAQ configuration in use.
   Note: This script sets the parameter phodo_debug_adc = 1 automatically.  If you are using a different script, you will need to change phodo_debug_adc = 1 in PARAM/SHMS/GEN/p_fadc_debug.param.    phodo_debug_adc = 1 ---> Use the correct hodo raw leaf variables

     a. Open root with 'root -l' and load the script with .L SCRIPT/SHMS/CALIBRATION/replay_hodoscope_shms.C or .L SCRIPT/COIN/CALIBRATION/replay_hodoscope_coin.C depending on the DAQ.
   	   i. Run the script as replay_hodoscope_shms(Run_Number, N_events, replay_step=1)
           ii. The ROOTfile from these scripts is saved in either ROOTfiles/SHMS/CALIBRATION/ or ROOTfiles/COIN/CALIBRATION depending on the DAQ used.

2. Determine the time walk correction parameters

     a. Start "root -l" and then  .L timeWalkHistos.C
            i. timeWalkHistos("path_to_file/replayed_file.root",Run_Number, "shms") ---> If doing coincidence, then "shms"->"coin"

     b. This creats the file: timeWalkHistos_<Run_Number>.root in the current directory.

     c. Start "root -l" and then .L timeWalkCalib.C
            i. timeWalkCalib(Run_Number)

     d. This creates the parameter file "PARAM/SHMS/HODO/phodo_TWcalib_runnumber.param"
            i.  Follow the naming convention outlined in the xem2wiki and move the renamed file to the proper location as outlined in the xem2wiki.

     e. It is likely the hodoscope calibration will only be done once per spectrometer.  In this case only the #include statement in DBASE/SHMS/default_det_calib_fa22.param file will need changed.
           i. replace the existing line #include "PARAM/SHMS/HODO/phodo_TWcalib_fa22_startup.param" with the new file following the naming convention.  This needs to match exactly what you named it in step d.i.

3.  Replay using the same script as before and the new parameter files.  Make sure the new file phodo_TWcalib_*.param file is used by checking the loaded PARAM files.  You will again need around 1-2M events.
     a. Load the script with .L SCRIPT/SHMS/CALIBRATION/replay_hodoscope_shms.C or .L SCRIPT/COIN/CALIBRATION/replay_hodoscope_coin.C depending on the DAQ.
   	   i. Run the script as replay_hodoscope_shms(Run_Number, N_events, replay_step=2)

4. Determine the the effective propagation speed in the paddle, the time difference between the positive and negative PMTs and then the relative time difference of all paddles compared to paddle 7 in plane S1X. The script puts cuts on P.cal.etracknorm, P.ngcer.npeSum and P.hod.betanotrack to select electrons. These cuts are hard coded as  etrknrm_low_cut = 0.7, npngcer_npeSum_low_cut = 0.7 , betanotrack_low_cut = 0.5 and betanotrack_hi_cut = 1.5. These may need to be modified. The event must have a track. 

     a.  Start "root -l" and then  .L  fitHodoCalib.C
            i. fitHodoCalib("path_to_file/replayed_file.root",Run_Number)     

     b.  This creates the parameter file "PARAM/SHMS/HODO/phodo_Vpcalib_runnumber.param"
            i. Rename this file and move it to the proper location according to the xem2wiki.  

     c. It also creates the root file HodoCalibPlots_runnumber.root
     	    i. Move this root file to the appropriate location on the group disk to make it viewable online.  

Extra:
     a. To analyze cosmic data :  .x  fitHodoCalib.C+("current_dir/to/ROOT_filename.root",Run_Number,kTRUE) 

     b. For cosmic data the speed of light is set to -30 cm/ns and the PID cut is just on P.hod.betanotrack with the default of betanotrack_low_cut = -1.2 and betanotrack_hi_cut = -.7
