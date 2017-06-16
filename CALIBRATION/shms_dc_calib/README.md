SHMS Drift Chambers Calibration
============================================
This directory contains the code for calibrating the pair of SHMS drift chambers.



Directory structure
----------------------
* hallc_replay/CALIBRATION/shms_dc_calib/run_Cal.C   : steering C++ code that executes all codes in the 'scripts' directory
* hallc_replay/CALIBRATION/shms_dc_calib/scripts  : all scripts necessary to do calibration lie in this directory
* hallc_replay/CALIBRATION/shms_dc_calib/root_files : all root files produced by the calibration are in this directory
* hallc_replay/CALIBRATION/shms_dc_calib/data_files : all data files produced by the calibration are in this directory




Running code
---------------
* First set the parameter 'p_using_tzero_per_wire = 0' in the 
  parameter file located at: hallc_replay/PARAM/SHMS/DC/pdc.param

* Replay the data to produce the uncalibrated root file to be used as input in the calibration
  * From the hallc_replay execute: ./hcana SCRIPTS/SHMS/replay_shms.C

* From THIS! directory execute: root -l run_Cal.C

* From the calibration results, two parameter files will be produced in:
  * hallc_replay/PARAM/SHMS/DC/pdc_tzero_per_wire_run%d_NEW.param. %d=run_number
  * hallc_replay/PARAM/SHMS/DC/pdriftmap_run%d_NEW.param

* Rename the new parameter files as follows:
  * copy: pdc_tzero_per_wire_run%d_NEW.param to pdc_tzero_per_wire.param
  * copy: pdriftmap_new.param to pdriftmap.param

* Before replaying the data again, set the parameter 'p_using_tzero_per_wire = 1' to 
  allow the source code (hcana) to read the parameter values during the replay.

* Replay the data with the updated parameters to produce the new calibrated root files
  with the corrected drift times and drift distances.

* Compare the calibrated and uncalibrated root files to verify the calibration was done properly.



Brief decription of code
------------------------
* Overview: The code determines the tzero offsets on a wire-by-wire basis. These offsets are 
            corrections by which each wire drift time spectrum must be shifted to align it with a 
            drift time of "0 ns". This time corresponds to when the electrons from the ionized gas inside
            the chamber are in contact with the sense wire, hence a drift time of "0 ns". 

* Brief Description of the scripts in the '/scripts' directory
  *** to run the scripts independenlty, open the input_RUN.txt files and write the filen_ame, run_number and number of events in this order: root_file.root   run_number   events 

  	** get_pdc_time_histo.C : 
	  	-- outputs root_file: 'shms_dc_time_%d.root', (%d=run_number)
		-- contains re-binned per-plane drift time histograms
	
	** wire_drift_times.C (and wire_drift_times.h) - See instructions in wire_drift_times.C on how to execute independently 
	   	-- outputs root_file: 'shms_DC_plane_%d_wire_histos.root' 
		-- contains a 2-D histo of "drift time vs. Wire Number" and drift time spectra for all wires in the plane

	** get_wire_tzero.C 
	        -- outputs root_file: 'shms_DC_plane_%d_fitted_histos.root'
		-- contains line-fitted wire drift time histos. The extrapolation of the fit to the x-axis is defined as "tzero" 
		   Also contains two "tzero vs. wirenumber" plots: one for all wires, and the other is after setting tzero=0 for
                   wires that did not have enough statistics for a good quality fit, a weighted average was calculated for the latter.  
		   
		-- outputs data_file: 'shms_dc_planetzero_run%d.dat'         -> contains list of "tzero" values for all wires  	
		   	   	      'shms_dc_planetzero_run%d_updated.dat' -> contains list of "tzero" values for all wires that
				      					     	had a good fit. The remaining tzeros are set to 0.
				      'tzero_weighted_avg_run%d.dat          -> contains weighted tzero values per plane 

	** get_tzero_per_wire_param.C
		-- outputs data_file: 'tzero_values_per_wire.dat'
		-- contains list of tzero values for all wires in all planes 

		-- outputs param_file: /hallc_replay/PARAM/SHMS/DC/pdc_tzero_per_wire_run%d_NEW.param, where %d=run_number
                -- contains tzero values for all wires in all planes, but the file is formatted so that the values may be read by the source code (hcana)
	  
	** get_pdc_time_histo_tzero_corrected.C  	  	
	   	-- outputs root_file: 'shms_tzero_corr_histos.root'
		-- contains list of "t0-corrected" wire drift times, and their respective plane drift times.

	** get_LookUp_Values.C
	        -- outputs param_file: /hallc_replay/PARAM/SHMS/DC/pdriftmap_new.param
 		-- contains scaling factors calculated from the corrected plane drift times on a bin-by-in basis. These values get read by the source code
		   which will be used to scale the drift distance histograms. 
