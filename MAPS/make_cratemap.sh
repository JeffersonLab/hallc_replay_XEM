#!/bin/bash
# Shell script to merge multiple crate maps into one
# Author: Eric Pooser, pooser@jlab.org, 11/17/16

echo
read -p "Enter the crate(s), separated by spaces, you would like form a single crate map for > " list

arr=($list)

for crate in "${arr[@]}"; do
    if ((${crate} < 1)) || ((${crate} > 7)); then
	echo "You must enter an integer value in the range [1, 7]"
	exit
    elif ((crate%2 == 1)) && ((crate != 7)); then
	(cat ${PWD}/HMS/db_crate${crate}map.dat; echo; echo) >> db_cratemap.dat
	echo "HMS  Crate ${crate} has been added to db_cratemap.dat"
    elif ((crate%2 == 0)) || ((crate == 7)); then
	(cat ${PWD}/SHMS/db_crate${crate}map.dat; echo; echo) >> db_cratemap.dat
	echo "SHMS Crate ${crate} has been added to db_cratemap.dat"
    fi
done

