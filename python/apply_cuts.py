#!/usr/bin/env python
"""
Apply cuts on root files
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2020-01-07 Tue 23:30]"

import math
from array import array
import ROOT
from ROOT import TCanvas, gStyle, TLorentzVector, TTree
from ROOT import TFile, TH1F, TLegend, TArrow, TChain, TVector3
import sys, os
import logging
from math import *
from tools import *
logging.basicConfig(level=logging.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')

def usage():
    sys.stdout.write('''
NAME
    apply_cuts.py

SYNOPSIS
    ./apply_cuts.py [file_in] [file_out]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    January 2020
\n''')

def apply(file_in, file_out):
    try:
        chain = TChain('track')
        chain.Add(file_in)
    except:
        logging.error(file_in + ' is invalid!')
        sys.exit()

    cut = ''
    cut_dang = '(abs(m_pipi_dang)<0.98&&(flag==1||(flag==-1&&(abs(m_pimlp_dang)<0.98&&abs(m_piplm_dang)<0.98)))) && '
    cut_chi2 = '(chi2_pipi_ll<60) && '
    cut_m_jpsi =  '(m_jpsi>3.08&&m_jpsi<3.12)'
    cut = cut_dang + cut_chi2 + cut_m_jpsi

    t = chain.CopyTree(cut)
    t.SaveAs(file_out)

def main():
    args = sys.argv[1:]
    if len(args)<2:
        return usage()
    file_in = args[0]
    file_out = args[1]

    print '--> Begin to process file: ' + file_in
    apply(file_in, file_out)
    print '--> End of processing file: ' + file_in

if __name__ == '__main__':
    main()
