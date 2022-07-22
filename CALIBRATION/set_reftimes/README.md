# Hall C Reference Time Setting Code

1. The scripts in reference_time_setup.C is used to visualize the current reference time cuts being applied.
2. The user can take this information and set new reference time cuts in the corresponding DAQ setup (hms, shms, coin).
3. Once the new reference times are set, the code can be re-ran to validate the changes were properly applied.

## Scripts within reference_time_setup.C
run_hms_reference_time_setup
run_shms_reference_time_setup
run_coin_reference_time_setup

The following are the inputs:
(TString infile, int RunNumber, TString outfile="move_me.root")
1. infile is the full path to the root file replayed by SCRIPTS/<SHMS,HMS,COIN>/TIMING/replay_no_reference_times_<shms,hms,coin>.C
2. RunNumber is the same run number used in the replay script.  This is used to initialize the gHcParms with the previous reference times.
3. outfile is the default output file containing canvases of the desired plots.

## Running the Calibration
For detailed information on reference times and timing windows, please see Carlos Yero's First Steps:
https://hallcweb.jlab.org/doc-private/ShowDocument?docid=1032
1. First replay the desired run with the devoted replay script: replay_no_reference_times_<shms,hms,coin>.  The purpose of this script is to replay the data FAST and produce specific histograms that will be used in the following steps.  You cannot use the production replay, this replay has specific DEF-files.
2. Load the reference time setup scripts with hcana.  Similar to a replay, this script REQUIRES hcana, you cannot just use root.  
`.L CALIBRATION/set_reftimes/reference_time_setup.C`
3. Run the appropriate script based on the DAQ configuration.  If you have a simple SHMS DAQ, use the run_shms_reference_time_setup with the appropriate inputs found above.  **Note: The CALIBRATION script loads gHcParms with the hard coded script.  If you changed the gHcParms->Load() statements in your production replay script, the cuts displayed could be  different from your production running. Ensure that the gHcParms in the CALIBRATION script are the same as those for production runnning.  Ideally, they would use the same setup, but this would require changes to how SCRIPTS are made in hallc_replay.**
4. Close hcana and open the output root file.  The default is move_me.root. **Move this file to a location and name to keep for your records.** Check to see that the orange cut lines are all **to the left** of the main reference time peaks required for your experiment**.  Several histograms may appear in different colors on top of each other.  These correspond to different multiplicity cuts. If you know what you are looking for, you can select a tighter reference time cut, but for most Hall C physics, we try to let the detectors select out bad events, no reference times.  
5. Change the corresponding reference time cuts if required in: `PARAM/<SHMS,HMS,COIN>/GEN/<p,h>_reftime_cut.param`.  **Note: the files changed must be the ones actually being loaded by hcana.  For instance, the COIN DAQ may load PARAM/HMS/GEN/h_reftime_cut_coindaq.param.  Check this in your standard.dbase or general.param files.**
6. Repeat these steps to validate your changes took place.

## Disclaimer
This code was developed for the EMC/X>1 experiments in Hall C.  Different DAQ configurations may require some changes to this script, specifically the gHcParms.  You can ensure this will work for your experiments purposes by running it over a different experiment's file before your experiment gets on the floor.  Setting the reference times and timing windows has been a major source of pain during experiment startup because the 50k replays and diagnostic histograms may have no events in them!  Familiarize yourself with this code prior to running the experiment.  