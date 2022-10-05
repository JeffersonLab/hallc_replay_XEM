#! /bin/bash

# Which run to analyze.
runNum=$1
if [ -z "$runNum" ]; then
  echo "I need the run number!!!"
  exit 0
fi

# How many events to analyze.
numEvents=$2
if [ -z "$numEvents" ]; then
    numEvents=-1
fi

# Which scripts to run.
script="SCRIPTS/COIN/PRODUCTION/replay_production_coin_pElec_hProt.C"
counts="coinElas.C"
#counts="sDeltaScan.C"

# Define some useful directories
coincountsDir="UTIL_XEM/HeeP"
# Which commands to run.
runHcana="./hcana -l -b -q \"${script}(${runNum}, ${numEvents})\""
runCounts="root -l \"${counts}(${runNum}, ${numEvents})\""

# Start analysis and monitoring plots.
{
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="
  echo "" 
  date
  echo ""
  echo "Running HeeP ${SPEC} analysis on the run ${runNum}:"
  echo " -> SCRIPT:  ${script}"
  echo " -> RUN:     ${runNum}"
  echo " -> NEVENTS: ${numEvents}"
  echo " -> COMMAND: ${runHcana}"
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="

  sleep 2
  eval ${runHcana}

  echo "" 
  echo ""
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="
  echo ""
  echo "Running Elastic Counts for analyzed HeeP ${SPEC} run ${runNum}:"
  echo " -> SRIPT:  ${counts}"
  echo " -> RUN:     ${runNum}"
  echo " -> NEVENTS: ${numEvents}"
  echo " -> COMMAND: ${runCounts}"
  echo " -> Directory: ${coincountsDir}"
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="

  sleep 2
  (cd UTIL_XEM/HeeP;eval ${runCounts})
  echo "" 
  echo ""
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="
  echo ""
  echo "Done analyzing HeeP ${SPEC} run ${runNum}."
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="

  echo "" 
  echo ""
  echo ""
  echo "-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|"
  echo ""
  echo "So long and thanks for all the elastics!"
  echo ""
  echo "-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|"
  echo "" 
  echo ""
  echo ""

}
