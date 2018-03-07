This directory contains both HMS and SHMS matrix elements. If you want to change which reconstruction matrix to use, you will need to tell hallc_replay where it is by changing the matrix listed in PARAM/*/GEN/*cana.param where * refers to using HMS or SHMS. It is listed in the recon_coeff_filename. 

~~~~
HMS
~~~~
-hms_recon_coeff.dat: Original, ideal, matrix elements prior to optimization.
-hms_recon_coeff_opt2018.dat: Optimized matrix elements for delta +/- 10%.


~~~~~
SHMS
~~~~~
-shms-2011-26cm-monte_q2_m015_rec.dat: matrix elements to reflect KPP run findings with Q2 at 1.5% low. 
-SHMS_fr3_rec__order_3.dat: ideal matrix elements using 3rd order fit. 
-SHMS_fr3_rec__order_4.dat: ideal matrix elements using 4th order fit.
-SHMS_fr3_rec__order_5.dat: ideal matrix elements using 5th order fit.
-SHMS_fr3_rec__order_6.dat: ideal matrix elements using 6th order fit. 
