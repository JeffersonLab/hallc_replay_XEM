# Notes

## Naming

* `X` stands for spectrometer name:
  * `h` for HMS
  * `p` for SHMS
  * `s` for SOS
* `Y` stands for plane name.
* `N` stands for sequential number

## Constants

* `raddeg = 3.14159265/180`
  * Conversion from degrees to radians.
  * real
  * `DBASE/general.param`


# Tracking

* `Xsel_using_scin = 0`
  * Use scintillator for track selection.
  * {0, 1} bool
  * `THcHallCSpectrometer::fSelUsingScin`

# Drift chambers

## `Xdc.param`

* `Xdc_sigma`
  * Sigma of wire chamber resolution for each plane.
  * [double]
  * `THcDC::fSigma`
* `Xdc_tdc_min_win`
  * TDC window lower limit for each plane.
  * [int]
  * `THcDC::fTdcWinMin`
* `Xdc_tdc_max_win`
  * TDC window higher limit for each plane.
  * [int]
  * `THcDC::fTdcWinMax`
* `Xdc_tdc_time_per_channel`
  * Conversion factor from TDC clicks to ns.
  * double
  * `THcDC::fNSperChan`
* `Xdc_plane_time_zero`
  * Zero time correction for each plane in ns that is added to TDC time.
  * [double]
  * `THcDC::fPlaneTimeZero`
* `Xdc_wire_velocity`
  * Wire velocity.
  * double
  * `THcDC::fWireVelocity`
* `Xdc_central_time`
  * Some sort of central time for each plane. Used for hit time correction.
  * [int]
  * `THcDC::fCentralTime`

## `Xdc.pos`

* `Xdc_num_planes`
  * Total number of drift chamber planes in detector.
  * int
  * `THcDC::fNPlanes`
* `Xdc_num_chambers`
  * Number of drift chambers in detector.
  * int
  * `THcDC::fNChambers`
* `Xdc_zpos`
  * z position in cm for each plane measured from focal plane.
  * [double]
  * `THcDC::fZPos`
* `Xdc_alpha_angle`
  * Angle alpha of wires for each plane in radians. Basically, angle of wires
    in x-y plane.
  * [double]
  * `THcDC::fAlphaAngle`
* `Xdc_beta_angle`
  * Beta angle of wires for each plane in radians.
  * [double]
  * `THcDC::fBetaAngle`
* `Xdc_gamma_angle`
  * Angle gamma of wires for each plane in radians
  * [double]
  * `THcDC::fGammaAngle`
* `Xdc_pitch`
  * Pitch between wires for each plane.
  * [double]
  * `THcDC::fPitch`
* `Xdc_nrwire`
  * Number of wires in each plane.
  * [int]
  * `THcDC::fNWires`
* `Xdc_xcenter`
  * x position of center of each wire chamber.
  * [double]
  * `THcDC::fXCenter`
* `Xdc_ycenter`
  * y position of center of each wire chamber.
  * [double]
  * `THcDC::fYCenter`
* `Xdc_central_wire`
  * Wire number of center of wire chamber for each plane. Basically, this is
    number of imaginary wire, that would cross center of plane. Wire numbers
    start with 1.
  * [double]
  * `THcDC::fCentralWire`
* `Xdc_chamber_planes`
  * Number chamber that each plane belongs to. Chamber number starts with 1.
  * [int]
  * `THcDC::fNChamber`
* `Xdc_wire_counting`
  * Order in which wires are counted in each plane.
    * `Xdc_wire_counting == 0 : center = (wire - Xdc_central_wire) * pitch`
    * `Xdc_wire_counting == 1 : center = (Xdc_nrwire + 1 - wire + Xdc_central_wire) * pitch`
  * [{0, 1}]
  * `THcDC::fWireOrder`
* `Xdc_drifttime_sign`
  * Used for velocity correction in each plane. The velocity correction is the
    distance from the center of the wire divided by the velocity of propagation
    times Xdc_drifttime_sign(plane). +/-1 for disc. card at +/- coord. (i.e.
    top = -x direction, so top readout is +1)
  * [{-1, 1}]
  * `THcDC::fDriftTimeSign`

## `driftmap.param`

Contains lookup table for drift time.

* `Xdriftbins`
  * Number of bins in table.
  * int
  * `THcDriftChamberPlane::ReadDatabase::NumDriftMapBins`
* `Xdrift1stbin`
  * Time of first bin in ns.
  * double
  * `THcDriftChamberPlane::ReadDatabase::DriftMapFirstBin`
* `Xdriftbinsz`
  * Bin size in ns.
  * double
  * `THcDriftChamberPlane::ReadDatabase::DriftMapBinSize`
* `XwcYfract`
  * Drift time lookup table entries for plane Y.
  * [double]
  * `THcDriftChamberPlane::ReadDatabase::DriftMap`
