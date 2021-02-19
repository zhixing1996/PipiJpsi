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
    printf "\n\t%-9s  %-40s"  "0.2"       "[generate and run on signal MC sample]"
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
    printf "\n\t%-9s  %-40s"  ""           ""
    printf "\n"
}

usage_0_2() {
    printf "\n\t%-9s  %-40s"  ""          ""
    printf "\n\t%-9s  %-40s"  "0.2.1"     "Generate MC samples ---- Simulation && Reconstruction"
    printf "\n\t%-9s  %-40s"  "0.2.2"     "Generate MC samples ---- Event Selection"
    printf "\n\t%-9s  %-40s"  "0.2.3"     "Synthesize MC sample root files"
    printf "\n\t%-9s  %-40s"  "0.2.4"     "Apply cuts"
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
           sed -i "s/ApplicationMgr\.EvtMax = -1/ApplicationMgr\.EvtMax = 20000/g" jobOptions_pipi_jpsi_data_4260-0.txt
           sed -i "s/pipi_jpsi_data_4260-1\.root/pipi_jpsi_data_4260-0\.root/g" jobOptions_pipi_jpsi_data_4260-0.txt
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
           cd ./run/pipi_jpsi/gen_script
           ./syn_Data_pipi_jpsi.sh 703
	       ;;

    0.1.6) echo "Apply cuts ..."
           cd ./run/pipi_jpsi/gen_script
           ./apply_cuts_Data_pipi_jpsi.sh 703
           mv Apply_Cuts_Data_703 ../logfile
           cd ../logfile
           chmod u+x Apply_Cuts_Data_703
           bash Apply_Cuts_Data_703
           rm -r Apply_Cuts_Data_703
           cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi
	       ;;

esac

}

sub_0_2() {

case $option in
    
    # --------------------------------------------------------------------------
    #  Signal MC  
    # --------------------------------------------------------------------------

    0.2.1) echo "Generate MC samples ---- Simulation && Reconstruction ..."
           echo "which MC sample do you want to simulate?"
           echo "    JPIPI       --> e+e- --> pi-pi-J/psi"
           read opt
           if [ $opt == "JPIPI" ]; then
               cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/gen_script/gen_mc/JPIPI
               ./sub_jobOption_PI_PI_JPSI.sh 703
           fi
	       ;;

    0.2.2) echo "Generate MC samples ---- Event Selection ..."
           echo "which MC sample do you want to select?"
           echo "    JPIPI       --> e+e- --> pi-pi-J/psi"
           read opt
           if [ $opt == "JPIPI" ]; then
               cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi/run/pipi_jpsi/gen_script/gen_mc/JPIPI
               ./sub_Ana_PI_PI_JPSI.sh 703
           fi
	       ;;

    0.2.3) echo "Synthesize inclusive MC samples root files ..."
           cd ./run/pipi_jpsi/gen_script
           echo "which MC sample do you want to synthesize?"
           echo "    JPIPI       --> e+e- --> pi-pi-J/psi"
           read opt
           if [ $opt == "JPIPI" ]; then
               ./syn_MC_pipi_jpsi.sh 703 JPIPI
           else
               echo "Please add the MC simulation joboption files!"
           fi
	       ;;

    0.2.4) echo "Apply cuts ..."
           cd ./run/pipi_jpsi/gen_script
           echo "which MC sample do you want to synthesize?"
           echo "    JPIPI       --> e+e- --> pi-pi-J/psi"
           read opt
           if [ $opt == "JPIPI" ]; then
               ./apply_cuts_MC_pipi_jpsi.sh 703 JPIPI
               mv Apply_Cuts_JPIPI_703 ../logfile
               cd ../logfile
               chmod u+x Apply_Cuts_JPIPI_703
               bash Apply_Cuts_JPIPI_703
               rm -r Apply_Cuts_JPIPI_703
               cd /besfs/groups/cal/dedx/$USER/bes/PipiJpsi
           else
               echo "Please add the MC simulation joboption files!"
           fi
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
