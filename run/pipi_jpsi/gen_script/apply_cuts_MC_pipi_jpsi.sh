#/bin/bash

BOSS=$1
TYPE=$2
SOURCE_PATH=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/rootfile/mc/$TYPE
ANA_PATH=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/anaroot/mc/$TYPE
LOG_PATH=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/logfile
mkdir -p $ANA_PATH
mkdir -p $LOG_PATH

if [ "$BOSS" = "703" ]; then
    ECMS=("4260")
fi

FILENAME="Apply_Cuts_${TYPE}_"$BOSS
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/python" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    mkdir -p $ANA_PATH/$ECM
    echo "./apply_cuts.py $SOURCE_PATH/$ECM/pipi_jpsi_mc_${TYPE}_${ECM}.root $ANA_PATH/$ECM/mc_${TYPE}_${ECM}_signal.root" >> $FILENAME
done
