Vijay Kumar, Univerity of Regina - 24/07/20
vijay36361@gmail.com

####################################
### Using the Calibration Script ###
####################################

The script has some default paths set for the KaonLT group on the farm, as well as machines at the University of Regina.

Please change or add your path to the block around Line 146 of the run_cal.C macro in this folder.

This path should point to the directory where your replayed files (to be calibrated) are stored.

To run the calibration, use the run_cal.C macro with root.

root run_cal.C

Executing the script in this way will yield several prompts to the user to provide the arguments, it is recommended you provide the arguments when running the script as detailed below.

It is highly recommend you execute the script without displaying any graphics (run root with -b).

This script requires between 4 and 13 arguments, these are -

RunPrefix - The prefix to the replayed rootfile you wish to run, e.g if you files are Coin_Replay_#RUNNUMBER_#NUMEVENTS.root, the prefix would be Coin_Replay, omit the trailing _ before the runnumber
NumEvents - The number of events per run (must all be equal), e.g. if you ran all events, enter -1 here
nRuns - Number of runs you want to chain together and attempt to calibrate over, the script can handle 1-10 runs - ONLY enter an integer between 1 to 10 here
RunNumber1 - The run number for the first run you want to add to the chain

If you entered > 1 for nRuns, you will also need to specify RunNumber2, RunNumber3... RunNumberN

RunNumber2 - The run number for the second run you want to add to the chain
RunNumber3 - The run number for the third run you want to add to the chain
... and so on

As an example of running the script, suppose you want to analyse Coin_Replay_5555_-1.root, Coin_Replay_5556_-1.root and Coin_Replay_5557_-1.root

From the directory this README is located in, execute

root -l -b -q 'run_cal.C("Coin_Replay", -1, 3, 5555, 5556, 5557)'

When running the script, it will complain if a rootfile cannot be found. 

The path it tried to find the rootfile in will be printed to screen, check this looks correct and adjust the pathing if it doesn't.

You may also need to hit enter (return) after running the command above as it sometimes hangs on -

Processing run_cal.C("Coin_Replay", -1, 3, 5555, 5556, 5557)...

###################
### Output Info ###
###################

By default, the output from the code is place in a folder called Calibration_plots.

Files are named by runnumber(s), there are a set of plots saved as a pdf as well as two param files with values.

By default, the weighted average of the first and second guess parameters are printed as the calibration parameters. The file phgcer_calib_#RUNNUMBER.param file is the file that should be fed back to hcana.

##############################################
### How many runs should I chain together? ###
##############################################

If any calibraiton paramaters are returned as NaN this is a sign that the calibration has failed. This is likely due to a lack of statistics.

If no quadrants for a PMT return a "good" fit (see the instructions pdf for more info) then the fitting will fail for that PMT.

If this is the case, try executing the calibration again with an extra run.

Typically, the calibration needs at least two runs chained together to yield decent results.