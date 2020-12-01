#/bin/bash

ECMS=$1
TYPE=$2

find . -name "*.bosslog" | xargs rm
find . -name "*.bosserr" | xargs rm
NUM_UP=$(ls -l | grep "mc_" | wc -l)
boss.condor -g physics -n $NUM_UP mc_${TYPE}_${ECMS}_%{ProcId}.txt
