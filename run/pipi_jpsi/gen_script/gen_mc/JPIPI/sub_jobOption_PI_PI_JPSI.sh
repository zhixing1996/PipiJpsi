#/bin/bash
BOSS=$1
if [ "$BOSS" = "703" ]; then
    ECMS=("4260")
    CMS=("4.25797")
    RUNNO_LOW=("29677")
    RUNNO_UP=("30367")
fi

COUN=0
FILENAME="Sub_jobOption_"$BOSS
for ECM in ${ECMS[@]}; do
    JobText_SaveDir=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/jobs_text/mc/JPIPI/$ECM
    Script_Dir=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/gen_script/gen_mc/JPIPI
    mkdir -p $JobText_SaveDir
    rm -rf $JobText_SaveDir/jobOptions*txt
    rm -rf $JobText_SaveDir/subSimRec_*.sh
    rm -rf $JobText_SaveDir/fort.*
    rm -rf $JobText_SaveDir/phokhara*
    cd $JobText_SaveDir
    cp -r $Script_Dir/jobOptions_sim_sig_PI_PI_JPSI_JPIPI_tempE_705.sh jobOptions_sim_sig_PI_PI_JPSI_JPIPI_${ECM}_705.sh
    cp -r $Script_Dir/jobOptions_rec_sig_PI_PI_JPSI_JPIPI_tempE_705.sh jobOptions_rec_sig_PI_PI_JPSI_JPIPI_${ECM}_705.sh
    sh jobOptions_sim_sig_PI_PI_JPSI_JPIPI_${ECM}_705.sh 0 49 $ECM ${CMS[$COUNT]} 5000 ${RUNNO_LOW[$COUNT]} ${RUNNO_UP[$COUNT]}
    sh jobOptions_rec_sig_PI_PI_JPSI_JPIPI_${ECM}_705.sh 0 49 $ECM
    rm -rf /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/rtraw/JPIPI/$ECM/*.rtraw
    rm -rf /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/dst/JPIPI/$ECM/*.dst
    cp -rf /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/gen_script/gen_mc/subSimRec.sh ./
    sh subSimRec.sh jobOptions_sim_sig_PI_PI_JPSI_JPIPI_$ECM jobOptions_rec_sig_PI_PI_JPSI_JPIPI_$ECM subSimRec_PI_PI_JPSI_$ECM 0 49
    COUNT=$((${COUNT}+1))
done
