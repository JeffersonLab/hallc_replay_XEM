# Hall C BPM Calibration Scripts

## Instructions
1. Source the setup.sh or setup.csh as appropriate for your shell.

2. It is expected that the ROOT files are already in the ~/hallc_replay/ROOTfiles directory.

3. It is expected that the HARP/BPM data for the appropriate runs are in a text file called harp_info.txt, although a different filename can be specified as an argument to the ROOT macro.

4.  Within ROOT, execute the bpm calibration macro as:

.x bpm_calibration.C("harp_info.txt")

Notes:

For the Spring 2018 running, the appropriate HARP calibration file is:

harp_info.txt.extra.no1766 
