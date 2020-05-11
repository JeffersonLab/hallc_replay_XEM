1) First need to prepare histograms of the goodAdcPed for each detector.
2) the 2d histograms of goodADCPed verus the paddle number are contained
   in HMS_PedDefault.def and SHMS_PedDefault.def
   Move files to DEF-files/SHMS/PRODUCTION/SHMS_PedDefault.def
   and DEF-files/HMS/PRODUCTION/HMS_PedDefault.def
3) Include the files in your  tree/histogram def file set in replay script.
   #include "DEF-files/SHMS/PRODUCTION/SHMS_PedDefault.def"
   #include "DEF-files/HMS/PRODUCTION/HMS_PedDefault.def"
4) Replay the data
5) Start root
   a) .L run_ped_default.C
   b) run_shms_ped_default("entirepath/DirName/filename.root")
   c) run_hms_ped_default("entirepath/DirName/filename.root")
6) The SHMS does HGCER, NGCER, AERO, Preshower and Shower
7) HSM does CER and CAL.
8) By hand copy each set of Pedestal defaults into the detector "cuts" file
  For example:
phgcer_PedDefault= 2086, 2153, 2320, 1987

  goes into phgcer_cuts.param or the equivelent

