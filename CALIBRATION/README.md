# Hall C Calibration Codes

1. This is the directory containing subdirectories for Hall C calibration codes.
2. A few of the directories are obsolete. 
3. Each subdirectory has a README or HOWTO which explains how to run the calibration code.

## Obsolete directories

1. hms_dc_calib : old HMS DC Tzero and time-to-distance maps calibration code
2. shms_dc_calib : old SHMS DC Tzero and time-to-distance maps calibration code
3. hodo_calib: old hodoscope calibration code used during the 6GeV running

## Directories

1. bcm_current_map : Contains scripts to create parameter files of the beam current as a function of event number to be used in the code for cutting data according to a beam current cut.

2. bpm_calib : Contains scripts for fitting HARP vs BPM scans and creating a parameter file.

3. dc_calib/scripts : Contains scripts for calibrating tzero parameters and time-to-dsitance maps for eithe the HMS or SHMS drift chambers. 

4. hms_cal_calib : Contains scripts for calibrating the HMS calorimeter energy/ADC conversion for each block. 

5. shms_cal_calib : Contains scripts for calibrating the SHMS calorimeter energy/ADC conversion for each block. 

6. hms_hodo_calib : Contains scripts for calibrating the time offsets for the HMS hodoscope. Creates the time walk correction parameters, time difference between negative-postive PMTs in a paddle and the relative time offsets of the paddles.

7. shms_hodo_calib : Contains scripts for calibrating the time offsets for the SHMS hodoscope. Creates the time walk correction parameters, time difference between negative-postive PMTs in a paddle and the relative time offsets of the paddles.

8. shms_aero_calib : Contains a script to calibrate the NPE/ADC conversion parameters from SPE peaks for the SHMS aerogel.

9. shms_hgcer_calib : Contains scripts to calibrate hte NPE/ADC conversion parameters for SHMS Gas Cerenkovs.




