#/bin/bash

BOSS=$1

if [ "$BOSS" = "703" ]; then
    ECMS=("4260")
fi

for ECM in ${ECMS[@]}; do
    cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/rootfile/data/$ECM
    if [ "$ECM" == "4260" ]; then
        rm -rf pipi_jpsi_data_4260-0.root
    fi
    rm -rf pipi_jpsi_data_${ECM}.root
    hadd pipi_jpsi_data_${ECM}.root *.root
done
