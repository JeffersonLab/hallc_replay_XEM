#!/usr/bin/env python
import sys
import os
import subprocess
import ROOT
from ROOT import gROOT
########################################################################################################
# This script is used to loop through tweaks to surveyed drift chambers to improve alignment. 
# Here we only tweak x and y positions. 
# You need to:
#      -make an output folder at: CALIBRATION/shms_dc_calib/output_align
#      -modify the run number (rNUM) and number of events (evNUM) as desired
#      -modify the step sizes as desired
#      -modify the start and max values as desired
#
# This should be run from the hallc_replay directory. Check path to hcana. 
# Make directories: raw, ROOTfiles, CALIBRATION/shms_dc_calib/output_align,
# REPORT_OUTPUT/SHMS/PRODUCTION
# raw should contain the .dat input file
#
# Author: Holly Szumila, hszumila@jlab.org
# Date: 27 September 2017
########################################################################################################
#######################################
# Enter run number and number of events
#######################################
rNUM = 488
evNUM = 10

############################################
# loop plane number, x position, y position
############################################
# define the step size:
xposStep = 0.001
yposStep = 0.001

# define the starting values:
xposInit = -0.05
yposInit = -0.05

# define the maximum values:
xposMax = 0.05
yposMax = 0.05

# define the start values:
xposSV = [-0.036,0.008]
yposSV = [0.019,0.013] 

# fill the start values by plane
xGlobal = [xposSV[0], xposSV[0], xposSV[0], xposSV[0], xposSV[0], xposSV[0], xposSV[1], xposSV[1], xposSV[1], xposSV[1], xposSV[1], xposSV[1]]
yGlobal = [yposSV[0], yposSV[0], yposSV[0], yposSV[0], yposSV[0], yposSV[0], yposSV[1], yposSV[1], yposSV[1], yposSV[1], yposSV[1], yposSV[1]]

# loop plane 
for plane in range (0,12):

    # define the starting values for iteration
    xpos = xposInit
    ypos = yposInit

    #  loop tweaks in x
    while xpos < xposMax:
        xShift = [0,0,0,0,0,0,0,0,0,0,0,0]
        xShift[plane] = xpos
        # generate the modified parameters
        text_file = open("PARAM/dcalign.param","w")
        text_file.write("; x and y position of each chamber, tweaking x.\n")
        for iplane in range (0,12):
            if (iplane == 0):
                text_file.write("pdc_xpos = %f\n" %(xGlobal[iplane]+xShift[iplane]) )
            else :
                text_file.write("           %f\n" %(xGlobal[iplane]+xShift[iplane]) )
        text_file.close()
            
        # open root in hcana, run the file, and exit
        subprocess.call("./hcana -b -q 'CALIBRATION/shms_dc_calib/scripts/replay_aligndc_shms.C (%d,%d)'" % (rNUM, evNUM),shell=True)
            
        # move the output file and rename
        os.rename("ROOTfiles/shms_replay_production_%d_%d.root" % (rNUM,evNUM), "CALIBRATION/shms_dc_calib/output_align/shms_replay_%d_xpos_%.3f.root" %(plane, xpos))
        #os.rename("REPORT_OUTPUT/SHMS/PRODUCTION/replay_shms_production_%d_%d.report" % (rNUM,evNUM), "CALIBRATION/shms_dc_calib/output_align/replay_shms_production_%d_xpos_%.3f.report" %(plane, xpos))
        #os.rename("PARAM/dcalign.param","CALIBRATION/shms_dc_calib/output_align/param_%d_xpos_%.3f.param" %(plane,xpos))
        xpos += xposStep
            
    # loop tweaks in y
    while ypos < yposMax:
        yShift = [0,0,0,0,0,0,0,0,0,0,0,0]
        yShift[plane] = ypos
        # generate the modified parameters
        text_file = open("PARAM/dcalign.param","w")
        text_file.write("; x and y position of each chamber, tweaking y.\n")
        for iplane in range (0,12):
            if (iplane == 0):
                text_file.write("pdc_ypos = %f\n" %(yGlobal[iplane]+yShift[iplane]) )
            else :
                text_file.write("           %f\n" %(yGlobal[iplane]+yShift[iplane]) )
        text_file.close()
    
        # open root in hcana, run the file, and exit
        subprocess.call("./hcana -b -q 'CALIBRATION/shms_dc_calib/scripts/replay_aligndc_shms.C (%d,%d)'" % (rNUM, evNUM),shell=True)
            
        # move the output file and rename
        os.rename("ROOTfiles/shms_replay_production_%d_%d.root" % (rNUM,evNUM), "CALIBRATION/shms_dc_calib/output_align/shms_replay_%d_ypos_%.3f.root" %(plane, ypos))
        ypos += yposStep
