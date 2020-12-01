#/bin/bash

BOSS=$1

if [ "$BOSS" = "703" ]; then
    ECMS=("4260")
fi

for ECM in ${ECMS[@]}; do
    cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/jobs_text/data/$ECM
    find . -name "*.bosslog" | xargs rm
    find . -name "*.bosserr" | xargs rm
    NUM_UP=$(ls -l | grep "txt" | wc -l)
    boss.condor -g physics -n $NUM_UP jobOptions_pipi_jpsi_data_$ECM-%{ProcId}.txt
done
