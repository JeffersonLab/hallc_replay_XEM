#!/usr/bin/python

import sys

if sys.version_info[0] < 3:
    import Tkinter as tk
else:	
    import tkinter as tk
import os

root = tk.Tk()

hms_def_filename = "DEF-files/HMS/PRODUCTION/hstackana_production.def"
hms_def_filename_bck = "DEF-files/HMS/PRODUCTION/hstackana_production.def.bck"
shms_def_filename = "DEF-files/SHMS/PRODUCTION/pstackana_production.def"
shms_def_filename_bck = "DEF-files/SHMS/PRODUCTION/pstackana_production.def.bck"

def hms_var_states():
    hms_def_file_text = []
#    if (hcerana_histos.get() == 1) or (hdcana_histos.get() == 1) or (hhodoana_histos.get() == 1) or (hcalana_histos.get() == 1) or (htrigana_histos.get() == 1):
#        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/BLOCK/hblock_vars.def"\n''')
#        hms_def_file_text.append("\n")

    if hcerana_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/TEST_STANDS/CER/hcerana_histos.def"\n''')
    if hdcana_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/TEST_STANDS/DC/hdcana_histos.def"\n''')
    if hhodoana_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/TEST_STANDS/HODO/hhodoana_histos.def"\n''')
    if hcalana_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/TEST_STANDS/CAL/hcalana_histos.def"\n''')
    if htrigana_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/TEST_STANDS/TRIG/htrigana_histos.def"\n''')
    if (hcerana_histos.get() == 1) or (hdcana_histos.get() == 1) or (hhodoana_histos.get() == 1) or (hcalana_histos.get() == 1) or (htrigana_histos.get() == 1):
        hms_def_file_text.append("\n")

    if htrig_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/TRIG/htrig_histos.def"\n''')
    if hcal_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/CAL/hcal_histos.def"\n''')
    if hcer_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/CER/hcer_histos.def"\n''')
    if hdc_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/DC/hdc_histos.def"\n''')
        #hms_def_file_text.append('''#include "DEF-files/HMS/TEST_STANDS/DC/hdc_resid.def"\n''')
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/DC/hdc_eff_histos.def"\n''')
    if hhodo_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/HODO/hhodo_histos.def"\n''')
    
    hms_def_file_text.append('''\n''')
   
    if hkin_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/KIN/hkin_histos.def"\n''')
    if hgtr_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/GTR/hgtr_histos.def"\n''')
    if hraster_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/RASTER/hraster_histos.def"\n''')
    if hreact_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/REACT/hreact_histos.def"\n''')
    if hpid_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/PID/hpid_histos.def"\n''')
    if hextcor_histos.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/EXTCOR/hextcor_histos.def"\n''')
    
    hms_def_file_text.append('''\n''')

    if hpid_vars.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/PID/hpid_vars.def"\n''')
    if hepics_vars.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/EPICS/hepics_vars.def"\n''')
    if hf2kin_vars.get() == 1:
        hms_def_file_text.append('''#include "DEF-files/HMS/PRODUCTION/KIN/hkin_vars.def"\n''')

    hms_def_file_text.append('''\n''')
    
    if hthms_vars.get() == 1:
        hms_def_file_text.append('''block T.hms.*\n''')
    if hcer_vars.get() == 1:
        hms_def_file_text.append('''block H.cer.*\n''')
    if hdc_vars.get() == 1:
        hms_def_file_text.append('''block H.dc.*\n''')
    if htrig_vars.get() == 1:
        hms_def_file_text.append('''block H.tr.*\n''')
    if hhodo_vars.get() == 1:
        hms_def_file_text.append('''block H.hod.*\n''')
    if hcal_vars.get() == 1:
        hms_def_file_text.append('''block H.cal.*\n''')
    if hgtr_vars.get() == 1:
        hms_def_file_text.append('''block H.gtr.*\n''')
    if hkin_vars.get() == 1:
        hms_def_file_text.append('''block H.kin.*\n''')
    if hraster_vars.get() == 1:
        hms_def_file_text.append('''block H.rb.*\n''')
    if hreact_vars.get() == 1:
        hms_def_file_text.append('''block H.react.*\n''')
   
    print ("Backing up current HMS Output DEF-file ...")
    command = "mv " + hms_def_filename + " " + hms_def_filename_bck
    os.system(command) 
    print ("Writing new HMS Output DEF-file ...")
    hms_def_file = open(hms_def_filename,"w")
    hms_def_file.writelines(hms_def_file_text)
    hms_def_file.close()

def shms_var_states():
    shms_def_file_text = []
#    if (paeroana_histos.get() == 1) or (pcalana_histos.get() == 1) or (pdcana_histos.get() == 1) or (phgcerana_histos.get() == 1) or (phodoana_histos.get() == 1) or ( pngcerana_histos.get() == 1) or (ptrigana_histos.get() == 1):
#        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/BLOCK/pblock_vars.def"\n''')
#        shms_def_file_text.append("\n")

    if paeroana_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/TEST_STANDS/AERO/paeroana_histos.def"\n''')
    if pcalana_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/TEST_STANDS/CAL/pcalana_histos.def"\n''')
    if pdcana_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/TEST_STANDS/DC/pdcana_histos.def"\n''')
    if phgcerana_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/TEST_STANDS/HGCER/pgcerana_histos.def"\n''')
    if phodoana_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/TEST_STANDS/HODO/phodoana_histos.def"\n''')
    if pngcerana_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/TEST_STANDS/NGCER/pngcerana_histos.def"\n''')
    if ptrigana_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/TEST_STANDS/TRIG/ptrigana_histos.def"\n''')
    if (paeroana_histos.get() == 1) or (pcalana_histos.get() == 1) or (pdcana_histos.get() == 1) or (phgcerana_histos.get() == 1) or (phodoana_histos.get() == 1) or ( pngcerana_histos.get() == 1) or (ptrigana_histos.get() == 1):
        shms_def_file_text.append("\n")

    if ptrig_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/TRIG/ptrig_histos.def"\n''')
    if paero_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/AERO/paero_histos.def"\n''')
    if pcal_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/CAL/pcal_histos.def"\n''')
    if pdc_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/DC/pdc_histos.def"\n''')
    if phgcer_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/HGCER/phgcer_histos.def"\n''')
    if pngcer_histos.get() == 1:
        shms_def_file_text.append('''# #include "DEF-files/SHMS/PRODUCTION/NGCER/pngcer_histos.def"\n''')
    if phodo_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/HODO/phodo_histos.def"\n''')
    
    shms_def_file_text.append('''\n''')
    
    if pkin_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/KIN/pkin_histos.def"\n''')
    if pgtr_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/GTR/pgtr_histos.def"\n''')
    if praster_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/RASTER/praster_histos.def"\n''')
    if preact_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/REACT/preact_histos.def"\n''')
    if ppid_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/PID/ppid_histos.def"\n''')
    if pextcor_histos.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/EXTCOR/pextcor_histos.def"\n''')
    
    shms_def_file_text.append('''\n''')
    
    if pf2dc_vars.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/DC/pdc_vars.def"\n''')
    if pf2gtr_vars.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/GTR/pgtr_vars.def"\n''')
    if ppid_vars.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/PID/ppid_vars.def"\n''')
    if pepics_vars.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/EPICS/pepics_vars.def"\n''')
    if pf2kin_vars.get() == 1:
        shms_def_file_text.append('''#include "DEF-files/SHMS/PRODUCTION/KIN/pkin_vars.def"\n''')
   
    shms_def_file_text.append('''\n''')
    
    if ptshms_vars.get() == 1:
        shms_def_file_text.append('''block T.shms.*\n''')
    if pngcer_vars.get() == 1:
        shms_def_file_text.append('''block P.ngcer.*\n''')
    if pdc_vars.get() == 1:
        shms_def_file_text.append('''block P.dc.*\n''')
    if phodo_vars.get() == 1:
        shms_def_file_text.append('''block P.hod.*\n''')
    if phgcer_vars.get() == 1:
        shms_def_file_text.append('''block P.hgcer.*\n''')
    if paero_vars.get() == 1:
        shms_def_file_text.append('''block P.aero.*\n''')
    if pcal_vars.get() == 1:
        shms_def_file_text.append('''block P.cal.*\n''')
    if ptrig_vars.get() == 1:
        shms_def_file_text.append('''block P.tr.*\n''')
    if pgtr_vars.get() == 1:
        shms_def_file_text.append('''block P.gtr.*\n''')
    if pkin_vars.get() == 1:
        shms_def_file_text.append('''block P.kin.*\n''')
    if praster_vars.get() == 1:
        shms_def_file_text.append('''block P.rb.*\n''')
    if preact_vars.get() == 1:
        shms_def_file_text.append('''block P.react.*\n''')
    
    print ("Backing up current SHMS Output DEF-file ...")
    command = "mv " + shms_def_filename + " " + shms_def_filename_bck
    os.system(command) 
    print ("Writing new SHMS Output DEF-file ...")
    shms_def_file = open(shms_def_filename,"w")
    shms_def_file.writelines(shms_def_file_text)
    shms_def_file.close()

def set_hms_production_defaults():
    hcal_histos.set(1)
    hcer_histos.set(1)
    hdc_histos.set(1)
    hhodo_histos.set(1)
    htrig_histos.set(1)
    hkin_histos.set(1)
    hgtr_histos.set(1)
    hraster_histos.set(1)
    hreact_histos.set(1)
    hpid_histos.set(1)
    hextcor_histos.set(1)
    hcalana_histos.set(0)
    hcerana_histos.set(0)
    hdcana_histos.set(0)
    hhodoana_histos.set(0)
    htrigana_histos.set(0)
    hepics_vars.set(1)
    hf2kin_vars.set(1)
    hpid_vars.set(1)
    hthms_vars.set(0)
    hcal_vars.set(0)
    hcer_vars.set(0)
    hdc_vars.set(0)
    hhodo_vars.set(0)
    hgtr_vars.set(0)
    htrig_vars.set(0)
    hkin_vars.set(0)
    hraster_vars.set(0)
    hreact_vars.set(0)
    
def set_shms_production_defaults():
    paero_histos.set(1)
    pcal_histos.set(1)
    pdc_histos.set(1)
    pgtr_histos.set(1)
    phgcer_histos.set(1)
    phodo_histos.set(1)
    pkin_histos.set(1)
    pngcer_histos.set(1)
    praster_histos.set(1)
    ptrig_histos.set(1)
    preact_histos.set(1)
    ppid_histos.set(1)
    pextcor_histos.set(1)
    paeroana_histos.set(0)
    pcalana_histos.set(0)
    pdcana_histos.set(0)
    phgcerana_histos.set(0)
    phodoana_histos.set(0)
    pngcerana_histos.set(0)
    ptrigana_histos.set(0)
    pepics_vars.set(1)
    pf2dc_vars.set(1)
    pf2kin_vars.set(1)
    pf2gtr_vars.set(1)
    ppid_vars.set(1)
    ptshms_vars.set(0)
    paero_vars.set(0)
    pcal_vars.set(0)
    pdc_vars.set(0)
    phgcer_vars.set(0)
    phodo_vars.set(0)
    pngcer_vars.set(0)
    ptrig_vars.set(0)
    pkin_vars.set(0)
    pgtr_vars.set(0)
    praster_vars.set(0)
    preact_vars.set(0)
    
def set_hms_teststand_on():
    hcalana_histos.set(1)
    hcerana_histos.set(1)
    hdcana_histos.set(1)
    hhodoana_histos.set(1)
    htrigana_histos.set(1)
    hthms_vars.set(1)
    hcal_vars.set(1)
    hcer_vars.set(1)
    hdc_vars.set(1)
    hhodo_vars.set(1)
    hgtr_vars.set(1)
    htrig_vars.set(1)
    hkin_vars.set(1)
    hraster_vars.set(1)
    hreact_vars.set(1)

def set_shms_teststand_on():
    paeroana_histos.set(1)
    pcalana_histos.set(1)
    pdcana_histos.set(1)
    phgcerana_histos.set(1)
    phodoana_histos.set(1)
    pngcerana_histos.set(1)
    ptrigana_histos.set(1)
    ptshms_vars.set(1)
    paero_vars.set(1)
    pcal_vars.set(1)
    pdc_vars.set(1)
    phgcer_vars.set(1)
    phodo_vars.set(1)
    pngcer_vars.set(1)
    ptrig_vars.set(1)
    pkin_vars.set(1)
    praster_vars.set(1)
    preact_vars.set(1)

hcal_histos = tk.IntVar()
hcer_histos = tk.IntVar()
hdc_histos = tk.IntVar()
hhodo_histos = tk.IntVar()
hkin_histos = tk.IntVar()
hgtr_histos = tk.IntVar()
hraster_histos = tk.IntVar()
htrig_histos = tk.IntVar()
hreact_histos = tk.IntVar()
hpid_histos = tk.IntVar()
hextcor_histos = tk.IntVar()
hepics_vars = tk.IntVar()
hf2kin_vars = tk.IntVar()
hpid_vars = tk.IntVar()

paero_histos = tk.IntVar()
pcal_histos = tk.IntVar()
pdc_histos = tk.IntVar()
pgtr_histos = tk.IntVar()
phgcer_histos = tk.IntVar()
phodo_histos = tk.IntVar()
pkin_histos = tk.IntVar()
pngcer_histos = tk.IntVar()
praster_histos = tk.IntVar()
ptrig_histos = tk.IntVar()
preact_histos = tk.IntVar()
ppid_histos = tk.IntVar()
pextcor_histos = tk.IntVar()
pepics_vars = tk.IntVar()
pf2dc_vars = tk.IntVar()
pf2kin_vars = tk.IntVar()
pf2gtr_vars = tk.IntVar()
ppid_vars = tk.IntVar()

hcalana_histos = tk.IntVar()
hcerana_histos = tk.IntVar()
hdcana_histos = tk.IntVar()
hhodoana_histos = tk.IntVar()
htrigana_histos = tk.IntVar()

hthms_vars = tk.IntVar()
hcer_vars = tk.IntVar()
hdc_vars = tk.IntVar()
htrig_vars = tk.IntVar()
hhodo_vars = tk.IntVar()
hcal_vars = tk.IntVar()
hkin_vars = tk.IntVar()
hgtr_vars = tk.IntVar()
hraster_vars = tk.IntVar()
hreact_vars = tk.IntVar()

paeroana_histos = tk.IntVar()
pcalana_histos = tk.IntVar()
pdcana_histos = tk.IntVar()
phgcerana_histos = tk.IntVar()
phodoana_histos = tk.IntVar()
pngcerana_histos = tk.IntVar()
ptrigana_histos = tk.IntVar()

ptshms_vars = tk.IntVar()
pngcer_vars = tk.IntVar()
pdc_vars = tk.IntVar()
phodo_vars = tk.IntVar()
phgcer_vars = tk.IntVar()
paero_vars = tk.IntVar()
pcal_vars = tk.IntVar()
ptrig_vars = tk.IntVar()
pkin_vars = tk.IntVar()
pgtr_vars = tk.IntVar()
praster_vars = tk.IntVar()
preact_vars = tk.IntVar()

set_hms_production_defaults()
set_shms_production_defaults()

irow=0
icol=0

tk.Label(root, 
        text="""HMS Histogram Blocks:""",
        justify = tk.LEFT,
        padx = 20,pady=20).grid(row=irow,column=icol,columnspan=2,sticky=tk.W)
irow=irow+1
tk.Label(root, 
        text="""Production:""",
        justify = tk.LEFT,
        padx = 20).grid(row=irow,column=icol,sticky=tk.W)
tk.Label(root, 
        text="""Test Stands:""",
        justify = tk.LEFT,
        padx = 20).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Calorimeter",
              padx = 20, 
              variable=hcal_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Calorimeter",
              padx = 20, 
              variable=hcalana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Cerenkov",
              padx = 20, 
              variable=hcer_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Cerenkov",
              padx = 20, 
              variable=hcerana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Drift Chambers",
              padx = 20, 
              variable=hdc_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Drift Chambers",
              padx = 20, 
              variable=hdcana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Hodoscope",
              padx = 20, 
              variable=hhodo_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Hodoscope",
              padx = 20, 
              variable=hhodoana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Trigger",
              padx = 20, 
              variable=htrig_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Trigger",
              padx = 20, 
              variable=htrigana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Kinematics",
              padx = 20, 
              variable=hkin_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Tracking",
              padx = 20, 
              variable=hgtr_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Raster",
              padx = 20, 
              variable=hraster_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Reaction",
              padx = 20, 
              variable=hreact_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="PID",
              padx = 20, 
              variable=hpid_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="EXTCOR",
              padx = 20, 
              variable=hextcor_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+4
tk.Label(root, 
        text="""HMS Variables:""",
        justify = tk.LEFT,
        padx = 20,pady=20).grid(row=irow,column=icol,columnspan=2,sticky=tk.W)
irow=irow+1
tk.Label(root, 
        text="""Production:""",
        justify = tk.LEFT,
        padx = 20).grid(row=irow,column=icol,sticky=tk.W)
tk.Label(root, 
        text="""Test Stands:""",
        justify = tk.LEFT,
        padx = 20).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="EPICS",
              padx = 20, 
              variable=hepics_vars).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="HMS Tree",
              padx = 20, 
              variable=hthms_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="PID",
              padx = 20, 
              variable=hpid_vars).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Calorimeter",
              padx = 20, 
              variable=hcal_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Kinematics",
              padx = 20, 
              variable=hf2kin_vars).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Cerenkov",
              padx = 20, 
              variable=hcer_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Drift Chambers",
              padx = 20, 
              variable=hdc_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Hodoscope",
              padx = 20, 
              variable=hhodo_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Trigger",
              padx = 20, 
              variable=htrig_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Kinematics",
              padx = 20, 
              variable=hkin_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Tracking",
              padx = 20, 
              variable=hgtr_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Raster",
              padx = 20, 
              variable=hraster_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Reaction",
              padx = 20, 
              variable=hreact_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+5
tk.Button(root, text='Quit', command=root.quit,justify=tk.LEFT,padx=20).grid(row=irow,column=icol,sticky=tk.W)
tk.Button(root, text='HMS Production Default', command=set_hms_production_defaults,justify=tk.LEFT,padx=20).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Button(root, text='Write HMS DEF-file', command=hms_var_states,justify=tk.LEFT,padx=20).grid(row=irow,column=icol,sticky=tk.W)
tk.Button(root, text='All HMS Test Stands', command=set_hms_teststand_on,justify=tk.LEFT,padx=20).grid(row=irow,column=icol+1,sticky=tk.W)

irow=0
icol=2

tk.Label(root, 
        text="""SHMS Histogram Blocks:""",
        justify = tk.LEFT,
        padx = 20).grid(row=irow,column=icol,columnspan=2,sticky=tk.W)
irow=irow+1
tk.Label(root, 
        text="""Production:""",
        justify = tk.LEFT,
        padx = 20).grid(row=irow,column=icol,sticky=tk.W)
tk.Label(root, 
        text="""Test Stands:""",
        justify = tk.LEFT,
        padx = 20).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Aerogel",
              padx = 20, 
              variable=paero_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Aerogel",
              padx = 20, 
              variable=paeroana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Calorimeter",
              padx = 20, 
              variable=pcal_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Calorimeter",
              padx = 20, 
              variable=pcalana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Drift Chambers",
              padx = 20, 
              variable=pdc_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Drift Chambers",
              padx = 20, 
              variable=pdcana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Heavy Gas Cerenkov",
              padx = 20, 
              variable=phgcer_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Heavy Gas Cerenkov",
              padx = 20, 
              variable=phgcerana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Hodoscope",
              padx = 20, 
              variable=phodo_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Hodoscope",
              padx = 20, 
              variable=phodoana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Noble Gas Cerenkov",
              padx = 20, 
              variable=pngcer_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Noble Gas Cerenkov",
              padx = 20, 
              variable=pngcerana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Trigger",
              padx = 20, 
              variable=ptrig_histos).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Raw Trigger",
              padx = 20, 
              variable=ptrigana_histos).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Kinematics",
              padx = 20, 
              variable=pkin_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Tracking",
              padx = 20, 
              variable=pgtr_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Raster",
              padx = 20, 
              variable=praster_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Reaction",
              padx = 20, 
              variable=preact_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="PID",
              padx = 20, 
              variable=ppid_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="EXTCOR",
              padx = 20, 
              variable=pextcor_histos).grid(row=irow,column=icol,sticky=tk.W)
irow=irow+2
tk.Label(root, 
        text="""SHMS Variables:""",
        justify = tk.LEFT,
        padx = 20).grid(row=irow,column=icol,columnspan=2,sticky=tk.W)
irow=irow+1
tk.Label(root, 
        text="""Production:""",
        justify = tk.LEFT,
        padx = 20).grid(row=irow,column=icol,sticky=tk.W)
tk.Label(root, 
        text="""Test Stands:""",
        justify = tk.LEFT,
        padx = 20).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="EPICS",
              padx = 20, 
              variable=pepics_vars).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="SHMS Tree",
              padx = 20, 
              variable=ptshms_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Tracking",
              padx = 20, 
              variable=pf2gtr_vars).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Aerogel",
              padx = 20, 
              variable=paero_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="PID",
              padx = 20, 
              variable=ppid_vars).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Calorimeter",
              padx = 20, 
              variable=pcal_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Drift Chambers",
              padx = 20, 
              variable=pf2dc_vars).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Drift Chambers",
              padx = 20, 
              variable=pdc_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Kinematics",
              padx = 20, 
              variable=pf2kin_vars).grid(row=irow,column=icol,sticky=tk.W)
tk.Checkbutton(root, 
              text="Heavy Gas Cerenkov",
              padx = 20, 
              variable=phgcer_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Hodoscope",
              padx = 20, 
              variable=phodo_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Noble Gas Cerenkov",
              padx = 20, 
              variable=pngcer_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Trigger",
              padx = 20, 
              variable=ptrig_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Kinematics",
              padx = 20, 
              variable=pkin_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Tracking",
              padx = 20, 
              variable=pgtr_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Raster",
              padx = 20, 
              variable=praster_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Checkbutton(root, 
              text="Reaction",
              padx = 20, 
              variable=preact_vars).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+3
tk.Button(root, text='SHMS Production Default', command=set_shms_production_defaults,justify=tk.LEFT,padx=20).grid(row=irow,column=icol+1,sticky=tk.W)
irow=irow+1
tk.Button(root, text='Write SHMS DEF-file', command=shms_var_states,justify=tk.LEFT,padx=20).grid(row=irow,column=icol,sticky=tk.W)
tk.Button(root, text='All SHMS Test Stands', command=set_shms_teststand_on,justify=tk.LEFT,padx=20).grid(row=irow,column=icol+1,sticky=tk.W)

root.mainloop()
