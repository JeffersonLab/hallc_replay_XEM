# Calibrating the Cherenkovs & Determining Efficiency
* To automatically run the calibration & efficiency
```
root -l run_cal.C
```
* To manually run the scripts:
* Link your ROOT file
* Run the script with options (listed after script is executed)
```
root -l ../../ROOTfiles/shms_replay_488_-1.root
T->Process("calibration.C+", "options");
T->Process("efficiencies.C+", "options");
```
* Or, if you want to run using PROOF
```
root -l
TChain ch("T");
ch.Add("/path/to/ROOTfile");
TProof::Open(""); \\For PROOF-lite, adjust accordingly for PROOF
ch.SetProof();
ch.Process("calibration.C+", "options");
```
### Options for the calibration script are (case/spelling important):
* **readall** - read all data from ROOT tree (very slow)
* **showall** - display all calibration details (lots of windows)
* **tracksfired** - alternate calibration method using the tracksfired leaf
* **cut** - use particle ID
* **pions** - use particle ID selecting pions

### Options for the efficiencies script are (case/spelling important):
* **showall** - display all calibration details (lots of windows)
* **Chercut** - use the other Cherenkov detector in particle ID
* **NGC** - calibrate the nobel gas Cherenkov
* **[0-9].[0-9]** - number of photoelectrons to cut on. If a cut on the other Cherenkov is desired, the second number entered will be taken as that Cherenkov's cut parameter.
