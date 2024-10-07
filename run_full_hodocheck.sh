#! /bin/bash

# Which run to analyze.
runNum=$1
if [ -z "$runNum" ]; then
  echo "I need the run number!!!"
  exit 0
fi

# How many events to analyze.
numEvents=-1

# Which scripts to run.
production_script="SCRIPTS/SHMS/COMMISSIONING/replay_hodo_check_other.C"

# Which commands to run.
runProd="./hcana -l -b -q \"${production_script}(${runNum}, ${numEvents})\""
runScal="./hcana -l -b -q \"${scaler_script}(${runNum}, ${numEvents})\""

{
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="
  echo "" 
  date
  echo ""
  echo "Running production ${SPEC} replay on the run ${runNum}:"
  echo " -> SCRIPT:  ${production_script}"
  echo " -> RUN:     ${runNum}"
  echo " -> NEVENTS: ${numEvents}"
  echo " -> COMMAND: ${runProd}"
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="

  sleep 2
  eval ${runProd}

  echo "" 
  echo ""
  echo ""
  echo "-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|"
  echo ""
  echo "Happy Shifting!"
  echo ""
  echo "-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|"
  echo "" 
  echo ""
  echo ""

}
