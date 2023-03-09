#!/bin/bash


echo "================================================="
echo "For help on usage, type: ./xem_setup.sh -help"
echo "================================================="
Help()
{
    # Display Help
    echo "-------------------------------------------------------"
    echo "This shell script automatically sets up the necessary symbolic" 
    echo "links (or dir.) for the XEM experiment based on which "
    echo "machine (ifarm, cdaq) the user is at."
    echo ""
    echo "Syntax: ./xem_setup.sh [ -h | -f ]"
    echo ""
    echo "options:"
    echo "-help    Print this help display"
    echo ""
    echo "For users on IFARM ONLY: "
    echo "-f    ONLY use this option if you are running this shell script on ifarm. "
    echo "      This option selects filesystem in which to read/write data " 
    echo "      from the XEM experiment. "
    echo ""
    echo "      The optional arguments are: volatile, ... THATS IT!"
    echo ""
    echo "      volatile, work, group: "
    echo "            these options will set symbolic links to the corresponding filesystem "
    echo "            you would want to set these options depending on which stage of the analysis you are in "
    echo "            for example, use volatile if you are in the beginning stages of off-line analysis"
    echo ""
    echo "      See https://hallcweb.jlab.org/wiki/index.php/XEM_Disk_Space " 
    echo "      for detailed information on each of these filesystems."  
    echo "        "
    echo "examples: ./xem_setup.sh -f group"   
    echo "          ./xem_setup.sh -f volatile"
    echo "          ./xem_setup.sh -f work" 
    echo "-------------------------------------------------------"    
}

set_hcana_link()
{
    if [[ -z $HCANALYZER ]]; then	
	echo ""
	echo "Environment variable: $HCANALYZER does NOT exist. "
	echo "Please make sure to do: source setup.sh(csh) in hcana first. " 
	echo ""
    else
	echo ""
	echo "Creating hcana symbolic link now  . . ."
	ln -sf $HCANALYZER"/hcana"
	ln -sf $HCANALYZER"/libHallC.so"
	ln -sf $HCANALYZER"/libHallC.so.0.90.0"
	echo ""
    fi    
}


# initialize machine flags to 0
# (depending on where this script gets called, it will turn ON one of these)
ifarm_flg=0
cdaq_flg=0



# define the optional arguments
while getopts ":h:f:" option; do
    case $option in
	h) # display Help
            Help	    
	    exit;;       	
	f) # Enter a filesystem name (only applies for ifarm)
            fsys=$OPTARG;;
	\?) # Invalid option
            echo "Error: Invalid option"
	    Help
            exit;;
    esac
done


if echo $HOSTNAME | grep -q "ifarm"; then
    ifarm_flg=1
elif echo $HOSTNAME | grep -q "cdaq"; then
    cdaq_flg=1
 else
    echo "Unknown hostname, use ifarm or cdaq"
fi

#if [[ ifarm_flg==0 &&  cdaq_flg==0 ]]; then
#    echo "***************************************"
#    echo " Did not recognize remote machine. "
#    echo " Please run: ./xem_setup.sh -help "
#    echo " for help in running this script."
#    echo "***************************************"
#fi

#--- most recent raw data (irrespective of exp.)----
coda_raw="/net/cdaq/cdaql1data/coda/data/raw"
coda_raw_copiedtotape="/net/cdaq/cdaql1data/coda/data/raw.copiedtotape"

#--- define tape allocations ---
# make sure to point to the XEM 2022 tape dir
# since this 1st part will be taken during pionLT running.


# where XEM raw data output to be replayed will be stored (.dat files(
# but these are NOT directly accessible, one would have to look for them in cache.
# tape_raw_dir="/mss/hallc/c-xem-2022/raw"
tape_raw_dir="/mss/hallc/xem2/raw"

# tape volume for analysis output (simulation or replay output you want to keep long-term)
tape_analysis_out="/mss/hallc/xem2/analysis" 

#--- define cache allocations ---
# xem
cache_raw_dir_xem="/cache/hallc/xem2/raw/"
cache_raw_dir_sp18="/cache/hallc/spring17/raw/"
cache_raw_dir_sp19="/cache/hallc/jpsi-007/raw/"
cache_raw_dir_cafe="/cache/hallc/c-cafe-2022/raw/"
cache_raw_dir_pionlt="/cache/hallc/c-pionlt/raw"

cache_analysis_out="/cache/hallc/xem2/analysis/"

#=================================
# ifarm
# (off-line experiment analysis
# or testing the replay scripts)
#
# =================================
if [[ ifarm_flg -eq 1 ]]; then

    # source hallc_replay_XEM
    source setup.csh
    
    if [[ -z $fsys ]]; then
	echo ""
	echo " No optional argumnets provided "
	echo ""
	echo "----------------------------------------------------------------------"
	echo " For help using additional options, please run: ./xem_setup.sh -help "
	echo "----------------------------------------------------------------------"
	echo "Exiting NOW . . . "
	echo ""
	exit 1
    fi
    
    echo ""
    echo "Checking if necessary directories or symlinks exist in remote machine: " ${USER}"@"${HOSTNAME}". . ."
    echo ""
    
    # setup the symbolic links to hcana
    source /group/c-xem2/software/XEM_v2.0.0/hcana_firmware_update/hcana/setup.csh /group/c-xem2/software/XEM_v2.0.0/hcana_firmware_update/hcana/
    set_hcana_link
   
    
    if [[ $fsys == "volatile" ]]; then	     
	echo ""
	echo 'Setting up symbolic links to volatile filesystem on ifarm . . .'
	base_dir_voli="/volatile/hallc/xem2/"	

	echo "Creating dir $base_dir_voli$USER . . ."
	mkdir $base_dir_voli$USER
	
	CACHE_LINKS="./CACHE_LINKS/"
	if [ ! -d "$CACHE_LINKS" ]; then
	    mkdir $CACHE_LINKS
	else
	    rm -rf $CACHE_LINKS
	    mkdir $CACHE_LINKS
	fi
	
	echo "Creating symlink to ${cache_raw_dir_xem}"
	ln -sf $cache_raw_dir_xem $CACHE_LINKS/cache_xem2
	
	echo "Creating symlink to ${cache_raw_dir_sp18}"
	ln -sf $cache_raw_dir_sp18 $CACHE_LINKS/cache_sp18
    
	echo "Creating symlink to ${cache_raw_dir_sp19}"
	ln -sf $cache_raw_dir_sp19 $CACHE_LINKS/cache_sp19
	
	echo "Creating symlink to ${cache_raw_dir_pionlt}"
	ln -sf $cache_raw_dir_pionlt $CACHE_LINKS/cache_pionlt
	
	echo "Creating symlink to ${cache_raw_dir_cafe}"
	ln -sf $cache_raw_dir_cafe $CACHE_LINKS/cache_cafe
	
	echo "Creating dir and symlink to $base_dir_voli$USER/REPORT_OUTPUT . . ."
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT"
	ln -sf $base_dir_voli$USER"/REPORT_OUTPUT"
	#Creating REPORT_OUTPUT/SPEC Directories
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/COIN"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/SHMS"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/HMS"
	#Add COIN REPORT_OUTPUT Directories
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/COIN/REPLAY_REPORT"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/COIN/COMMISSIONING"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/COIN/CALIBRATION"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/COIN/PRODUCTION"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/COIN/SCALARS"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/COIN/HeeP"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/COIN/TIMING"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/COIN/shms50k"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/COIN/hms50k"
	#Add SHMS REPORT_OUTPUT Directories
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/SHMS/REPLAY_REPORT"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/SHMS/COMMISSIONING"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/SHMS/COMMISSIONING/HODO_CHECK"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/SHMS/CALIBRATION"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/SHMS/PRODUCTION"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/SHMS/SCALARS"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/SHMS/TIMING"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/SHMS/shms50k"
	#Add HMS REPORT_OUTPUT Directories
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/HMS/REPLAY_REPORT"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/HMS/COMMISSIONING"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/HMS/CALIBRATION"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/HMS/PRODUCTION"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/HMS/SCALARS"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/HMS/TIMING"
	mkdir $base_dir_voli$USER"/REPORT_OUTPUT/HMS/hms50k"

	
	echo "Creating dir and symlink to $base_dir_voli$USER/ROOTfiles . . ."
	mkdir $base_dir_voli$USER"/ROOTfiles"
	ln -sf $base_dir_voli$USER"/ROOTfiles"
	#Creating ROOTfiles/SPEC Directories
	mkdir $base_dir_voli$USER"/ROOTfiles/COIN"
	mkdir $base_dir_voli$USER"/ROOTfiles/SHMS"
	mkdir $base_dir_voli$USER"/ROOTfiles/HMS"
	#Add COIN ROOTfiles Directories
	mkdir $base_dir_voli$USER"/ROOTfiles/COIN/COMMISSIONING"
	mkdir $base_dir_voli$USER"/ROOTfiles/COIN/CALIBRATION"
	mkdir $base_dir_voli$USER"/ROOTfiles/COIN/PRODUCTION"
	mkdir $base_dir_voli$USER"/ROOTfiles/COIN/SCALARS"
	mkdir $base_dir_voli$USER"/ROOTfiles/COIN/TIMING"
	mkdir $base_dir_voli$USER"/ROOTfiles/COIN/HeeP"
	mkdir $base_dir_voli$USER"/ROOTfiles/COIN/shms50k"
	mkdir $base_dir_voli$USER"/ROOTfiles/COIN/hms50k"
	#Add SHMS ROOTfiles Directories
	mkdir $base_dir_voli$USER"/ROOTfiles/SHMS/COMMISSIONING"
	mkdir $base_dir_voli$USER"/ROOTfiles/SHMS/COMMISSIONING/HODO_CHECK"
	mkdir $base_dir_voli$USER"/ROOTfiles/SHMS/CALIBRATION"
	mkdir $base_dir_voli$USER"/ROOTfiles/SHMS/PRODUCTION"
	mkdir $base_dir_voli$USER"/ROOTfiles/SHMS/SCALARS"
	mkdir $base_dir_voli$USER"/ROOTfiles/SHMS/TIMING"
	mkdir $base_dir_voli$USER"/ROOTfiles/SHMS/shms50k"
	#Add HMS ROOTfiles Directories
	mkdir $base_dir_voli$USER"/ROOTfiles/HMS/COMMISSIONING"
	mkdir $base_dir_voli$USER"/ROOTfiles/HMS/CALIBRATION"
	mkdir $base_dir_voli$USER"/ROOTfiles/HMS/PRODUCTION"
	mkdir $base_dir_voli$USER"/ROOTfiles/HMS/SCALARS"
	mkdir $base_dir_voli$USER"/ROOTfiles/HMS/TIMING"
	mkdir $base_dir_voli$USER"/ROOTfiles/HMS/hms50k"
	
    elif [[ $fsys == "work" ]]; then	     
	echo ""
	echo 'Setting up symbolic links to work filesystem on ifarm . . .'
	echo 'Currently not implemented . . .'	
	
    elif [[ $fsys == "group" ]]; then	     
	echo ""
	echo 'Setting up symbolic links to group filesystem on ifarm . . .'
	base_dir_group="/group/c-xem-2022/"
	echo 'Currently not implemented . . .'	

    fi
fi


#===============================
# cdaq cluster
# (online experiment analysis)
#===============================

if [[ cdaq_flg -eq 1 ]]; then

    echo "Enter your CUE Username"
    read usr
    echo "Checking if necessary directories or symlinks exist in remote machine: " ${usr}"@"${HOSTNAME}". . ."

    # source xem_online_replay
    source setup.csh
    
    # setup the symbolic links to hcana
    set_hcana_link
    
    base_dir_cdaq="/net/cdaq/cdaql1data/cdaq/hallc-online-xem2022/USERS/${usr}"
    if [ -d "${base_dir_cdaq}" ]; then
	echo "Directory ${base_dir_cdaq} exists."
    else 
	mkdir $base_dir_cdaq
    fi

    echo "Creating symlink to ${coda_raw}"
    ln -sf $coda_raw

    echo "Creating symlink to ${coda_raw_copiedtotape}"
    ln -sf $coda_raw_copiedtotape 

    CACHE_LINKS="./CACHE_LINKS/"
    if [ ! -d "$CACHE_LINKS" ]; then
	mkdir $CACHE_LINKS
    else
	rm -rf $CACHE_LINKS
	mkdir $CACHE_LINKS
    fi
    
    echo "Creating symlink to ${cache_raw_dir_xem}"
    ln -sf $cache_raw_dir_xem $CACHE_LINKS/cache_xem2
    
    echo "Creating symlink to ${cache_raw_dir_sp18}"
    ln -sf $cache_raw_dir_sp18 $CACHE_LINKS/cache_sp18
    
    echo "Creating symlink to ${cache_raw_dir_sp19}"
    ln -sf $cache_raw_dir_sp19 $CACHE_LINKS/cache_sp19
    
    echo "Creating symlink to ${cache_raw_dir_pionlt}"
    ln -sf $cache_raw_dir_pionlt $CACHE_LINKS/cache_pionlt
    
    echo "Creating symlink to ${cache_raw_dir_cafe}"
    ln -sf $cache_raw_dir_cafe $CACHE_LINKS/cache_cafe

    echo "Creating dir and symlink to $base_dir_cdaq/REPORT_OUTPUT . . ."
    mkdir $base_dir_cdaq"/REPORT_OUTPUT"
    ln -sf $base_dir_cdaq"/REPORT_OUTPUT"
    #Creating REPORT_OUTPUT/SPEC Directories
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/COIN"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/SHMS"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/HMS"
    #Add COIN REPORT_OUTPUT Directories
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/COIN/REPLAY_REPORT"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/COIN/COMMISSIONING"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/COIN/CALIBRATION"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/COIN/PRODUCTION"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/COIN/SCALARS"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/COIN/TIMING"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/COIN/shms50k"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/COIN/hms50k"
    #Add SHMS REPORT_OUTPUT Directories
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/SHMS/REPLAY_REPORT"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/SHMS/COMMISSIONING"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/SHMS/COMMISSIONING/HODO_CHECK"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/SHMS/CALIBRATION"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/SHMS/PRODUCTION"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/SHMS/SCALARS"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/SHMS/TIMING"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/SHMS/shms50k"
    #Add HMS REPORT_OUTPUT Directories
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/HMS/REPLAY_REPORT"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/HMS/COMMISSIONING"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/HMS/CALIBRATION"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/HMS/PRODUCTION"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/HMS/SCALARS"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/HMS/TIMING"
    mkdir $base_dir_cdaq"/REPORT_OUTPUT/HMS/hms50k"

    
    echo "Creating dir and symlink to $base_dir_cdaq/ROOTfiles . . ."
    mkdir $base_dir_cdaq"/ROOTfiles"
    ln -sf $base_dir_cdaq"/ROOTfiles"
    #Creating ROOTfiles/SPEC Directories
    mkdir $base_dir_cdaq"/ROOTfiles/COIN"
    mkdir $base_dir_cdaq"/ROOTfiles/SHMS"
    mkdir $base_dir_cdaq"/ROOTfiles/HMS"
    #Add COIN ROOTfiles Directories
    mkdir $base_dir_cdaq"/ROOTfiles/COIN/COMMISSIONING"
    mkdir $base_dir_cdaq"/ROOTfiles/COIN/CALIBRATION"
    mkdir $base_dir_cdaq"/ROOTfiles/COIN/PRODUCTION"
    mkdir $base_dir_cdaq"/ROOTfiles/COIN/SCALARS"
    mkdir $base_dir_cdaq"/ROOTfiles/COIN/TIMING"
    mkdir $base_dir_cdaq"/ROOTfiles/COIN/HeeP"
    mkdir $base_dir_cdaq"/ROOTfiles/COIN/shms50k"
    mkdir $base_dir_cdaq"/ROOTfiles/COIN/hms50k"
    #Add SHMS ROOTfiles Directories
    mkdir $base_dir_cdaq"/ROOTfiles/SHMS/COMMISSIONING"
    mkdir $base_dir_cdaq"/ROOTfiles/SHMS/COMMISSIONING/HODO_CHECK"
    mkdir $base_dir_cdaq"/ROOTfiles/SHMS/CALIBRATION"
    mkdir $base_dir_cdaq"/ROOTfiles/SHMS/PRODUCTION"
    mkdir $base_dir_cdaq"/ROOTfiles/SHMS/SCALARS"
    mkdir $base_dir_cdaq"/ROOTfiles/SHMS/TIMING"
    mkdir $base_dir_cdaq"/ROOTfiles/SHMS/shms50k"
    #Add HMS ROOTfiles Directories
    mkdir $base_dir_cdaq"/ROOTfiles/HMS/COMMISSIONING"
    mkdir $base_dir_cdaq"/ROOTfiles/HMS/CALIBRATION"
    mkdir $base_dir_cdaq"/ROOTfiles/HMS/PRODUCTION"
    mkdir $base_dir_cdaq"/ROOTfiles/HMS/SCALARS"
    mkdir $base_dir_cdaq"/ROOTfiles/HMS/TIMING"
    mkdir $base_dir_cdaq"/ROOTfiles/HMS/hms50k"

    echo "Creating dir and symlink to $base_dir_cdaq/HISTOGRAMS . . ."
    mkdir $base_dir_cdaq"/HISTOGRAMS"
    ln -sf $base_dir_cdaq"/HISTOGRAMS"
    #Creating HISTOGRAMS/SPEC Directories
    mkdir $base_dir_cdaq"/HISTOGRAMS/COIN"
    mkdir $base_dir_cdaq"/HISTOGRAMS/COIN/ROOT"
    mkdir $base_dir_cdaq"/HISTOGRAMS/COIN/PDF"
    mkdir $base_dir_cdaq"/HISTOGRAMS/SHMS"
    mkdir $base_dir_cdaq"/HISTOGRAMS/SHMS/ROOT"
    mkdir $base_dir_cdaq"/HISTOGRAMS/SHMS/PDF"
    mkdir $base_dir_cdaq"/HISTOGRAMS/HMS"
    mkdir $base_dir_cdaq"/HISTOGRAMS/HMS/ROOT"
    mkdir $base_dir_cdaq"/HISTOGRAMS/HMS/PDF"

    echo "Creating dir and symlink to $base_dir_cdaq/MON_OUTPUT . . ."
    mkdir $base_dir_cdaq"/MON_OUTPUT"
    ln -sf $base_dir_cdaq"/MON_OUTPUT"
    #Creating MON_OUTPUT/SPEC Directories
    mkdir $base_dir_cdaq"/MON_OUTPUT/COIN"
    mkdir $base_dir_cdaq"/MON_OUTPUT/SHMS"
    mkdir $base_dir_cdaq"/MON_OUTPUT/HMS"
    mkdir $base_dir_cdaq"/MON_OUTPUT/COIN/REPORT"
    mkdir $base_dir_cdaq"/MON_OUTPUT/SHMS/REPORT"
    mkdir $base_dir_cdaq"/MON_OUTPUT/HMS/REPORT"
	
fi
