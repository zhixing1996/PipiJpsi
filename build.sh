#!/usr/bin/env bash

# Main driver to build programs
# Author SHI Xin <shixin@ihep.ac.cn>
# Modified by JING Maoqiang <jingmq@ihep.ac.cn>
# Created [2019-12-11 Dec 14:50]


usage() {
    printf "NAME\n\tbuild.sh - Main driver to build programs\n"
    printf "\nSYNOPSIS\n"
    printf "\n\t%-5s\n" "./build.sh [OPTION]" 
    printf "\nOPTIONS\n" 
    printf "\n\t%-9s  %-40s"  "0.0"      "[set up useful tools]"
    printf "\n\t%-9s  %-40s"  "0.1"      "[build PipiJpsi analyzer]"
    printf "\n\n" 
}

usage_0_0() {
    printf "\n\t%-9s  %-40s"  ""         ""   
    printf "\n\t%-9s  %-40s"  "0.0.1"    "Set up topology analysis tool ---- 1"
    printf "\n\t%-9s  %-40s"  "0.0.2"    "Set up topology analysis tool ---- 2"
    printf "\n\t%-9s  %-40s"  ""          ""
    printf "\n"
}

usage_0_1() {
    printf "\n\t%-9s  %-40s"  ""         ""   
    printf "\n\t%-9s  %-40s"  "0.1.1"    "Build PIPIJPSIALGROOT module(24)"
    printf "\n\t%-9s  %-40s"  ""          ""
    printf "\n"
}

if [[ $# -eq 0 ]]; then
    usage
    echo "Please enter your option: "
    read option
else
    option=$1    
fi

sub_0_0() {

case $option in
    
    # --------------------------------------------------------------------------
    #  Useful tools 
    # --------------------------------------------------------------------------

    0.0.1) echo "Set up topology analysis tool ----1..."
           git clone git@github.com:buaazhouxingyu/topoana.git ./topoana
           cd topoana
           ./Configure
           echo "##########Please pay attention to the printed info##########"
	       ;;

    0.0.2) echo "Set up topology analysis tool ----2..."
           cd topoana
           make
           ./Setup Example
	       ;;

esac

}

sub_0_1() {

case $option in
    
    # --------------------------------------------------------------------------
    #  PIPI_JPSIALGROOT module
    # --------------------------------------------------------------------------

    0.1.1) echo "Building PIPIJPSIALGROOT module(24) ..."
           rm -rf ./Analysis/Physics/pipi_jpsiAlg/pipi_jpsiAlg-00-00-24/x86_*
           cd ./Analysis/Physics/pipi_jpsiAlg/pipi_jpsiAlg-00-00-24/cmt
           cmt config
           gmake  
	       ;;

esac

}

case $option in
    
    # --------------------------------------------------------------------------
    #  Useful tools 
    # --------------------------------------------------------------------------

    0.0) echo "Setting up useful tools..."
         usage_0_0
         echo "Please enter your option: " 
         read option  
         sub_0_0 option 
	     ;;

    0.0.*) echo "Setting up useful tools..."
           sub_0_0 option  
           ;;  

    # --------------------------------------------------------------------------
    #  PIPI_JPSIALGROOT module 
    # --------------------------------------------------------------------------

    0.1) echo "Building PIPIJPSIALGROOT module..."
         usage_0_1 
         echo "Please enter your option: " 
         read option  
         sub_0_1 option 
	     ;;

    0.1.*) echo "Building PIPIJPSIALGROOT module..."
           sub_0_1 option  
           ;;  
        
esac
