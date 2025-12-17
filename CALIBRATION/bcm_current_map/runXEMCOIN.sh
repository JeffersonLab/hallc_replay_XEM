#!/bin/bash

# Make the directory structure for the current maps if it does not exist
# NOTE: This creates the directories in the local folder you are running the 
#   script from, which is where the output param files will be placed. This
#   is intentional so that the output can be validated prior to overwriting
#   any existing param files.
# COINCIDENCE scripts load from the SHMS folder
mkdir -p PARAM/SHMS/BCM

# Directory containing the files
base_dir="/cache/hallc/xem2/analysis/OFFLINE/REPLAYS/COIN/HeeP/pass2.1"

# Loop over all files matching the pattern
for file in "$base_dir"/coin_replay_production_*_-1.root; do
  # Extract the run number from the file name
  if [[ $file =~ coin_replay_production_([0-9]+)_-1\.root ]]; then
    run="${BASH_REMATCH[1]}"
    echo "Processing file: $file"
    echo "Run number: $run"
    
    # Add your processing logic here
root -l -b<<EOF
.x run.C("${file}", "P", ${run})
EOF
  fi
done
