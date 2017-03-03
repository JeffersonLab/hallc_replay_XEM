#!/bin/bash
# Shell script to merge multiple crate maps into one
# Author: Eric Pooser, pooser@jlab.org, 11/17/16

function proceed {
    rm -f db_cratemap.dat
    echo ;read -p "Enter the crate(s), separated by spaces, you would like to form a single crate map for > " list
    arr=($list)
    for crate in "${arr[@]}"; do
	if ((${crate} < 1)) || ((${crate} > 7)); then
	    echo; echo "You must enter an integer value in the range [1, 7]"; echo
	    exit
	fi
    done
    for crate in "${arr[@]}"; do
	if ((crate%2 == 1)) && ((crate != 7)); then
	    (cat ${PWD}/HMS/CRATE/db_crate${crate}map.dat; echo; echo) >> db_cratemap.dat
	    echo; echo "HMS  Crate ${crate} has been added to db_cratemap.dat"
	elif ((crate%2 == 0)) || ((crate == 7)); then
	    (cat ${PWD}/SHMS/CRATE/db_crate${crate}map.dat; echo; echo) >> db_cratemap.dat
	    echo; echo "SHMS Crate ${crate} has been added to db_cratemap.dat"
	    if ((crate == 7)); then echo; fi
	fi
    done
}

function ask_proceed {
    read -p "Would you like to proceed? (yes or no) > " response
    if [[ $response = "no" ]]; then
	echo; "Please rename/move db_cratemap.dat in the current working directory before proceeding"; echo
	exit
    elif [[ $response = "yes" ]]; then
	proceed
    else
	echo; echo "Please respond by specifically typing yes or no"; echo; exit
    fi
}

file=( $(find db_cratemap.dat -type f 2> /dev/null) )
if [[ ${file[0]} = "db_cratemap.dat" ]]; then
    echo; echo "CAUTION: The file db_cratemap.dat will be created/overwritten!!!"; echo
    ask_proceed
else
    proceed
fi
