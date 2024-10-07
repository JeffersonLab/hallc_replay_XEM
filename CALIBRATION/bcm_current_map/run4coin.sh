#!/bin/bash

list=$1

while read line
do

fname=/group/c-xem2/cmorean/FullExperiment/hallc_replay_XEM/ROOTfiles/COIN/SCALERS/coin_replay_scalers_${line}_-1.root

root -b<<EOF
.x run.C("${fname}", "P", ${line})
EOF

[ ! -d "coin" ] && `mkdir coin`

mv bcmcurrent* ./coin/

done < ${list}
