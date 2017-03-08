onlineGUI
============================================
This is the GUI that is used for the monitoring of online replay.

Directory structure
----------------------
* Config : directory with the  onlineGUI configuration files
  *Files in Config:
    *HMS/HODO/raw_hhodoana.cfg (Plots standard  hisotgrams in DEF-files/HMS/HODO/hhodoana.def )
    *SHMS/HODO/raw_phodoana.cfg (Plots standard  hisotgrams in DEF-files/SHMS/HODO/phodoana.def )
    *HMS/HODO/raw_hhodo_2d.cfg (2d plots of Good HMS Hodoscope data, efficiency plots)
    *HMS/HODO/good_hhodo_2d.cfg (2d plots of Good HMS Hodoscope data, efficiency plots)
    *SHMS/HODO/good_phodo_2d.cfg (2d of Good SHMS Hodoscope data, efficiency plots)
* Macros : directory with root macros that can be called by the onlineGUI code
  * Files in Macro: 
    * overlay.C (script to overlay 2 histograms)
    * rawdraw_2d_hhodo.C ( Used by raw_hhodo_2d.cfg)
    * rawdraw_2d_phodo.C ( Used by raw_phodo_2d.cfg)
    * gooddraw_2d_hhodo.C ( Used by good_hhodo_2d.cfg)
    * gooddraw_2d_phodo.C ( Used by good_phodo_2d.cfg)
    * hodo_efficiency.C ( Used by good_phodo_2d.cfg,good_hhodo_2d.cfg)
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


