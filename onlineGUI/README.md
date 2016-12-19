onlineGUI
============================================
This is the GUI that is used in Hall A for the monitoring of online replay.

Directory structure
----------------------
* Config : directory with the  onlineGUI configuration files
* Macros : directory with root macros that can be called by the onlineGUI code
  * Files in Macro: 
    *  overlay.C (script to overlay 2 histograms)
    * rawdraw.C (script to plot the individual TDc and ADC for HMS hodoscope using the tree. Good example for other scripts)
* onlineGUI_v1.2.1: directory with the online.c and other files.

Compiling code
----------------------
* In onlineGUI_v1.2.1 directory type: make
* executable "online" is created in the onlineGUI directory

Documentation and Running the code
----------------
* The pdf file  guiHOWTO2007.pdf documents how to use the code
* One method to run the code is:
  * online -f Config/hmsdc.cfg -r 60
  * "60" is the run number
  * This assumes that hmsdc.cfg as a line such as:protorootfile ../ROOTfiles/hms1190_XXXXX.root


