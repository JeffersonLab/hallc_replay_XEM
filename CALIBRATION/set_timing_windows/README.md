# Hall C Timing Window Setting Code

1. The scripts in timing_window_setup.C are used to visualize the current timing window cuts being applied and reset the if necessary.
2. If setting the timing windows with this script, it is highly recommended to visualize the cuts and ensure they are being applied properly.
3. Once the new timing windows are set, the code can be re-ran to validate the changes were properly applied.

## Scripts within timing_window_setup.C
calc_timing_windows - Used by the run_<spec>_timing_windows codes
run_hms_timing_windows - Used for HMS DAQ
run_shms_timing_windows - Used for SHMS DAQ
run_coin_timing_windows - Used for COIN DAQ

The following are the inputs:
(TString file_name, TString out_file, int RunNumber, bool newWindows=false, double width=40., TString spec="shms")
1. infile is the full path to the root file replayed by SCRIPTS/<SHMS,HMS,COIN>/TIMING/replay_no_timing_windows_<shms,hms,coin>.C
2. outfile is the default output file containing canvases of the desired plots. **Must be set**
3. RunNumber is the same run number used in the replay script.  This is used to initialize the gHcParms with the previous timing windows.
4. Bool newWindows is used to set new windows if desired.
5. Uses the peak position found using TSpectrum in the timing windows and outputs a cut that is +/- width/2 from the peak.
6. spec is used to run the run_coin_timing_windows script.  You can optionally run just one arm of the COIN DAQ by changing spec to coin.

## Running the Calibration
For detailed information on reference times and timing windows, please see Carlos Yero's First Steps:
https://hallcweb.jlab.org/doc-private/ShowDocument?docid=1032
1. First replay the desired run with the devoted replay script: replay_no_timing_windows_<shms,hms,coin>.  The purpose of this script is to replay the data FAST and produce specific histograms that will be used in the scripts mentioned above.  You cannot use the production replay, the above scripts require specific DEF-files.  
2. Load the timing window setup scripts with hcana.  Similar to a replay, this script REQUIRES hcana, you cannot just use root.  
`.L CALIBRATION/set_timing_windows/timing_window_setup.C`
3. Run the appropriate script based on the DAQ configuration.  If you have a simple SHMS DAQ, use the run_shms_timing_window_setup with the appropriate inputs found above (you can click tab after the paraenthesis to show the usage after loading the script).  **Note: The CALIBRATION script loads gHcParms with the hard coded script.  If you changed the gHcParms->Load() statements in your production replay script, the cuts displayed could be different from your production running. Ensure that the gHcParms in the CALIBRATION script are the same as those for production runnning.  Ideally, they would use the same setup, but this would require changes to how SCRIPTS are made in hallc_replay.**
4. Close hcana and open the output root file.  There is no default file name, so you must give one. **Move this file to a location and name to keep for your records.** Once open, look at the cut lines (in red or green).  Cuts that do not encapsulate the found peak are colored RED while those that encapsulate the peak are GREEN.  The found peak may not always be the correct peak, especially if there are few statistics.  If all or most of the peaks are GREEN, then continue to adjust the timing window cuts by hand.  If they are mostly red, you can use the automatic functionality of the script to output the CUT files to be updated in hallc_replay.
5. Change the corresponding timing window cuts if required in: `PARAM/<SHMS,HMS,COIN>/<DETEC>/<p,h><detec>_cuts.param`.  **Note: Check with the top of the production replay to see which CUTS files need altered.**
6. Repeat these steps to validate your changes took place.

'''
To output the **new** timing window cuts to the screen in hallc_replay format, simply change newWindows to *true* and change the width to the desired value.  Copy the output timing windows by hand to a new cut or replace the existing cuts.  
'''

## Disclaimer
This code was developed for the EMC/X>1 experiments in Hall C.  Different DAQ configurations may require some changes to this script, specifically the gHcParms.  You can ensure this will work for your experiments purposes by running it over a different experiment's file before your experiment gets on the floor.  Setting the reference times and timing windows has been a major source of pain during experiment startup because the 50k replays and diagnostic histograms may have no events in them!  Familiarize yourself with this code prior to running the experiment.  
