#! /bin/bash

SPEC="COIN"

# What is the last run number for the spectrometer.
# The pre-fix zero must be stripped because ROOT is ... well ROOT
#lastRun=$( \
#    ls raw/"${spec}"_all_*.dat raw/../raw.copiedtotape/"${spec}"_all_*.dat -R 2>/dev/null | perl -ne 'if(/0*(\d+)/) {prin#t "$1\n"}' | sort -n | tail -1 \
#)
lastRun=$( \
    ls raw/coin_all_*.dat raw/../raw.copiedtotape/coin_all_*.dat cache/coin_all_*.dat -R 2>/dev/null | perl -ne 'if(/0*(\d+)/) {print "$1\n"}' | sort -n | tail -1 \
)

# Which run to analyze.
runNum=$1
if [ -z "$runNum" ]; then
  runNum=$lastRun
fi

# How many events to analyze.
numEvents=-1

# Which scripts to run.
script="SCRIPTS/${SPEC}/PRODUCTION/replay_production_coin_hElec_pProt.C"
#config="CONFIG/${SPEC}/PRODUCTION/${spec}_coin_production.cfg"

# Which commands to run.
runHcana="./hcana -q \"${script}(${runNum}, ${numEvents})\""
#runOnlineGUI="./online -f ${config} -r ${runNum}"
#saveOnlineGUI="./online -f ${config} -r ${runNum} -P"

# Name of the replay ROOT file
rootFileDir="./ROOTfiles"
replayFile="coin_replay_production_hElec_pProt_${runNum}"
rootFile="${replayFile}_${numEvents}.root"
latestRootFile="${rootFileDir}/${replayFile}_latest.root"

# Names of the monitoring file
#monRootDir="./HISTOGRAMS/${SPEC}/ROOT"
#monPdfDir="./HISTOGRAMS/${SPEC}/PDF"
#monRootFile="${spec}_coin_production_${runNum}.root"
#monPdfFile="${spec}_coin_production_${runNum}.pdf"
#latestMonRootFile="${monRootDir}/${spec}_coin_production_latest.root"
#latestMonPdfFile="${monPdfDir}/${spec}_coin_production_latest.pdf"

# Where to put log.
reportFile="REPORT_OUTPUT/${SPEC}/PRODUCTION/replay_coin_ev7_production_${runNum}_${numEvents}.txt"
summaryFile="REPORT_OUTPUT/${SPEC}/PRODUCTION/summary_production_ev7_${runNum}_${numEvents}.txt"

# What is base name of onlineGUI output.
#outFile="${spec}_coin_production_${runNum}"

# Start analysis and monitoring plots.
{
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="
  echo "" 
  date
  echo ""
  echo "Running ${SPEC} COIN analysis on the run ${runNum}:"
  echo " -> SCRIPT:  ${script}"
  echo " -> RUN:     ${runNum}"
  echo " -> NEVENTS: ${numEvents}"
  echo " -> COMMAND: ${runHcana}"
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="

  sleep 2
  eval ${runHcana}

  # Link the ROOT file to latest for online monitoring
 # ln -fs ${rootFile} ${latestRootFile}
  
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="
  echo ""
  echo "Running onlineGUI for analyzed ${SPEC} COIN run ${runNum}:"
  echo " -> CONFIG:  ${config}"
  echo " -> RUN:     ${runNum}"
  echo " -> COMMAND: ${runOnlineGUI}"
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="

#  sleep 2
#  cd onlineGUI
#  eval ${runOnlineGUI}
#  eval ${saveOnlineGUI}
#  mv "${outFile}.pdf" "../HISTOGRAMS/${SPEC}/PDF/${outFile}.pdf"
#  mv "${outFile}.root" "../HISTOGRAMS/${SPEC}/ROOT/${outFile}.root"
#  cd ..
#  ln -fs ${monRootFile} ${latestMonRootFile}
#  ln -fs ${monPdfFile} ${latestMonPdfFile}

  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="
  echo ""
  echo "Done analyzing ${SPEC} run ${runNum}."
  echo ""
  echo ":=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:="

} 
#2>&1 | tee "${reportFile}"
#2>&1 | tee "${summaryFile}"
