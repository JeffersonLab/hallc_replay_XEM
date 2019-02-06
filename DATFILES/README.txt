This directory contains both HMS and SHMS matrix elements. If you want to change which reconstruction matrix to use, you will need to tell hallc_replay where it is by changing the matrix listed in PARAM/*/GEN/*cana.param where * refers to using HMS or SHMS. It is listed in the recon_coeff_filename. 

~~~~
HMS
~~~~
-hms_recon_coeff.dat: Original, ideal, matrix elements prior to optimization.
-hms_recon_coeff_opt2018.dat: Optimized matrix elements for delta +/- 10%.

You must use the corresponding hmsflags file in the PARAM/*/GEN/ to keep offsets correct!!

~~~~~
SHMS
~~~~~
-shms-2019-newopt-Jan19.dat : includes new optimization that excludes the 4 GeV runs used previously and uses a 6th order fit.

- shms-2017-optimized_delta_newfit3.dat : The xptar,ytar,yptar matrix elements are the same as shms-2017-optimized-2plow.dat . The delta matrix was optimized using carbon elastic data taken during fall 2017. The carbon elastic data determined that the dipole momentum was being set 1.7% too low for the Dec 2017 - May 2018 running period. The magnet field setting program will changed before running in Aug 2018 so that  all magnets were shifted by 1.7%.
 


-shms-2017-optimized-2plow.dat: matrix elements optimized using runs 1808-2239 during Jan 2018. To use matrix, set pphi_offset in shmsflags.param to -8.681269905E-4. Shift the yfp by -0.429871 cm. This differs fromm shms-2017-optimized.dat by using an initial COSY matrix model with delta offset by 2% low. 
-shms-2017-optimized.dat: matrix elements optimized using runs 1808-2239 during commissioning run period. To use this matrix, one must change the pphi_offset in shmsflags.param to 4.83E-4 rad. This is a 5th order fit. Still using non-optimized delta. Also, shift yfp by -0.613 cm.
-shms-2011-26cm-monte_q2_m015_rec.dat: matrix elements to reflect KPP run findings with Q2 at 1.5% low. 
-SHMS_fr3_rec__order_3.dat: ideal matrix elements using 3rd order fit. 
-SHMS_fr3_rec__order_4.dat: ideal matrix elements using 4th order fit.
-SHMS_fr3_rec__order_5.dat: ideal matrix elements using 5th order fit.
-SHMS_fr3_rec__order_6.dat: ideal matrix elements using 6th order fit. 
