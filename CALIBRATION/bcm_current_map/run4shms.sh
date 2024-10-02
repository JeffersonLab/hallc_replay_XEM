#!/bin/bash

list=$1

while read line
do

fname=/volatile/hallc/xem2/cmorean/FullExperiment/ROOTfiles/SHMS/SCALERS/shms_replay_scalers_${line}_-1.root

root -b<<EOF
.x run.C("${fname}", "P", ${line})
EOF

[ ! -d "shms" ] && `mkdir shms`

mv bcmcurrent* ./shms/

done < ${list}
