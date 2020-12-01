#ifndef Physics_Analysis_pipi_jpsi_H
#define Physics_Analysis_pipi_jpsi_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
//#include "VertexFit/ReadBeamParFromDb.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h" 
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "VertexFit/WTrackParameter.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "VertexFit/SecondVertexFit.h"

class pipi_jpsi : public Algorithm {

public:
  pipi_jpsi(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  
  void corgen(HepMatrix &, HepVector &, int );
  void corset(HepSymMatrix &, HepMatrix &, int );
  void calibration(RecMdcKalTrack * , HepVector &, int );


private:
  int m_storetruth;

  double m_vr0cut;
  double m_vz0cut;

  //Declare energy, dphi, dthe cuts for fake gam's
  double m_energyThreshold;
  double m_gamPhiCut;
  double m_gamThetaCut;
  double m_gamAngleCut;

  // 
  int m_test4C;

  // 
  int m_checkDedx;
  int m_checkTof;
  double m_Ecms;

  // define Ntuples here

  NTuple::Tuple*  m_tuple1;      // charged track vertex
  NTuple::Item<double>  m_vx0;
  NTuple::Item<double>  m_vy0;
  NTuple::Item<double>  m_vz0;
  NTuple::Item<double>  m_vr0;
  NTuple::Item<double>  m_rvxy0;
  NTuple::Item<double>  m_rvz0;
  NTuple::Item<double>  m_rvphi0;
  NTuple::Item<double>  m_costhe;

  NTuple::Tuple*  m_tuple2;      // fake photon
  NTuple::Item<double>  m_dthe;
  NTuple::Item<double>  m_dphi;
  NTuple::Item<double>  m_dang;
  NTuple::Item<double>  m_eraw;

  NTuple::Tuple*  m_tuple3;     // rhopi: raw mgg, etot
  NTuple::Item<double>  m_m2gg;
  NTuple::Item<double>  m_etot;

  NTuple::Tuple*  m_tuple_charged;     //information of charged track 
  NTuple::Item<double>  E_EMC;
  NTuple::Item<double>  P_MDC;
  NTuple::Item<double>  E_P_ratio;
 
  NTuple::Tuple*  m_tuple_pipi_ll;     // pipi_ll 4C
  NTuple::Item<int> flag;
  NTuple::Item<double>  chi_4c_pipi_ll;
  NTuple::Item<double>  m_pipi_dang;
  NTuple::Item<double>  m_pimlp_dang;
  NTuple::Item<double>  m_piplm_dang;
  NTuple::Item<double>  m_lv_pionp;
  NTuple::Item<double>  m_lv_pionm;
  NTuple::Item<double>  m_lv_lepp;
  NTuple::Item<double>  m_lv_lepm;
  NTuple::Item<double>  m_recoil;
  NTuple::Item<double>  m_jpsi;
  NTuple::Item<double>  m_cms_lepm;
  NTuple::Item<double>  m_cms_lepp;
  NTuple::Array<double> p4_lp;
  NTuple::Array<double> p4_lm;
  NTuple::Array<double> p4_pip;
  NTuple::Array<double> p4_pim;
  // for sys error
  NTuple::Item<double> m_chi2_svf;
  NTuple::Item<double> m_ctau_svf;
  NTuple::Item<double> m_L_svf;
  NTuple::Item<double> m_Lerr_svf;

  NTuple::Tuple* m_tuple7;    // dE/dx
  NTuple::Item<double> m_ptrk;
  NTuple::Item<double> m_chie;
  NTuple::Item<double> m_chimu;
  NTuple::Item<double> m_chipi;
  NTuple::Item<double> m_chik;
  NTuple::Item<double> m_chip;
  NTuple::Item<double> m_probPH;
  NTuple::Item<double> m_normPH;
  NTuple::Item<double> m_ghit;
  NTuple::Item<double> m_thit;

  NTuple::Tuple* m_tuple8;   // endcap tof
  NTuple::Item<double> m_ptot_etof;
  NTuple::Item<double> m_cntr_etof;
  NTuple::Item<double> m_te_etof;
  NTuple::Item<double> m_tmu_etof;
  NTuple::Item<double> m_tpi_etof;
  NTuple::Item<double> m_tk_etof;
  NTuple::Item<double> m_tp_etof;
  NTuple::Item<double> m_ph_etof;
  NTuple::Item<double> m_rhit_etof;
  NTuple::Item<double> m_qual_etof;

  NTuple::Tuple* m_tuple9;  // barrel inner tof
  NTuple::Item<double> m_ptot_btof1;
  NTuple::Item<double> m_cntr_btof1;
  NTuple::Item<double> m_te_btof1;
  NTuple::Item<double> m_tmu_btof1;
  NTuple::Item<double> m_tpi_btof1;
  NTuple::Item<double> m_tk_btof1;
  NTuple::Item<double> m_tp_btof1;
  NTuple::Item<double> m_ph_btof1;
  NTuple::Item<double> m_zhit_btof1;
  NTuple::Item<double> m_qual_btof1;

  NTuple::Tuple* m_tuple10;  // barrel outer tof
  NTuple::Item<double> m_ptot_btof2;
  NTuple::Item<double> m_cntr_btof2;
  NTuple::Item<double> m_te_btof2;
  NTuple::Item<double> m_tmu_btof2;
  NTuple::Item<double> m_tpi_btof2;
  NTuple::Item<double> m_tk_btof2;
  NTuple::Item<double> m_tp_btof2;
  NTuple::Item<double> m_ph_btof2;
  NTuple::Item<double> m_zhit_btof2;
  NTuple::Item<double> m_qual_btof2;

  // for McTruth Info
  NTuple::Tuple* m_tuple11;
  int idxmc;
  int pdgid[100];
  int motheridx[100];
  NTuple::Item<int> m_runNo;
  NTuple::Item<int> m_evtNo;
  NTuple::Item<int> m_idxmc;
  NTuple::Array<int> m_pdgid;
  NTuple::Array<int> m_motheridx;
  bool stat_fitSecondVertex;
  bool fitSecondVertex(WTrackParameter &Dpiplus, WTrackParameter &Dpiminus);
};

#endif 
