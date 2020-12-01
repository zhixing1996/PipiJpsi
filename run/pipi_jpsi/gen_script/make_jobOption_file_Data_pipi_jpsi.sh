#!/bin/bash

BOSS=$1
if [ "$BOSS" = "703" ]; then
    ECMS=("4260")
    CMS=("4.25797")
fi

COUNT=0
for ECM in ${ECMS[@]}; do
    NUM_UP=$(ls -l /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/samples/data/$ECM | grep "txt" | wc -l)
    JobText_SaveDir=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/jobs_text/data/$ECM
    ROOT_SaveDir=/besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/rootfile/data/$ECM
    mkdir -p $JobText_SaveDir
    mkdir -p $ROOT_SaveDir
    rm -r $JobText_SaveDir/*txt
    rm -r $ROOT_SaveDir/*.root
    
    for ((num = 0; num <= $NUM_UP; num++))
    do
        file_list=data_${ECM}_pipi_jpsi_20G-${num}.txt
        rootfile=pipi_jpsi_data_$ECM-${num}.root
        jobOptions=jobOptions_pipi_jpsi_data_$ECM-${num}.txt
        echo "#include \"\$ROOTIOROOT/share/jobOptions_ReadRec.txt\"                                             "  > ${JobText_SaveDir}/${jobOptions}
        echo "#include \"\$VERTEXFITROOT/share/jobOptions_VertexDbSvc.txt\"                                      " >> ${JobText_SaveDir}/${jobOptions}
        echo "#include \"\$MAGNETICFIELDROOT/share/MagneticField.txt\"                                           " >> ${JobText_SaveDir}/${jobOptions}
        echo "#include \"\$ABSCORROOT/share/jobOptions_AbsCor.txt\"                                              " >> ${JobText_SaveDir}/${jobOptions}
        echo "#include \"/besfs/groups/cal/dedx/\$USER/bes/PipiJpsi/run/pipi_jpsi/samples/data/$ECM/$file_list\" " >> ${JobText_SaveDir}/${jobOptions}
        echo "                                                                                                   " >> ${JobText_SaveDir}/${jobOptions}
        echo "ApplicationMgr.DLLs += {\"pipi_jpsiAlg\"};                                                         " >> ${JobText_SaveDir}/${jobOptions}
        echo "ApplicationMgr.TopAlg += { \"pipi_jpsi\" };                                                        " >> ${JobText_SaveDir}/${jobOptions}
        echo "pipi_jpsi.Ecms = ${CMS[$COUNT]};                                                                   " >> ${JobText_SaveDir}/${jobOptions}
        echo "                                                                                                   " >> ${JobText_SaveDir}/${jobOptions}
        echo "// Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )                      " >> ${JobText_SaveDir}/${jobOptions}
        echo "MessageSvc.OutputLevel = 5;                                                                        " >> ${JobText_SaveDir}/${jobOptions}
        echo "                                                                                                   " >> ${JobText_SaveDir}/${jobOptions}
        echo "// Number of events to be processed (default is 10)                                                " >> ${JobText_SaveDir}/${jobOptions}
        echo "ApplicationMgr.EvtMax = -1;                                                                        " >> ${JobText_SaveDir}/${jobOptions}
        echo "                                                                                                   " >> ${JobText_SaveDir}/${jobOptions}
        echo "ApplicationMgr.HistogramPersistency = \"ROOT\";                                                    " >> ${JobText_SaveDir}/${jobOptions}
        echo "NTupleSvc.Output = {\"FILE1 DATAFILE='$ROOT_SaveDir/$rootfile' OPT='NEW' TYP='ROOT'\"};            " >> ${JobText_SaveDir}/${jobOptions}
    done
    COUNT=$COUNT+1
done
