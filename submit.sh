#!/usr/bin/env bash

# Main driver to submit jobs 
# Author SHI Xin <shixin@ihep.ac.cn>
# Modified by JING Maoqiang <jingmq@ihep.ac.cn>
# Created [2019-12-11 Dec 14:56]

usage() {
    printf "NAME\n\tsubmit.sh - Main driver to submit jobs\n"
    printf "\nSYNOPSIS\n"
    printf "\n\t%-5s\n" "./submit.sh [OPTION]" 
    printf "\nOPTIONS\n" 
    printf "\n\t%-9s  %-40s"  "0.1"       "[run on data sample for pipi_jpsi]"
    printf "\n\t%-9s  %-40s"  "0.2"       "[run on inclusive MC sample for pipi_jpsi]"
    printf "\n\t%-9s  %-40s"  "0.3"       "[generate and run on signal MC sample]"
    printf "\n\n" 
}

usage_0_1() {
    printf "\n\t%-9s  %-40s"  ""          ""   
    printf "\n\t%-9s  %-40s"  "0.1.1"     "Split data sample with each group 5G"
    printf "\n\t%-9s  %-40s"  "0.1.2"     "Generate Condor jobs on data ---- 1"
    printf "\n\t%-9s  %-40s"  "0.1.3"     "Test for data"
    printf "\n\t%-9s  %-40s"  "0.1.4"     "Submit Condor jobs on data ---- 2"
    printf "\n\t%-9s  %-40s"  "0.1.5"     "Synthesize data root files"
    printf "\n\t%-9s  %-40s"  "0.1.6"     "Apply cuts"
    printf "\n\t%-9s  %-40s"  "0.1.7"     "Plot invariant mass of jpsi"
    printf "\n\t%-9s  %-40s"  ""           ""
    printf "\n"
}

usage_0_2() {
    printf "\n\t%-9s  %-40s"  ""          ""
    printf "\n\t%-9s  %-40s"  "0.2.1"     "Generate Condor jobs on signal MC(simulation) ---- 1"
    printf "\n\t%-9s  %-40s"  "0.2.2"     "Test for signal MC(simulation)"
    printf "\n\t%-9s  %-40s"  "0.2.3"     "Submit Condor jobs on signal MC(simulation) ---- 2"
    printf "\n\t%-9s  %-40s"  "0.2.4"     "Generate Condor jobs on signal MC(reconstruction) ---- 1"
    printf "\n\t%-9s  %-40s"  "0.2.5"     "Test for signal MC(reconstruction)"
    printf "\n\t%-9s  %-40s"  "0.2.6"     "Submit Condor jobs on signal MC(reconstruction) ---- 2"
    printf "\n\t%-9s  %-40s"  "0.2.7"     "Split signal MC sample with each group 5G(rootfile)"
    printf "\n\t%-9s  %-40s"  "0.2.8"     "Generate Condor jobs on signal MC(rootfile) ---- 1"
    printf "\n\t%-9s  %-40s"  "0.2.9"     "Test for signal MC(rootfile)"
    printf "\n\t%-9s  %-40s"  "0.2.10"    "Submit Condor jobs on signal MC(rootfile) ---- 2"
    printf "\n\t%-9s  %-40s"  ""           ""
    printf "\n"
}

if [[ $# -eq 0 ]]; then
    usage
    echo "Please enter your option: "
    read option
else
    option=$1
fi

sub_0_1() {

case $option in
    
    # --------------------------------------------------------------------------
    #  Data  
    # --------------------------------------------------------------------------

    0.1.1) echo "Split data sample with each group 20G ..."
           cd ./run/pipi_jpsi/gen_script
           ./make_file_list_Data_pipi_jpsi.sh 703
           chmod u+x Gen_FileList_Data_703
           bash Gen_FileList_Data_703
           rm -r Gen_FileList_Data_703
           cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi
	       ;;

    0.1.2) echo "Generate Condor jobs on data ---- 1..." 
	       cd ./run/pipi_jpsi/gen_script
	       ./make_jobOption_file_Data_pipi_jpsi.sh 703
	       cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/jobs_text/data/4260
	       cp -r jobOptions_pipi_jpsi_data_4260-1.txt jobOptions_pipi_jpsi_data_4260-0.txt
           sed -i "s/ApplicationMgr\.EvtMax = -1/ApplicationMgr\.EvtMax = 2000/g" jobOptions_pipi_jpsi_data_4260-0.txt
           sed -i "s/pipi_jpsi_data_4260-1\.root/pipi_jpsi_data_4260-0\.root/g" jobOptions_DDbarPi_data_4260-0.txt
	       ;;

    0.1.3) echo "Test for data" 
           echo "have you changed test number?(yes / no)
           ./run/pipi_jpsi/jobs_text/data/4260/jobOptions_pipi_jpsi_data_4260-0.txt"
           read opt
           if [ $opt == "yes" ]
               then
               echo "now in yes"  
               cd ./run/pipi_jpsi/jobs_text/data/4260
               boss.exe jobOptions_pipi_jpsi_data_4260-0.txt
           else
               echo "Default value is 'no', please change test number."
           fi
           ;;

    0.1.4) echo "Submit Condor jobs on data ---- 2..." 
           cd ./run/pipi_jpsi/gen_script
           ./sub_jobOption_file_Data_pipi_jpsi.sh 703
	       ;;

    0.1.5) echo "Synthesize data root files..."
           cd /scratchfs/bes/$USER/bes3ws2020/data12/psip
           rm -rf data12_psip.root
           hadd data12_psip.root *.root
	       ;;

    0.1.6) echo "Apply cuts..."
           mkdir -p /besfs/users/$USER/bes3ws2020/data12/psip
           ./python/apply_cuts.py /scratchfs/bes/$USER/bes3ws2020/data12/psip/data12_psip.root /besfs/users/$USER/bes3ws2020/data12/psip/data12_psip.root
	       ;;

    0.1.7) echo "Plot invariant mass of jpsi..."
           ./python/plot_m_jpsi.py /besfs/users/$USER/bes3ws2020/data12/psip/data12_psip.root
           ;;
 
esac

}

sub_0_2() {

case $option in
    
    # --------------------------------------------------------------------------
    #  Signal MC  
    # --------------------------------------------------------------------------

    0.2.1) echo "Generate Condor jobs on signal MC(simulation) ---- 1 ..."
           cd run/pipi_jpsi/gen_script/gen_mc
           ./make_jobOption_file_pipi_jpsi_sim_664p03.sh
           cd ../../jobs_text/sigMC/jobs_sig
           cp -r jobOptions_pipi_jpsi_sigMC_psip_sim-300.txt jobOptions_pipi_jpsi_sigMC_psip_sim-0.txt
           sed -i "s/ApplicationMgr\.EvtMax = 5000/ApplicationMgr\.EvtMax = 50/g" jobOptions_pipi_jpsi_sigMC_psip_sim-0.txt
	       # made 300 groups
	       ;;

    0.2.2) echo "Test for signal MC(simulation)" 
           echo "have you changed test number?(yes / no)
           ./run/pipi_jpsi/job_text/sigMC/jobs_sig/jobOptions_pipi_jpsi_sigMC_psip_sim-0.txt"
           read opt
           if [ $opt == "yes" ]
               then
               echo "now in yes"  
               cd run/pipi_jpsi/jobs_text/sigMC/jobs_sig
               boss.exe jobOptions_pipi_jpsi_sigMC_psip_sim-0.txt
           else
               echo "Default value is 'no', please change test number."
           fi
           ;;

    0.2.3) echo "Submit Condor jobs on signal MC(simulation) ---- 2..." 
	       cd run/pipi_jpsi/jobs_text/sigMC/jobs_sig
           find . -name "*.out.*" | xargs rm
           find . -name "*.err.*" | xargs rm
		   rm /scratchfs/bes/$USER/bes3ws2020/sigMC/psip/pipi_jpsi/rtraw/*.rtraw
		   boss.condor -g physics -n 5 jobOptions_pipi_jpsi_sigMC_psip_sim-%{ProcId}.txt
	       ;;

    0.2.4) echo "Generate Condor jobs on signal MC(reconstruction) ---- 1 ..."
           cd run/pipi_jpsi/gen_script/gen_mc
           ./make_jobOption_file_pipi_jpsi_rec_664p03.sh
           cd ../../jobs_text/sigMC/jobs_sig
           cp -r jobOptions_pipi_jpsi_sigMC_psip_rec-300.txt jobOptions_pipi_jpsi_sigMC_psip_rec-0.txt
           sed -i "s/ApplicationMgr\.EvtMax = -1/ApplicationMgr\.EvtMax = 50/g" jobOptions_pipi_jpsi_sigMC_psip_rec-0.txt
	       # made 300 groups
	       ;;

    0.2.5) echo "Test for signal MC(reconstruction)" 
           echo "have you changed test number?(yes / no)
           ./run/pipi_jpsi/job_text/sigMC/jobs_sig/jobOptions_pipi_jpsi_sigMC_psip_rec-0.txt"
           read opt
           if [ $opt == "yes" ]
               then
               echo "now in yes"  
               cd run/pipi_jpsi/jobs_text/sigMC/jobs_sig
               boss.exe jobOptions_pipi_jpsi_sigMC_psip_rec-0.txt
           else
               echo "Default value is 'no', please change test number."
           fi
           ;;

    0.2.6) echo "Submit Condor jobs on signal MC(reconstruction) ---- 2..." 
	       cd run/pipi_jpsi/jobs_text/sigMC/jobs_sig
           find . -name "*.out.*" | xargs rm
           find . -name "*.err.*" | xargs rm
		   rm /scratchfs/bes/$USER/bes3ws2020/sigMC/psip/pipi_jpsi/dst/*.dst
		   boss.condor -g physics -n 5 jobOptions_pipi_jpsi_sigMC_psip_rec-%{ProcId}.txt
	       ;;

    0.2.7) echo "Split signal MC sample with each group 5G(rootfile) ..."
	       ./python/get_samples.py /scratchfs/bes/$USER/bes3ws2020/sigMC/psip/pipi_jpsi/dst run/samples/signal_MC_664p03_psip_pipi_jpsi.txt 5G
	       # made XX groups
	       ;;

    0.2.8) echo "Generate Condor jobs on signal MC(rootfile) ---- 1..." 
	       cd run/pipi_jpsi/gen_script
	       ./make_jobOption_file_sigMC_pipi_jpsi.sh
	       cd ../jobs_text/sigMC/jobs_sig
	       cp -r jobOptions_pipi_jpsi_signal_psip-1.txt jobOptions_pipi_jpsi_signal_psip-0.txt
           sed -i "s/ApplicationMgr\.EvtMax = -1/ApplicationMgr\.EvtMax = 50/g" jobOptions_pipi_jpsi_signal_psip-0.txt
	       ;;

    0.2.9) echo "Test for signal MC(rootfile)" 
           echo "have you changed test number?(yes / no)
           ./run/pipi_jpsi/job_text/sigMC/jobs_sig/jobOptions_pipi_jpsi_signal_psip-0.txt"
           read opt
           if [ $opt == "yes" ]
               then
               echo "now in yes"  
               cd run/pipi_jpsi/jobs_text/sigMC/jobs_sig
               boss.exe jobOptions_pipi_jpsi_signal_psip-0.txt
           else
               echo "Default value is 'no', please change test number."
           fi
           ;;

    0.2.10) echo "Submit Condor jobs on signal MC(rootfile) ---- 2..." 
	        cd run/pipi_jpsi/jobs_text/sigMC/jobs_sig
            find . -name "*.out.*" | xargs rm
            find . -name "*.err.*" | xargs rm
		    rm /scratchfs/bes/$USER/bes3ws2020/sigMC/psip/pipi_jpsi/rootfile/*.root
		    boss.condor -g physics -n 5 jobOptions_pipi_jpsi_signal_psip-%{ProcId}.txt
	        ;;

    0.2.11) echo "Synthesize signal MC root files..."
            cd /scratchfs/bes/$USER/bes3ws2020/sigMC/psip/pipi_jpsi/rootfile
            rm -rf sigMC_psip_pipi_jpsi.root
            hadd sigMC_psip_pipi_jpsi.root *.root
	        ;;

    0.2.12) echo "Apply cuts..."
            mkdir -p /besfs/users/$USER/bes3ws2020/sigMC/psip/pipi_jpsi
            ./python/apply_cuts.py /scratchfs/bes/$USER/bes3ws2020/sigMC/psip/pipi_jpsi/sigMC_psip_pipi_jpsi.root /besfs/users/$USER/bes3ws2020/sigMC/psip/pipi_jpsi/sigMC_psip_pipi_jpsi.root
	        ;;

esac

}
case $option in
    
    # --------------------------------------------------------------------------
    #  Data  
    # --------------------------------------------------------------------------

    0.1) echo "Running on data sample..."
         usage_0_1 
         echo "Please enter your option: " 
         read option  
         sub_0_1 option 
	     ;;

    0.1.*) echo "Running on data sample..."
           sub_0_1 option  
           ;;  
        
    # --------------------------------------------------------------------------
    #  Signal MC 
    # --------------------------------------------------------------------------

    0.2) echo "Generating and running on signal MC sample..."
         usage_0_2
         echo "Please enter your option: " 
         read option  
         sub_0_2 option 
	     ;;

    0.2.*) echo "Generating and running on signal MC sample..."
           sub_0_2 option  
           ;;  

esac
