Cameron Cotton 02/20/23

This directory contains code used to calibrate the SHMS NGCER detector.

To run the code, simply be in this directory (shms_ngcer_calib) and type root -l pngcer_calib.C. This will open an interactive root session and begin running the script.

You will be prompted to enter run number(s), space separated, for the run(s) you would like to use for the calibration. As this code utilizes RDataFrames and runs very quickly, I would suggest using on the order of millions of events for the calibration. Ideally, the runs chosen will have a large fraction of good electrons, and events at that setting should cover all 4 of the Cerenkov's PMTs well. The path from this directory to the corresponding replay files is expected to follow the pattern "../../ROOTfiles/SHMS/CALIBRATION/shms_replay_cer_%i_-1.root" where %i is to be replaced with the run number. Change this if your file naming or directory organization is different.

Once run number(s) are entered, the code will run.

One common runtime error is that the code cannot find one of the branches it is looking for. If the code cannot find a branch that it is looking for, then that branch is not likely present in the replay file used, and needs to be added. Required Branches: P.cal.etottracknorm P.gtr.dp P.ngcer.goodAdcMult P.ngcer.xAtCer P.ngcer.yAtCer P.ngcer.goodAdcPulseInt

If it does not find one of the branches, the error message will generally say that it cannot find "P".

Once the code finishes running, it will produce 3 windows with diagnostic plots for the user to confirm that the data looks ok. Verify that the fits on the plots of the pulse integral are fitting the peak well. If the correct range is not being fitted, you can manually change it in the code.

The calibration constants to be put in the param file will also be printed to the terminal. The calibration constant represents the effective gain of the system, or the conversion factor between the pulse integral and the number of photoelectrons (charge in pC per photoelectron).

As of 01/11/23, PMT 1 is noisy, so different fit ranges need to be set on those plots. Also, the multiplicity cut should be changed from P.ngcer.goodAdcMult[0]==1 to >=1 as suggested by Mark Jones to deal with this issue.
