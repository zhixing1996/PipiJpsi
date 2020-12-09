#/bin/bash

BOSS=$1
TYPE=$2

if [ "$BOSS" = "703" ]; then
    ECMS=("4260")
fi

for ECM in ${ECMS[@]}; do
    cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/rootfile/mc/$TYPE/$ECM
    rm -rf pipi_jpsi_mc_${TYPE}_${ECM}.root
    hadd pipi_jpsi_mc_${TYPE}_${ECM}.root *.root
done
