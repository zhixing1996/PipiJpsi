#!/bin/sh

INPUT=$1
UPLIMIT=$2
ENERGYPOINT=$3

echo "./jobOptions_rec_sig_PI_PI_JPSI_JPIPI_${ENERGYPOINT}.sh [NUM1] [NUM2]"
echo "[NUM1]: the minimum number range of job generated"
echo "[NUM2]: the maximum number range of job generated"

SEED=123
 
DIR_NAME_RTRAW="/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/rtraw/mc/JPIPI/$ENERGYPOINT/"
DIR_NAME_DST="/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/dst/mc/JPIPI/$ENERGYPOINT/"
mkdir -p $DIR_NAME_DST
EVENT_NO=-1

JOB_NAME="jobOptions_rec_sig_PI_PI_JPSI_JPIPI_$ENERGYPOINT"
INPUT_NAME="Sig_PI_PI_JPSI_JPIPI_$ENERGYPOINT"
OUTPUT_NAME="Sig_PI_PI_JPSI_JPIPI_$ENERGYPOINT"

# steer file for reconstruction
echo "steer file for reconstruction"

until [ $INPUT -gt $UPLIMIT ]
do

    INPUTFILE_NAME=$INPUT_NAME"_"$INPUT".rtraw"
    OUTPUTFILE_NAME=$OUTPUT_NAME"_"$INPUT".dst"
    REC_NAME=$JOB_NAME"_"$INPUT".txt"

    touch $REC_NAME

    echo "//input ROOT MC data" > $REC_NAME
    echo "#include \"\$ROOTIOROOT/share/jobOptions_ReadRoot.txt\"" >> $REC_NAME
    echo "#include \"\$OFFLINEEVENTLOOPMGRROOT/share/OfflineEventLoopMgr_Option.txt\"" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "// background mixing" >> $REC_NAME
    echo "#include \"\$BESEVENTMIXERROOT/share/jobOptions_EventMixer_rec.txt\"" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "#include \"\$CALIBSVCROOT/share/job-CalibData.txt\"" >> $REC_NAME
    echo "#include \"\$MAGNETICFIELDROOT/share/MagneticField.txt\"" >> $REC_NAME
    echo "#include \"\$ESTIMEALGROOT/share/job_EsTimeAlg.txt\"" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "// PAT+TSF+HOUGH method for MDC reconstruction" >> $REC_NAME
    echo "#include \"\$MDCHOUGHFINDERROOT/share/jobOptions_MdcPatTsfHoughRec.txt\"" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "#include \"\$KALFITALGROOT/share/job_kalfit_numf_data.txt\"" >> $REC_NAME
    echo "#include \"\$MDCDEDXALGROOT/share/job_dedx_all.txt\"" >> $REC_NAME
    echo "#include \"\$TRKEXTALGROOT/share/TrkExtAlgOption.txt\"" >> $REC_NAME
    echo "#include \"\$TOFRECROOT/share/jobOptions_TofRec.txt\"" >> $REC_NAME
    echo "#include \"\$TOFENERGYRECROOT/share/TofEnergyRecOptions_MC.txt\"" >> $REC_NAME
    echo "#include \"\$EMCRECROOT/share/EmcRecOptions.txt\"" >> $REC_NAME
    echo "#include \"\$MUCRECALGROOT/share/jobOptions_MucRec.txt\"" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "#include \"\$EVENTASSEMBLYROOT/share/EventAssembly.txt\"" >> $REC_NAME
    echo "#include \"\$PRIMARYVERTEXALGROOT/share/jobOptions_kalman.txt\"" >> $REC_NAME
    echo "#include \"\$VEEVERTEXALGROOT/share/jobOptions_veeVertex.txt\"" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "#include \"\$HLTMAKERALGROOT/share/jobOptions_HltMakerAlg.txt\"" >> $REC_NAME
    echo "#include \"\$EVENTNAVIGATORROOT/share/EventNavigator.txt\"" >>$REC_NAME
    echo "" >> $REC_NAME
    echo "//output ROOT REC data" >> $REC_NAME
    echo "#include \"\$ROOTIOROOT/share/jobOptions_Dst2Root.txt\"" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "//configure of calibration constants for MC" >> $REC_NAME
    echo "#include \"\$CALIBSVCROOT/share/calibConfig_rec_mc.txt\"" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "//**************job options for random number************************" >> $REC_NAME
    echo "BesRndmGenSvc.RndmSeed = $SEED;" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "//Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )" >> $REC_NAME
    echo "MessageSvc.OutputLevel = 5;" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "//ROOT input data file" >> $REC_NAME
    echo "EventCnvSvc.digiRootInputFile = {\"$DIR_NAME_RTRAW$INPUTFILE_NAME\"};" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "//ROOT output data file" >> $REC_NAME
    echo "EventCnvSvc.digiRootOutputFile =\"$DIR_NAME_DST$OUTPUTFILE_NAME\";" >> $REC_NAME
    echo "" >> $REC_NAME
    echo "//Number of events to be processed (default is 10)" >> $REC_NAME
    echo "ApplicationMgr.EvtMax = $EVENT_NO;" >> $REC_NAME
    echo "" >> $REC_NAME
    
    echo $REC_NAME" done!"   

    INPUT=$(($INPUT+1))
    SEED=$(($SEED+1))

done

echo "all done!"   
