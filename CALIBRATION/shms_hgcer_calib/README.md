# Calibrating the Cherenkovs
* Link your ROOT file
* Run the script with options (listed after script is executed)
```
root -l ../../ROOTfiles/shms_replay_488_-1.root
T->Process("calibration.C+", "options");
T->Process("efficiencies.C+", "options");
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
