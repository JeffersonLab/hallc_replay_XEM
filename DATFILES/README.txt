This directory contains both HMS and SHMS matrix elements. If you want to change which reconstruction matrix to use, you will need to tell hallc_replay where it is by changing the matrix listed in PARAM/*/GEN/*cana.param where * refers to using HMS or SHMS. It is listed in the recon_coeff_filename. 

~~~~
HMS
~~~~
-hms_recon_coeff_opt2018.dat: Optimized matrix elements for delta +/- 10%.

You must use the corresponding hmsflags file in the PARAM/*/GEN/ to keep offsets correct!!

-hms_poltar_fit.dat: Fit to optics data from the 40cm polarized target experiment. 


~~~~~
SHMS
~~~~~
-shms_newfit_global_zbin_allA1n.dat : Fit to optics data taken during A1N experiment for long 40cm target.
-shms_newfit_xptar_aprl2020 : Just use runs 1814 and 1919 to improve xptar fit in the > 10% delta region.
-shms-2019-newopt-Jan19.dat : includes new optimization that excludes the 4 GeV runs used previously and uses a 6th order fit.
shms-2017-26cm-monte_quads_p18_q2_120_rec.dat: Current matrix elements for SHMS in hallc_replay.  Needs double-checked for Sp22 startup.
shms-21deg_2p7_Gev_correction.dat: 2018 EMC running at 2.7GeV
shms-21deg_3p3_Gev_correction.dat: 2018 EMC running at 3.3GeV
shms-21deg_4p0_Gev_correction.dat: 2018 EMC running at 4.0GeV
shms-21deg_5p1_Gev_correction.dat: 2018 EMC running at 5.1GeV
shms-25deg_3p5_Gev_correction.dat: 2018 EMC running at 3.5GeV
shms-25deg_4p4_Gev_correction.dat: 2018 F2 running at 4.4GeV
shms-8deg_9p8_Gev_correction.dat: 2018 SRC running at 9.8GeV
shms_newfit_xptar_april2020.dat: Default values for Sp18, iterated by Aruni
