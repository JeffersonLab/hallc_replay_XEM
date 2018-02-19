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

echo " counting for ${target} target"

# Which scripts to run.
script="SCRIPTS/COIN/MONITOR/Bean_counter.C"

# Which commands to run.
runbc="root -l \"${script}(${runNum}, ${targ})\""
eval ${runbc}

echo "If the plots look good quit root otherwise call an expert"
