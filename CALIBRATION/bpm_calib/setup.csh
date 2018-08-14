#!/usr/bin/csh

# -----------------------------------------------------------------------------
#  Change these if this if not where hallc_replay and hcana live
setenv hcana_dir "/home/$USER/analysis/hcana"
setenv hallc_replay_dir "/home/$USER/analysis/hallc_replay"

# -----------------------------------------------------------------------------
#  Change if this gives you the wrong version of root, evio, etc
#source /site/12gev_phys/production.csh 2.1

# -----------------------------------------------------------------------------
# Source setup scripts
set curdir=`pwd`
cd $hcana_dir
source setup.csh
setenv PATH "$hcana_dir/bin:$PATH"
echo Sourced $hcana_dir/setup.csh

cd $hallc_replay_dir
source setup.csh
echo Sourced $hallc_replay_dir/setup.csh

echo cd back to $curdir
cd $curdir

