#!/bin/bash

list=$1

while read line
do

fname=/volatile/hallc/xem2/cmorean/ROOTfiles_XEM/hms_replay_scalers_${line}_-1.root

root -b<<EOF
.x run.C("${fname}", "H")
EOF

[ ! -d "hms" ] && `mkdir hms`

mv bcmcurrent* ./hms/

done < ${list}
