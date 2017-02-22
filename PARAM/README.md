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
* `cminch = 2.54`
    * Conversion from inches to cm.
    * real
    * `DBASE/general.param`


# Tracking

* `Xsel_using_scin = 0`
    * Use scintillator for track selection.
    * {0, 1} bool
    * `THcHallCSpectrometer::fSelUsingScin`

# Drift chambers

## `Xdc.param`

* `Xdc_num_chambers`
    * Number of drift chambers in detector.
    * int
    * `THcDC::fNChambers`
* `Xdc_num_planes`
    * Total number of drift chamber planes in detector.
    * int
    * `THcDC::fNPlanes`
* `Xdc_plane_names`
    * Names of planes.
    * string
    * `THcDC::Setup::planenamelist`
* `Xdc_chamber_planes`
    * Index of chamber that each plane belongs to. Chamber number starts with 1.
    * [int]
    * `THcDC::fNChamber`
* `Xdc_nrwire`
    * Number of wires in each plane.
    * [int]
    * `THcDC::fNWires`

## `Xdc.pos`

* `Xdc_xcenter`
    * x position in cm of center for each wire chamber.
    * [double]
    * `THcDC::fXCenter`
* `Xdc_ycenter`
    * y position in cm of center for each wire chamber.
    * [double]
    * `THcDC::fYCenter`
* `Xdc_N_zpos`
    * z position in cm of center for each wire chamber.
    * double
    * internal only
* `Xdc_N_roll`
    * Roll in degrees for each wire chamber. Added to `Xdc_alpha_angle`.
    * double
    * internal only
* `Xdc_N_pitch`
    * Pitch in degrees for each wire chamber. Added to `Xdc_beta_angle`.
    * double
    * internal only
* `Xdc_N_yaw`
    * Yaw in degrees for each wire chamber. Added to `Xdc_gamma_angle`.
    * double
    * internal only


* `Xdc_central_wire`
    * Wire number of center of wire chamber for each plane. Basically, this is
      number of imaginary wire, that would cross center of plane. Wire numbers
      start with 1.
    * [double]
    * `THcDC::fCentralWire`
* `Xdc_pitch`
    * Distance between wires for each plane in cm.
    * [double]
    * `THcDC::fPitch`
* `Xdc_wire_counting`
    * Order in which wires are counted in each plane.
        * `Xdc_wire_counting == 0` : `center = (wire - Xdc_central_wire) * pitch`
        * `Xdc_wire_counting == 1` : `center = (Xdc_nrwire + 1 - wire -
           Xdc_central_wire) * pitch`
    * [{0, 1}]
    * `THcDC::fWireOrder`
* `Xdc_zpos`
    * Position along z in cm of each plane measured from focal plane.
    * [double]
    * `THcDC::fZPos`
* `Xdc_alpha_angle`
    * Angle $\alpha$ of wires for each plane in radians. Basically the angle of
      the wires. See also [link][SOS_tracking].
    * [double]
    * `THcDC::fAlphaAngle`
* `Xdc_beta_angle`
    * Angle $\beta$ of wires for each plane in radians. See also
      [link][SOS_tracking].
    * [double]
    * `THcDC::fBetaAngle`
* `Xdc_gamma_angle`
    * Angle $\gamma$ of wires for each plane in radians. See also
      [link][SOS_tracking].
    * [double]
    * `THcDC::fGammaAngle`

[SOS_tracking]: https://hallcweb.jlab.org/DocDB/0008/000812/001/SOS_Tracking.pdf (Tracking in the SOS Spectrometer)

## `Xdc_tracking.param`

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
    * CAEN 1190 : 100 ps/click
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
    * Used for wire velocity correction in each plane.
    * [int]
    * `THcDC::fCentralTime`
* `Xdc_drifttime_sign`
    * Used for wire velocity correction in each plane.
    * [{-1, 1}]
    * `THcDC::fDriftTimeSign`
* `Xdc_fix_lr`
    * Used for wire velocity correction in each plane.
        * Let hit have different L/R assignment for different space point.
        * Should be 0 to mimic ENGINE behavior but 1 for more correct behavior.
    * {0, 1} bool
    * `THcDC::fFixLR`
* `Xdc_fix_propcorr`
    * Used for wire velocity correction in each plane.
        * Do propagation along wire each time hit appears in a space point.
    * {0, 1} bool
    * `THcDC::fFixPropagationCorrection`

### Velocity correction

The velocity correction is the distance from the center of the wire divided by
the velocity of propagation times Xdc_drifttime_sign(plane). +/-1 for disc. card
at +/- coord. (i.e. top = -x direction, so top readout is +1)

## `Xdriftmap.param`

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

## Deprecated

* `Xdc_num_deadwires`
    * Number of dead wires in drift chambers (< 20 % efficiency)
    * int
* `Xdc_deadwire_plane`
    * Plane number for each dead wire.
    * [int]
* `Xdc_deadwire_num`
    * Wire number for each dead wire.
    * [int]


* `Xdc_card_no`
    * Index of connector card for each wire.
        * Single array for all planes.
        * Wires start with plane 1, wire 1.
        * For each plane there must be 'max number of wires' entries.
    * [int]
* `Xdc_card_delay`
    * Cable delays for each connector card.
    * [int]
