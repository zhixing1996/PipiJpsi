#/bin/bash

BOSS=$1

if [ "$BOSS" = "703" ]; then
    ECMS=("4260")
    PATH="/besfs3/offline/data/703-1/xyz/round06"
fi

FILENAME="Gen_FileList_Data_"$BOSS
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    echo "rm -r /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/samples/data/$ECM/*txt" >> $FILENAME
    echo "./python/get_samples.py $PATH/$ECM/dst ./run/pipi_jpsi/samples/data/$ECM/data_${ECM}_pipi_jpsi.txt 20G" >> $FILENAME
done
