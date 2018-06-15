HMS/SHMS Drift Chambers Calibration
============================================
This directory contains the code for calibrating the pair of HMS/SHMS drift chambers.



Directory structure
----------------------
* hallc_replay/CALIBRATION/dc_calib/scripts/main_calib.C   : steering C++ code that executes the methods in DC_Calib.C
* hallc_replay/CALIBRATION/dc_calib/scripts/DC_Calib.C  : Calibration Code where all the class  methods are defined
* hallc_replay/CALIBRATION/dc_calib/scripts/DC_Calib.h  : Header file containing the variable definitions used in the methods



Running code
---------------
NOTE: p: SHMS,  h: HMS,  <spec>: HMS, or SHMS

* First set the parameter 'p(h)_using_tzero_per_wire = 0' in the 
  parameter file located in: hallc_replay/PARAM/<spec>/DC/p(h)dc_cuts.param

* Replay the data to produce the uncalibrated root file to be used as input in the calibration
  NOTE: Make sure to include the necessary leafs if you want to make any pid cuts.
  The leafs are:

      SHMS:
      P.ngcer.npeSum, P.cal.etot, T.shms.pEL_CLEAN_tdcTime (the P.cal.etot is currently commented out until further discussion)

      HMS:
      H.cer.npeSum, H.cal.etot, T.hms.hEL_CLEAN_tdcTime (the H.cal.etot is currently commented out until further discussion)
      

  * From the hallc_replay execute: ./hcana SCRIPTS/<spec>/PRODUCTION/<replay_script.C>

* From the directory where this README file is:
  -Open and modify 'DC_Calib obj()' line. This line has the following format:

  -------------------------------------------------------------------------------------------------
  	DC_calib obj("<spec_flag>", "/path/to/ROOTfile/", run_NUM, event_NUM, "<pid_flag>");

	**The <spec_flag> and <pid_flag> have the possible arguments:

	    <spec_flag>: HMS, SHMS

	    <pid_flag>: pid_elec, pid_hadron, dc_1hit, pid_kFLASE

	    When the <pid_flag> is selected, the calibration script applies the following cuts:

	    	 1) pid_elec:  P.ngcer.npeSum > 1.0 && T.shms.pEL_CLEAN_tdcTime > 0.0   (for HMS:  H.cer.npeSum > 1.0 && T.hms.hEL_CLEAN_tdcTime > 0.0 )
		 2) pid_hadron: P.ngcer.npeSum < 1.0  (for HMS:  H.cer.npeSum < 1.0 )
		 3) dc_1hit: Ndata.P(H).dc.plane.time == 1 && Ndata.P(H).dc.plane.wirenum == 1,  where plane-> 1u1, 1u2, ...
		    NOTE: The Ndata cut, requires a single hit in each chamber per plane per event
		 4) pid_kFALSE: No cuts are applied.
  -------------------------------------------------------------------------------------------------
	    		

* Once the arguments are specified, execute: root -l main_calib.C

When the calibration is completed, a directory will be created under the name: <spec_flag>_DC_Log_runNUM/

     In this directory, the calibration output files are stored automatically, once the calibration is completed:

     	     1)  <spec_flag>_DC_driftimes.root
	     2) t_zero_values_plane.dat
	     3) p(h)dc_calib_runNUM.param
	     4) p(h)dc_tzero_per_wire_runNUM.param

	     	Each file contains the following:
		     1) uncalibrated/calibrated drift times, fitted drift times, tzero_v_wire graphs
		     2) data files containing four columns: wire, tzero, tzero_error,  entries
		     3) parameter file containing time-to-distance look-up values
		     4) parameter file containing per-wire tzero corrections


* The parameter files must be copied to the following location:

      -> /hallc_replay/PARAM/<spec>/DC/hdc_calib.param
      -> /hallc_replay/PARAM/<spec>/DC/hdc_tzero_per_wire.param

* Set the parameter 'p(h)_using_tzero_per_wire = 1' in the 
  parameter file located in: hallc_replay/PARAM/<spec>/DC/p(h)dc_cuts.param


* From the hallc_replay execute: ./hcana SCRIPTS/SHMS/PRODUCTION/<replay_script.C> once again.

NOTE: An indication that the calibration worked is by looking a the drift distances, which should appear
      relatively flat. There may be a high number of counts at the extremes of the drift distance, for which
      the additional cuts may need to be applied.

* The recommended cuts when looking at drift distances are:

      1) P(H).dc.plane.time > 0   (drift time > 0)
      2) PID cuts that were applied based on the <pid_flag> selected
	     





