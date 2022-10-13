from ROOT import TFile, TH1D, TLegend, gStyle, TCanvas
import matplotlib as mpl
import numpy as np

sets = [{'runs':[17270, 17279, 17288, 17297, 17298],
  'targ': "C12",
  'ang': 8,
  'leg':["ALL ON", "S1X 1-7 off, S2X 1-6 off", "S1X 1-8 off, S2X 1-6 off", "S1X 1-7 off, S2X 1-8 off", "S1X 1-8 off, S2X 1-7 off"],
  'Q':[26.92, 25.981, 27.676, 13.9, 28.383],
  'PS':[65, 33, 33, 33, 33]},
{'runs':[17268, 17282, 17292, 17293, 17295],
  'targ': "D2",
  'ang': 8,
  'leg':["ALL ON (3/4 instead of ELREAL)", "S1X 1-7 off, S2X 1-6 off", "S1X 1-7 off, S2X 1-7 off", "S1X 1-8 off, S2X 1-7 off", "S1X 1-7 off, S2X 1-8 off"],
  'Q':[24.308, 28.027, 23.969, 25.081, 29.675],
  'PS':[129, 65, 65, 65, 33]},
]

gStyle.SetOptStat(0)

for s in sets:
  s1=8
  s2=7
  if s["ang"]==8.5:
    s1=6
    s2=8
  
  c = TCanvas()
  c.SetLogy()

  l = TLegend(0.5,0.7,0.9,0.9)
  for i,run in enumerate(s["runs"]):
    h = TH1D("h{}".format(i), "h", 30, 0, 3)
    f = TFile("UTIL_XEM/paddle_plots_out/paddle_cut_plots_{}_S1X{}_S2X{}.root".format(run, s1, s2))
    #f.Project("h{}".format(i),"hx")
    h = f.Get("hx")
    #h.SetLineColor(i)
    l.AddEntry(h, s['leg'][i], 'l')
    if i==1:
      h.Draw()
    else:
      h.Draw('same')
    f.Close()
  l.Draw()

  c.SaveAs("{}_{}deg.png".format(s['targ'], s['ang']))

