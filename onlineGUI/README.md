onlineGUI
============================================
This is the GUI that is used for the monitoring of online replay.

Directory structure
----------------------
* CONFIG : directory with the  onlineGUI configuration files
  This folder is organized by spectrometer, then by system. A GEN top level 
  folder contains .cfg files that contain whole runs or multiple systems.
* UTIL : directory with root macros that can be called by the onlineGUI code
  This folder is organized by spectrometer, then by system. A GEN top level
  folder contains codes that are non-specific toward the detector system.
* onlineGUI_v1.2.1: directory with the online.c and its dependencies.

Compiling code
----------------------
* In onlineGUI_v1.2.1 directory type: make
* executable "online" is created in the onlineGUI directory

Documentation and Running the code
----------------
* The pdf file  guiHOWTO2007.pdf documents how to use the code
* One method to run the code is:
  * online -f CONFIG/kpp.cfg -r 488
  * "488" is the run number
  * This looks for a file following the format in the .cfg file:protorootfile ../ROOTfiles/shms_replay_XXXXX.root


