#!/bin/sh

rm -rf jobs.out
mkdir jobs.out
rm -rf jobs.err
mkdir jobs.err

SIM=$1
REC=$2
SUB=$3
INPUT=$4
UPLIMIT=$5
echo "./subjectSimRec.sh [SIM] [REC] [SUM] [NUM1] [NUM2]"
echo "[SIM]: name of sim scripts"
echo "[REC]: name of rec scripts"
echo "[SUB]: name of sub scripts"
echo "[NUM1]: the minimum number range of job subjected"
echo "[NUM2]: the maximum number range of job subjected"

# subject jobs
echo "subject jobs"

until [ $INPUT -gt $UPLIMIT ]
do
    
    SIM_NAME="boss.exe "$SIM"_"$INPUT".txt"
    REC_NAME="boss.exe "$REC"_"$INPUT".txt"
    SUB_NAME=$SUB"_"$INPUT".sh"
    touch $SUB_NAME
    chmod u+x $SUB_NAME

    echo "#!/bin/sh" > $SUB_NAME
    echo $SIM_NAME >> $SUB_NAME
    echo "sleep 50" >> $SUB_NAME
    echo $REC_NAME >> $SUB_NAME
    
    echo $SUB_NAME" done!"

    hep_sub -g physics $SUB_NAME -o ./jobs.out -e ./jobs.err

    INPUT=$(($INPUT+1))
  
done

echo "all done!"   
