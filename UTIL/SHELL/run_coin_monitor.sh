#! /bin/bash

# Which run to use
runNum=$1
if [ -z "$runNum" ]; then
  echo " give Run Number to use"
  exit
fi

# which target
target=$2
if [ -z "$target" ]; then
  echo " give target type h or c "
  exit
fi

if [ "$target" = "h" ]; then
 targ=1
fi
if [ "$target" = "c" ]; then
 targ=2
fi 
if [ "$target" = "d" ]; then
 targ=3
fi 

echo " Grabbing monitored data for run ${runNum}, ${target} target"

# Grab monitored data and put it in the database
script="SCRIPTS/COIN/MONITOR/coin_monitor_calc.C"

runbc="root -q -l \"${script}(${runNum}, ${targ})\""
eval ${runbc}

# Plot this run, with 5 on either side
script="SCRIPTS/COIN/MONITOR/coin_monitor_graph.C"
runMin=$((runNum-10))
runMax=$((runNum+5))
runbc="root -l \"${script}(${runMin}, ${runMax})\""
eval ${runbc}


echo "If the plots look good quit root otherwise call an expert"
