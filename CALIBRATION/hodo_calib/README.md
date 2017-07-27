Hodoscope Calibrations
============================================
This is directory contains the code for calibrating the hodoscope timing
in SHMS and HMS

Directory structure
----------------------
* tofcal.f  : Fortran code which execute the calibration.
* Maketof   : Script to compile tofcal.f and make executable tofcal
* tofcal.inp : Input file for tofcal.f
* shmstofcal.inp : Version of tofcal.inp for SHMS
* hmstofcal.inp : Version of tofcal.inp for HMS



Compiling code
--------------
* Execute ./Maketof

Running code
-----------
* First replay data by adding  either line to replay script
** HMS:   gHcParms->Load("PARAM/HMS/HODO/CALIB/htofcalib.param");
** SHMS:   gHcParms->Load("PARAM/SHMS/HODO/CALIB/ptofcalib.param");
* The replay will put the new file : hfort.37 or pfort.37 in the CALIBRATION/hodo_calib directory
* Copy either hmstofcal.inp or shmstofcal.inp to tofcal.inp
* Execute ./tofcal
* Print out info on the initial chi2 of  data and final chi2 of the fit.
* Creates file tofcal.param
* Edit the parameters in tofcal.param into either PARAM/HMS/HODO/hhodo.param or  PARAM/SHMS/HODO/phodo.param

Brief decription of code
------------------------
* The code fits tcorr = time - offset - path * velocity - adccor / sqrt(ADC)
* Uses paddle 10 positive PMT on plane 1 as the reference PMT ( offset fixed at 0)
* To get data file name. Reads-in the input file: tofcal.inp 
* Opens data file
    * "0" line separates events
    * Should have pair of neg and pos PMTs for each paddle
    * Format is : iside  ipl ipad tdcval_uncorr pathl zcor tdcval_corr adcval
        * iside = 1,2 for pos and neg PMT
        * ipl = plane number
        * tdcval_uncorr = raw time in ns. The program uses this in optimization.
        * pathl = distance for focal plane to the paddle using the track
        * zcor = time (ns) for the particle to go from focal plane to paddle. Correction subtracted from tdcval_uncrr
        * tdcval_corr = the corrected time in ns. Not used by program. Just gives the initial chi2.
        * adcval = ADC value used in fit
* Loops through data determines the number of hits in each PMT. Will only include PMT if the number of hits > 100.
* Again loops through the data filling array with time difference between all pairs PMTs, pathl and adcval.
* Use the CERNLIB routine deqn to invert the matrix and determine the fit parameters.
