#/bin/bash

BOSS=$1
SOURCE_PATH=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/rootfile/data
ANA_PATH=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/anaroot/data
LOG_PATH=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/logfile
mkdir -p $ANA_PATH
mkdir -p $LOG_PATH

if [ "$BOSS" = "703" ]; then
    ECMS=("4260")
fi

FILENAME="Apply_Cuts_Data_"$BOSS
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/python" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    mkdir -p $ANA_PATH/$ECM
    echo "./apply_cuts.py $SOURCE_PATH/$ECM/pipi_jpsi_data_${ECM}.root $ANA_PATH/$ECM/data_${ECM}_signal.root" >> $FILENAME
done
