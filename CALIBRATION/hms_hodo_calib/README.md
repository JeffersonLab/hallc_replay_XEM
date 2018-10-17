# Hall C HMS Hodo Calibration Codes

A set of ROOT scripts to determine the time offset parameters for the HMS hodoscope. To use these parameters a  parameter flag ptofusinginvadc is set to 0. The first script determines the time walk correction factor. The next script determines the effective propagation speeed in the paddle, the time difference between the positive and negative PMTs and then the relative time difference of all paddles compared to paddle 7 in plane S1X.


This supercedes an older calibration code. The parameters from that code are
used if a parameter flag ptofusinginvadc is set to 1. 

The two codes have different parameters and it is possible to switch between the parameters of the two calibration codes using the  parameter flag ptofusinginvadc


## Instructions

1. Replay the data with ptofusinginvadc=0 and need to have T.hms.* and H.hod.* in the tree. 
   Go to: PARAM/HMS/GEN/h_fadc_debug.param, and set hhodo_debug_adc = 1 --->  Use the correct hodo raw leafs variables 

(In case you are calibrating HMS hodo using a coincidence run, then make sure to include T.coin.*)


2. Determine the time walk correction parameters

     a. Start "root -l" and then  .x timeWalkHistos.C+("current_dir/to/ROOT_filename.root", Run_Number, "hms") ---> If doing coincidence, then "hms"->"coin"

     b. This creats the file: timeWalkHistos.root

     c. Start "root -l" and then .x timeWalkCalib.C+

     d. This creates the parameter file "../../PARAM/HMS/HODO/hhodo_TWcalib_runnumber.param"

3.  Replay the data with ptofusinginvadc=0 and the new parameter files (the simplest is to copy phodo_TWcalib_runnumber.param to phodo_TWcalib.param).

4. Determine the effective propagation speed in the paddle, the time difference between the positive and negative PMTs and then the relative time difference of all paddles compared to paddle 7 in plane S1X. The script
puts cuts on H.cal.etracknorm, H.hgcer.npeSum and H.hod.betanotrack to select electrons. These cuts are hard coded as  etrknrm_low_cut = 0.7, npngcer_npeSum_low_cut = 0.7 , betanotrack_low_cut = 0.5 and betanotrack_hi_cut = 1.5. These may need to be modified. The event must have a track. 

*** Make sure to have included H.hgcer.* and H.cal.etracknorm in the ROOT tree

     a.  Start "root -l" and then  .x  fitHodoCalib.C+("current_dir/to/ROOT_filename.root",Run_Number)     

     b.  This creates the parameter file "../../PARAM/HMS/HODO/hhodo_Vpcalib_runnumber.param"

     c.  It also creates the root file HodoCalibPlots_runnumber.root

     d.  To analyze cosmic data :  .x  fitHodoCalib.C+("current_dir/to/ROOT_filename.root",Run_Number,kTRUE) 

     e. For cosmic data the speed of light is set to -30 cm/ns and the PID cut is just on P.hod.betanotrack with the default of betanotrack_low_cut = -1.2 and betanotrack_hi_cut = -.7
