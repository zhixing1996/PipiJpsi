#!/bin/sh
BOSS=$1
if [ "$BOSS" = "703" ]; then
    ECMS=("4260")
fi

WORKAREA=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi
for ECM in ${ECMS[@]}; do
    mkdir -p $WORKAREA/run/pipi_jpsi/rootfile/mc/JPIPI/$ECM
    mkdir -p $WORKAREA/run/pipi_jpsi/jobs_text/mc/JPIPI/$ECM
    cd $WORKAREA/run/pipi_jpsi/jobs_text/mc/JPIPI/$ECM
    rm -rf mc_PI_PI_JPSI_JPIPI_$ECM*txt
    cp -rf $WORKAREA/python/make_mc.py ./
    cp -rf $WORKAREA/python/tools.py ./
    ./make_mc.py /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/dst/mc/JPIPI/$ECM mc PI_PI_JPSI JPIPI JPIPI $ECM 2
    cp -rf /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/gen_script/gen_mc/subAna.sh ./
    rm -rf *boss*
    rm -rf $WORKAREA/run/pipi_jpsi/rootfile/mc/JPIPI/$ECM/*root
    ./subAna.sh $ECM PI_PI_JPSI_JPIPI
done
