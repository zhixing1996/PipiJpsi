#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "VertexFit/IVertexDbSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "McTruth/McParticle.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"

#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "pipi_jpsiAlg/pipi_jpsi.h"
#include "TRandom.h"
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/Helix.h"
#include "ParticleID/ParticleID.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include <vector>

const double mpi = 0.13957;
const double me = 0.000511;
const double mmu = 0.105658;
const double meta = 0.547853;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

int Ncut0=0,Ncut1=0,Ncut2=0,Ncut3=0,Ncut4=0,Ncut5=0,Ncut6=0;

bool   debug_key     =  false;
bool   read_vertex   =  true;

/////////////////////////////////////////////////////////////////////////////

pipi_jpsi::pipi_jpsi(const std::string& name, ISvcLocator* pSvcLocator) :
	Algorithm(name, pSvcLocator) 
{
	//Declare the properties  
	declareProperty("Vr0cut", m_vr0cut=1.0);
	declareProperty("Vz0cut", m_vz0cut=5.0);
	declareProperty("GammaPhiCut", m_gamPhiCut=20.0);
	declareProperty("GammaThetaCut", m_gamThetaCut=20.0);
	declareProperty("GammaAngleCut", m_gamAngleCut=20.0);
	//		declareProperty("Test4C", m_test4C = 1);
	declareProperty("CheckDedx", m_checkDedx = 1);
	declareProperty("CheckTof",  m_checkTof = 1);
	declareProperty("MCTruthStore", m_storetruth = 0);//*top
	declareProperty("Ecms", m_Ecms = 3.686);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode pipi_jpsi::initialize()
{
	MsgStream log(msgSvc(), name());

	log << MSG::INFO << "in initialize()" << endmsg;

	StatusCode status;
	NTuplePtr nt1(ntupleSvc(), "FILE1/vxyz");
	if ( nt1 ) m_tuple1 = nt1;
	else 
	{
		m_tuple1 = ntupleSvc()->book ("FILE1/vxyz", CLID_ColumnWiseTuple, "ks N-Tuple example");
		if ( m_tuple1 )    
		{
			status = m_tuple1->addItem ("vx0",   m_vx0);
			status = m_tuple1->addItem ("vy0",   m_vy0);
			status = m_tuple1->addItem ("vz0",   m_vz0);
			status = m_tuple1->addItem ("vr0",   m_vr0);
			status = m_tuple1->addItem ("rvxy0",  m_rvxy0);
			status = m_tuple1->addItem ("rvz0",   m_rvz0);
			status = m_tuple1->addItem ("rvphi0", m_rvphi0);
			status = m_tuple1->addItem ("costhe", m_costhe);
		}
		else    
		{ 
			log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
			return StatusCode::FAILURE;
		}
	}

	NTuplePtr nt2(ntupleSvc(), "FILE1/photon");
	if ( nt2 ) m_tuple2 = nt2;
	else 
	{
		m_tuple2 = ntupleSvc()->book ("FILE1/photon", CLID_ColumnWiseTuple, "ks N-Tuple example");
		if ( m_tuple2 )    
		{
			status = m_tuple2->addItem ("dthe",   m_dthe);
			status = m_tuple2->addItem ("dphi",   m_dphi);
			status = m_tuple2->addItem ("dang",   m_dang);
			status = m_tuple2->addItem ("eraw",   m_eraw);
		}
		else    
		{ 
			log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple2) << endmsg;
			return StatusCode::FAILURE;
		}
	}


	NTuplePtr nt3(ntupleSvc(), "FILE1/etot");
	if ( nt3 ) m_tuple3 = nt3;
	else 
	{
		m_tuple3 = ntupleSvc()->book ("FILE1/etot", CLID_ColumnWiseTuple, "ks N-Tuple example");
		if ( m_tuple3 )    
		{
			status = m_tuple3->addItem ("m2gg",   m_m2gg);
			status = m_tuple3->addItem ("etot",   m_etot);
		}
		else    
		{ 
			log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple3) << endmsg;
			return StatusCode::FAILURE;
		}
	}

	NTuplePtr nt_charged(ntupleSvc(), "FILE1/Charged");
	if ( nt_charged ) m_tuple_charged = nt_charged;
	else {
		m_tuple_charged = ntupleSvc()->book ("FILE1/Charged", CLID_ColumnWiseTuple, "ks N-Tuple example");
		if ( m_tuple_charged )    {
			status = m_tuple_charged->addItem ("E_P_ratio",   E_P_ratio);
			status = m_tuple_charged->addItem ("E_EMC",       E_EMC);
			status = m_tuple_charged->addItem ("P_MDC",       P_MDC);
		}
		else    { 
			log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple_charged) << endmsg;
			return StatusCode::FAILURE;
		}
	}


	NTuplePtr nt_pipi_ll(ntupleSvc(), "FILE1/track");
	if ( nt_pipi_ll ) m_tuple_pipi_ll = nt_pipi_ll;
	else 
	{
		m_tuple_pipi_ll = ntupleSvc()->book ("FILE1/track", CLID_ColumnWiseTuple, "ks N-Tuple example");
		if ( m_tuple_pipi_ll )
		{
			status = m_tuple_pipi_ll->addItem ("flag",  flag);
			status = m_tuple_pipi_ll->addItem ("chi2_pipi_ll",  chi_4c_pipi_ll);
			status = m_tuple_pipi_ll->addItem ("m_pipi_dang",   m_pipi_dang);
			status = m_tuple_pipi_ll->addItem ("m_pimlp_dang",  m_pimlp_dang);
			status = m_tuple_pipi_ll->addItem ("m_piplm_dang",  m_piplm_dang);
			status = m_tuple_pipi_ll->addItem ("m_lv_pionp",    m_lv_pionp);
			status = m_tuple_pipi_ll->addItem ("m_lv_pionm",    m_lv_pionm);
			status = m_tuple_pipi_ll->addItem ("m_lv_lepp",     m_lv_lepp);
			status = m_tuple_pipi_ll->addItem ("m_lv_lepm",     m_lv_lepm);
			status = m_tuple_pipi_ll->addItem ("m_recoil",      m_recoil);
			status = m_tuple_pipi_ll->addItem ("m_jpsi",        m_jpsi);
			status = m_tuple_pipi_ll->addItem ("m_cms_lepm",    m_cms_lepm);
			status = m_tuple_pipi_ll->addItem ("m_cms_lepp",    m_cms_lepp);
			status = m_tuple_pipi_ll->addItem ("p4_lp",  4,  p4_lp);
			status = m_tuple_pipi_ll->addItem ("p4_lm",  4,  p4_lm);
			status = m_tuple_pipi_ll->addItem ("p4_pip", 4,  p4_pip);
			status = m_tuple_pipi_ll->addItem ("p4_pim", 4,  p4_pim);
			status = m_tuple_pipi_ll->addItem ("m_chi2_svf",      m_chi2_svf);
			status = m_tuple_pipi_ll->addItem ("m_L_svf",         m_L_svf);
			status = m_tuple_pipi_ll->addItem ("m_Lerr_svf",      m_Lerr_svf);
			status = m_tuple_pipi_ll->addItem ("m_ctau_svf",      m_ctau_svf);
			status = m_tuple_pipi_ll->addItem ("m_Vr_pionp",    m_Vr_pionp);
			status = m_tuple_pipi_ll->addItem ("m_Vr_pionm",    m_Vr_pionm);
			status = m_tuple_pipi_ll->addItem ("m_Vr_lepp",     m_Vr_lepp);
			status = m_tuple_pipi_ll->addItem ("m_Vr_lepm",     m_Vr_lepm);
			status = m_tuple_pipi_ll->addItem ("m_Vz_pionp",    m_Vz_pionp);
			status = m_tuple_pipi_ll->addItem ("m_Vz_pionm",    m_Vz_pionm);
			status = m_tuple_pipi_ll->addItem ("m_Vz_lepp",     m_Vz_lepp);
			status = m_tuple_pipi_ll->addItem ("m_Vz_lepm",     m_Vz_lepm);
			status = m_tuple_pipi_ll->addItem ("m_cos_pionp",    m_cos_pionp);
			status = m_tuple_pipi_ll->addItem ("m_cos_pionm",    m_cos_pionm);
			status = m_tuple_pipi_ll->addItem ("m_cos_lepp",     m_cos_lepp);
			status = m_tuple_pipi_ll->addItem ("m_cos_lepm",     m_cos_lepm);
		}
		else    
		{ 
			log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple_pipi_ll) << endmsg;
			return StatusCode::FAILURE;
		}
	}


	if(m_checkDedx == 1) {
		NTuplePtr nt7(ntupleSvc(), "FILE1/dedx");
		if ( nt7 ) m_tuple7 = nt7;
		else {
			m_tuple7 = ntupleSvc()->book ("FILE1/dedx", CLID_ColumnWiseTuple, "ks N-Tuple example");
			if ( m_tuple7 )    {
				status = m_tuple7->addItem ("ptrk",   m_ptrk);
				status = m_tuple7->addItem ("chie",   m_chie);
				status = m_tuple7->addItem ("chimu",   m_chimu);
				status = m_tuple7->addItem ("chipi",   m_chipi);
				status = m_tuple7->addItem ("chik",   m_chik);
				status = m_tuple7->addItem ("chip",   m_chip);
				status = m_tuple7->addItem ("probPH",   m_probPH);
				status = m_tuple7->addItem ("normPH",   m_normPH);
				status = m_tuple7->addItem ("ghit",   m_ghit);
				status = m_tuple7->addItem ("thit",   m_thit);
			}
			else    { 
				log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple7) << endmsg;
				return StatusCode::FAILURE;
			}
		}
	} // check dE/dx

	if(m_checkTof == 1) {
		NTuplePtr nt8(ntupleSvc(), "FILE1/tofe");
		if ( nt8 ) m_tuple8 = nt8;
		else {
			m_tuple8 = ntupleSvc()->book ("FILE1/tofe",CLID_ColumnWiseTuple, "ks N-Tuple example");
			if ( m_tuple8 )    {
				status = m_tuple8->addItem ("ptrk",   m_ptot_etof);
				status = m_tuple8->addItem ("cntr",   m_cntr_etof);
				status = m_tuple8->addItem ("ph",  m_ph_etof);
				status = m_tuple8->addItem ("rhit", m_rhit_etof);
				status = m_tuple8->addItem ("qual", m_qual_etof);
				status = m_tuple8->addItem ("te",   m_te_etof);
				status = m_tuple8->addItem ("tmu",   m_tmu_etof);
				status = m_tuple8->addItem ("tpi",   m_tpi_etof);
				status = m_tuple8->addItem ("tk",   m_tk_etof);
				status = m_tuple8->addItem ("tp",   m_tp_etof);
			}
			else    { 
				log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple8) << endmsg;
				return StatusCode::FAILURE;
			}
		}
	} // check Tof:endcap

	if(m_checkTof == 1) {
		NTuplePtr nt9(ntupleSvc(), "FILE1/tof1");
		if ( nt9 ) m_tuple9 = nt9;
		else {
			m_tuple9 = ntupleSvc()->book ("FILE1/tof1", CLID_ColumnWiseTuple, "ks N-Tuple example");
			if ( m_tuple9 )    {
				status = m_tuple9->addItem ("ptrk",   m_ptot_btof1);
				status = m_tuple9->addItem ("cntr",   m_cntr_btof1);
				status = m_tuple9->addItem ("ph",  m_ph_btof1);
				status = m_tuple9->addItem ("zhit", m_zhit_btof1);
				status = m_tuple9->addItem ("qual", m_qual_btof1);
				status = m_tuple9->addItem ("te",   m_te_btof1);
				status = m_tuple9->addItem ("tmu",   m_tmu_btof1);
				status = m_tuple9->addItem ("tpi",   m_tpi_btof1);
				status = m_tuple9->addItem ("tk",   m_tk_btof1);
				status = m_tuple9->addItem ("tp",   m_tp_btof1);
			}
			else    { 
				log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple9) << endmsg;
				return StatusCode::FAILURE;
			}
		}
	} // check Tof:barrel inner Tof 


	if(m_checkTof == 1) {
		NTuplePtr nt10(ntupleSvc(), "FILE1/tof2");
		if ( nt10 ) m_tuple10 = nt10;
		else {
			m_tuple10 = ntupleSvc()->book ("FILE1/tof2", CLID_ColumnWiseTuple, "ks N-Tuple example");
			if ( m_tuple10 )    {
				status = m_tuple10->addItem ("ptrk",   m_ptot_btof2);
				status = m_tuple10->addItem ("cntr",   m_cntr_btof2);
				status = m_tuple10->addItem ("ph",  m_ph_btof2);
				status = m_tuple10->addItem ("zhit", m_zhit_btof2);
				status = m_tuple10->addItem ("qual", m_qual_btof2);
				status = m_tuple10->addItem ("te",   m_te_btof2);
				status = m_tuple10->addItem ("tmu",   m_tmu_btof2);
				status = m_tuple10->addItem ("tpi",   m_tpi_btof2);
				status = m_tuple10->addItem ("tk",   m_tk_btof2);
				status = m_tuple10->addItem ("tp",   m_tp_btof2);
			}
			else    { 
				log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple10) << endmsg;
				return StatusCode::FAILURE;
			}
		}
	} // check Tof:barrel outter Tof

    // for mc truth info
	NTuplePtr nt11(ntupleSvc(), "FILE1/truth");
	if ( nt11 ) m_tuple11 = nt11;
	else 
	{
		m_tuple11 = ntupleSvc()->book ("FILE1/truth", CLID_ColumnWiseTuple, "ks N-Tuple example");
		if ( m_tuple11 )    
		{
            status = m_tuple11->addItem("run", m_runNo);
            status = m_tuple11->addItem("evt", m_evtNo);
            status = m_tuple11->addItem("indexmc", m_idxmc, 0, 100);
            status = m_tuple11->addIndexedItem("pdgid", m_idxmc, m_pdgid);
            status = m_tuple11->addIndexedItem("motheridx", m_idxmc, m_motheridx);
		}
		else    
		{ 
			log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple11) << endmsg;
			return StatusCode::FAILURE;
		}
	}

	//
	//--------end of book--------
	//

	log << MSG::INFO << "successfully return from initialize()" <<endmsg;
	return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode pipi_jpsi::execute()
{
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in execute()" << endreq;

	SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
	m_runNo = eventHeader->runNumber();
	m_evtNo = eventHeader->eventNumber();
	log << MSG::DEBUG <<"run, evtnum = "
		<< m_runNo << " , "
		<< m_evtNo <<endreq;
	Ncut0++;

	SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
	log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
		<< evtRecEvent->totalCharged() << " , "
		<< evtRecEvent->totalNeutral() << " , "
		<< evtRecEvent->totalTracks() <<endreq;

	SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);

    // save all MCTruth
    // Xingyu's older method
    if (m_runNo < 0) {
        SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), "/Event/MC/McParticleCol");
        if (!mcParticleCol) {
            std::cout << "Could not retreive McParticleCol" << std::endl;
        }
        else {
            Event::McParticleCol::iterator iter_mc = mcParticleCol->begin(); // loop all the particles in the decay chain(MCTruth)
            int pid = (*iter_mc)->particleProperty();
            unsigned int idx;
            unsigned int midx;
            idxmc = 0;
            // const int incPid=443; // 443 is the PDG code of J/psi
            // const int incPid=100443; // 100443 is the PDG code of psi(2S)
            // const int incPid=30443; // 100443 is the PDG code of psi(3770)
            const int incPid=9030443; // 9030443 is the PDG code of psi(4260)
            // const int incPid=92; // 92 is the PDG code of string
            // const int incPid_1=90022; // 90022 is the PDG code of gamma*
            // const int incPid_2=80022; // 80022 is the PDG code of gamma*
            // if (pid == 90022 || pid == 80022) {
            if (incPid) {
                 for (iter_mc++; iter_mc != mcParticleCol->end(); iter_mc++) {
                     if (!(*iter_mc)->decayFromGenerator()) continue;
                     pid = (*iter_mc)->particleProperty();
                     idx = (*iter_mc)->trackIndex();
                     midx = ((*iter_mc)->mother()).trackIndex();
                     pdgid[idxmc] = pid;
                     if (idx == midx || midx == 0) motheridx[idxmc] = idx - 1;
                     else motheridx[idxmc] = midx - 1;
                     idxmc++;
                 }
            }
            else {
                for (; iter_mc != mcParticleCol->end(); iter_mc++) {
                    if (!(*iter_mc)->decayFromGenerator()) continue;
                    pdgid[idxmc] = (*iter_mc)->particleProperty();
                    motheridx[idxmc]= ((*iter_mc)->mother()).trackIndex();
                    idxmc++;
                }
            }
        }
    }

    // Xingyu's newest method
    // if (m_runNo < 0) {
    //     SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
    //     if(!mcParticleCol) {
    //         std::cout<<"Could not retrieve McParticelCol"<<std::endl;
    //         return(StatusCode::FAILURE);  
    //     }
    //     Event::McParticleCol::iterator iter_mc=mcParticleCol->begin();
    //     idxmc=0;
    //     // const int incPid=443; // 443 is the PDG code of J/psi
    //     // const int incPid=100443; // 100443 is the PDG code of psi(2S)
    //     // const int incPid=30443; // 100443 is the PDG code of psi(3770)
    //     const int incPid=9030443; // 9030443 is the PDG code of psi(4260)
    //     // const int incPid=92; // 92 is the PDG code of string
    //     bool incPdcy(false);
    //     int rootIndex(-1);
    //     for (;iter_mc!=mcParticleCol->end();iter_mc++) {
    //         //std::cout<<"idx="<<idxmc<<"\t"<<"pid="<<(*iter_mc)->particleProperty()<<"\t"<<"midx="<<((*iter_mc)->mother()).trackIndex()<<std::endl;
    //         if ((*iter_mc)->primaryParticle()) continue;
    //         if (!(*iter_mc)->decayFromGenerator()) continue;
    //         if ((*iter_mc)->particleProperty()==incPid) {
    //             incPdcy=true;
    //             rootIndex=(*iter_mc)->trackIndex();
    //         }
    //         if (!incPdcy) continue;
    //         pdgid[idxmc]=(*iter_mc)->particleProperty();
    //         if ((*iter_mc)->particleProperty()==incPid||((*iter_mc)->mother()).particleProperty()==incPid)
    //             motheridx[idxmc]=((*iter_mc)->mother()).trackIndex()-rootIndex;
    //         else 
    //             motheridx[idxmc]=((*iter_mc)->mother()).trackIndex()-rootIndex-1;
    //         idxmc++;
    //     }
    // }

	HepLorentzVector ecms(m_Ecms*0.011, 0, 0, m_Ecms);

	//
	// check x0, y0, z0, r0
	// suggest cut: |z0|<5 && r0<1
	//

	Vint iGood;
	iGood.clear();

	int nCharge = 0;

	Hep3Vector xorigin(0,0,0);

	if(read_vertex)
	{
		IVertexDbSvc*  vtxsvc;
		Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
		if(vtxsvc->isVertexValid())
		{
			double* dbv = vtxsvc->PrimaryVertex(); 
			double*  vv = vtxsvc->SigmaPrimaryVertex();  
			xorigin.setX(dbv[0]);
			xorigin.setY(dbv[1]);
			xorigin.setZ(dbv[2]);
		}
	}

	for(int i = 0; i < evtRecEvent->totalCharged(); i++){
		EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
		if(!(*itTrk)->isMdcTrackValid()) continue;
		RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
		double pch=mdcTrk->p();
		double x0=mdcTrk->x();
		double y0=mdcTrk->y();
		double z0=mdcTrk->z();
		double phi0=mdcTrk->helix(1);
		double xv=xorigin.x();
		double yv=xorigin.y();
		double Rxy=(x0-xv)*cos(phi0)+(y0-yv)*sin(phi0);
		m_vx0 = x0;
		m_vy0 = y0;
		m_vz0 = z0;
		m_vr0 = Rxy;

		HepVector a = mdcTrk->helix();
		HepSymMatrix Ea = mdcTrk->err();
		HepPoint3D point0(0.,0.,0.);   // the initial point for MDC recosntruction
		HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]); 
		VFHelix helixip(point0,a,Ea); 
		helixip.pivot(IP);
		HepVector vecipa = helixip.a();
		double  Rvxy0=fabs(vecipa[0]);  //the nearest distance to IP in xy plane
		double  Rvz0=vecipa[3];         //the nearest distance to IP in z direction
		double  Rvphi0=vecipa[1];
		double  costheta0   = cos(mdcTrk -> theta());
		m_rvxy0=Rvxy0;
		m_rvz0=Rvz0;
		m_rvphi0=Rvphi0;
		m_costhe=costheta0;

		if ( m_evtNo % 1000 == 0 ) m_tuple1->write();
		if (costheta0  >  0.93 || costheta0 < -0.93) continue;

		if(fabs(Rvz0) >= 10.0) continue;
		if(fabs(Rvxy0) >= 1.0) continue;

		iGood.push_back(i);
		nCharge += mdcTrk->charge();
	}

	//
	// Finish Good Charged Track Selection
	//
	int nGood = iGood.size();
	if(debug_key)	cout<< "ngood, totcharge = " << nGood << " , " << nCharge << endl;

	if((nGood != 4)||(nCharge!=0))
	{
		return StatusCode::SUCCESS;
	}

	Ncut1++;

	//Isolate photon selection from guoaq

	//*************good photons selection****************************
	double eraw0=-1;
	double gtime0 = 100;
	Vint iGam;
	iGam.clear();
	Vint FSR_gam, FSR_charge;
	FSR_gam.clear();
	FSR_charge.clear();

	for (int i = evtRecEvent -> totalCharged();i < evtRecEvent -> totalTracks(); i++)
	{
		EvtRecTrackIterator itTrk = evtRecTrkCol -> begin() + i;
		if (!(*itTrk) -> isEmcShowerValid()) continue;
		RecEmcShower *emcTrk = (*itTrk) -> emcShower();
		if(i==0)
		{
			eraw0 = emcTrk->energy();
			gtime0 = emcTrk->time();
		}
		Hep3Vector emcpos(emcTrk -> x(), emcTrk -> y(), emcTrk -> z());
		double m_TDC_guo=emcTrk->time();
		double the  = emcTrk -> theta();
		double costhe_guo=cos(the);
		double dthe = 200.;
		double dphi = 200.;
		double dang = 200.; 
		int charge_temp=-1;
		for (int j = 0; j < evtRecEvent -> totalCharged(); j++)
		{
			EvtRecTrackIterator jtTrk = evtRecTrkCol -> begin() + j;

			if (!(*jtTrk) -> isExtTrackValid()) continue;

			RecExtTrack *extTrk = (*jtTrk) -> extTrack();

			if (extTrk -> emcVolumeNumber() == -1) continue;

			Hep3Vector extpos = extTrk -> emcPosition();

			double angd = extpos.angle(emcpos);
			double thed = extpos.theta() - emcpos.theta();
			double phid = extpos.deltaPhi(emcpos);

			thed = fmod(thed + CLHEP::twopi + CLHEP::twopi + pi,
					CLHEP::twopi) - CLHEP::pi;

			phid = fmod(phid + CLHEP::twopi + CLHEP::twopi + pi,
					CLHEP::twopi) - CLHEP::pi;

			if (fabs(thed) < fabs(dthe)) dthe = thed;
			if (fabs(phid) < fabs(dphi)) dphi = phid;
			if (angd < dang) {dang = angd;charge_temp=j;}
		}

		double eraw = emcTrk -> energy();
		int st=emcTrk->status();

		dthe = dthe * 180 / (CLHEP::pi);
		dphi = dphi * 180 / (CLHEP::pi);
		dang = dang * 180 / (CLHEP::pi);
		m_dthe    = dthe;
		m_dphi    = dphi;
		m_dang    = dang;
		m_eraw    = eraw;
		if ( m_evtNo % 1000 == 0 ) m_tuple2 -> write();

		if(dang >= 200) continue;
		if(fabs(dang)<5) {FSR_gam.push_back(i);FSR_charge.push_back(charge_temp);}
		if(fabs(dang) < 20 ) continue;

		if(evtRecEvent->totalCharged()>0)
		{
			if(m_TDC_guo < 0||m_TDC_guo>14) continue;
		}
		if(evtRecEvent->totalCharged()==0)
		{
			if(m_TDC_guo-gtime0<-10||m_TDC_guo-gtime0>10) continue;
		}
		if(fabs(costhe_guo)<0.8&&(eraw > 0.025 )) iGam.push_back(i);
		if(fabs(costhe_guo)>0.86&&fabs(costhe_guo)<0.92&&(eraw > 0.05 )) iGam.push_back(i);
	}

	//******************* Finish Good Photon Selection********************************

	//Isolate photon selection from guoaq

	int nGam = iGam.size();
	int nFSRGam = FSR_gam.size();

	log << MSG::DEBUG << "num Good Photon " << nGam  << " , " <<evtRecEvent->totalNeutral()<<endreq;
	if(nGam>10||nFSRGam>4){
		return StatusCode::SUCCESS;
	}
	Ncut2++;

	//
	// check dedx infomation
	//

	if(m_checkDedx == 1) {
		for(int i = 0; i < nGood; i++) {
			EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];
			if(!(*itTrk)->isMdcTrackValid()) continue;
			if(!(*itTrk)->isMdcDedxValid())continue;
			RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
			RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();
			m_ptrk = mdcTrk->p();

			m_chie = dedxTrk->chiE();
			m_chimu = dedxTrk->chiMu();
			m_chipi = dedxTrk->chiPi();
			m_chik = dedxTrk->chiK();
			m_chip = dedxTrk->chiP();
			m_ghit = dedxTrk->numGoodHits();
			m_thit = dedxTrk->numTotalHits();
			m_probPH = dedxTrk->probPH();
			m_normPH = dedxTrk->normPH();
		}
		m_tuple7->write();
	}

	//
	// check TOF infomation
	//

	if(m_checkTof == 1) {
		for(int i = 0; i < nGood; i++) {
			EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];
			if(!(*itTrk)->isMdcTrackValid()) continue;
			if(!(*itTrk)->isTofTrackValid()) continue;

			RecMdcTrack * mdcTrk = (*itTrk)->mdcTrack();
			SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();

			double ptrk = mdcTrk->p();

			SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();
			for(;iter_tof != tofTrkCol.end(); iter_tof++ ) { 
				TofHitStatus *status = new TofHitStatus; 
				status->setStatus((*iter_tof)->status());
				if(!(status->is_barrel())){//endcap
					if( !(status->is_counter()) ) continue; // ? 
					if( status->layer()!=0 ) continue;//layer1
					double path=(*iter_tof)->path(); // ? 
					double tof  = (*iter_tof)->tof();
					double ph   = (*iter_tof)->ph();
					double rhit = (*iter_tof)->zrhit();
					double qual = 0.0 + (*iter_tof)->quality();
					double cntr = 0.0 + (*iter_tof)->tofID();
					double texp[5];
					for(int j = 0; j < 5; j++) {
						double gb = ptrk/xmass[j];
						double beta = gb/sqrt(1+gb*gb);
						texp[j] = 10 * path /beta/velc;
					}
					m_cntr_etof  = cntr;
					m_ptot_etof = ptrk;
					m_ph_etof   = ph;
					m_rhit_etof  = rhit;
					m_qual_etof  = qual;
					m_te_etof    = tof - texp[0];
					m_tmu_etof   = tof - texp[1];
					m_tpi_etof   = tof - texp[2];
					m_tk_etof    = tof - texp[3];
					m_tp_etof    = tof - texp[4];
		            m_tuple8->write();
				}
				else {//barrel
					if( !(status->is_counter()) ) continue; // ? 
					if(status->layer()==1){ //layer1
						double path=(*iter_tof)->path(); // ? 
						double tof  = (*iter_tof)->tof();
						double ph   = (*iter_tof)->ph();
						double rhit = (*iter_tof)->zrhit();
						double qual = 0.0 + (*iter_tof)->quality();
						double cntr = 0.0 + (*iter_tof)->tofID();
						double texp[5];
						for(int j = 0; j < 5; j++) {
							double gb = ptrk/xmass[j];
							double beta = gb/sqrt(1+gb*gb);
							texp[j] = 10 * path /beta/velc;
						}

						m_cntr_btof1  = cntr;
						m_ptot_btof1 = ptrk;
						m_ph_btof1   = ph;
						m_zhit_btof1  = rhit;
						m_qual_btof1  = qual;
						m_te_btof1    = tof - texp[0];
						m_tmu_btof1   = tof - texp[1];
						m_tpi_btof1   = tof - texp[2];
						m_tk_btof1    = tof - texp[3];
						m_tp_btof1    = tof - texp[4];
		                m_tuple9->write();
					}

					if(status->layer()==2){//layer2
						double path=(*iter_tof)->path(); // ? 
						double tof  = (*iter_tof)->tof();
						double ph   = (*iter_tof)->ph();
						double rhit = (*iter_tof)->zrhit();
						double qual = 0.0 + (*iter_tof)->quality();
						double cntr = 0.0 + (*iter_tof)->tofID();
						double texp[5];
						for(int j = 0; j < 5; j++) {
							double gb = ptrk/xmass[j];
							double beta = gb/sqrt(1+gb*gb);
							texp[j] = 10 * path /beta/velc;
						}

						m_cntr_btof2  = cntr;
						m_ptot_btof2 = ptrk;
						m_ph_btof2   = ph;
						m_zhit_btof2  = rhit;
						m_qual_btof2  = qual;
						m_te_btof2    = tof - texp[0];
						m_tmu_btof2   = tof - texp[1];
						m_tpi_btof2   = tof - texp[2];
						m_tk_btof2    = tof - texp[3];
						m_tp_btof2    = tof - texp[4];
		                m_tuple10->write();
					} 
				}

				delete status; 
			} 
		} // loop all charged track
	}  // check tof

	//Record the E/p ratio of charged tracks

	Vp4 pep,pem,pmup,pmum,ppip,ppim;
	pep.clear();
	pem.clear();
	pmup.clear();
	pmum.clear();
	ppip.clear();
	ppim.clear();

	Vint iep,iem,imup,imum,ipip,ipim;
	iep.clear();
	iem.clear();
	imup.clear();
	imum.clear();
	ipip.clear();
	ipim.clear();

	double ETrk;
	double PTrk;
	double E_p_r;

	for (int i = 0; i < nGood; i++)
	{
		EvtRecTrackIterator itTrk = evtRecTrkCol -> begin() + iGood[i];
		RecMdcKalTrack* mdcTrk = (*itTrk) -> mdcKalTrack();
		if (!(*itTrk) -> isEmcShowerValid())
		{ ETrk  = -100;} 
		else
		{
			RecEmcShower *emcTrk = (*itTrk) -> emcShower();
			ETrk       = emcTrk ->energy();
		}
		PTrk       = mdcTrk -> p();
		E_p_r=ETrk/PTrk;

		E_EMC=ETrk;
		P_MDC=PTrk;
		E_P_ratio=E_p_r;

		//	cout<<"P_MDC="<<P_MDC<<endl;

		if(debug_key)cout<<"E_EMC="<<E_EMC<<endl;
		if(debug_key)cout<<"P_MDC="<<P_MDC<<endl;
		//	cout<<"E_P_ratio="<<E_P_ratio<<endl;

		if(P_MDC>1&&P_MDC<3)
		{
			if(E_p_r>0.8)
			{
				RecMdcKalTrack::setPidType  (RecMdcKalTrack::electron);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion
				if(mdcTrk->charge() >0) {
		            if(!(*itTrk)->isMdcTrackValid()) continue;
		            RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
		            HepVector a = mdcTrk->helix();
		            HepSymMatrix Ea = mdcTrk->err();
		            HepPoint3D point0(0.,0.,0.);   // the initial point for MDC recosntruction
		            HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]); 
		            VFHelix helixip(point0,a,Ea); 
		            helixip.pivot(IP);
		            HepVector vecipa = helixip.a();
		            double  Rvxy0=fabs(vecipa[0]);  //the nearest distance to IP in xy plane
		            double  Rvz0=vecipa[3];         //the nearest distance to IP in z direction
		            double  Rvphi0=vecipa[1];
		            double  costheta0 = cos(mdcTrk -> theta());
		            m_Vr_lepp = Rvxy0;
		            m_Vz_lepp = Rvz0;
		            m_cos_lepp = costheta0;

					iep.push_back(iGood[i]);
					HepLorentzVector ptrk;
					ptrk.setPx(mdcTrk->px());
					ptrk.setPy(mdcTrk->py());
					ptrk.setPz(mdcTrk->pz());
					double p3 = ptrk.mag();
					ptrk.setE(sqrt(p3*p3+me*me));
					pep.push_back(ptrk);
				}
				else {
		            RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
		            HepVector a = mdcTrk->helix();
		            HepSymMatrix Ea = mdcTrk->err();
		            HepPoint3D point0(0.,0.,0.);   // the initial point for MDC recosntruction
		            HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]); 
		            VFHelix helixip(point0,a,Ea); 
		            helixip.pivot(IP);
		            HepVector vecipa = helixip.a();
		            double  Rvxy0=fabs(vecipa[0]);  //the nearest distance to IP in xy plane
		            double  Rvz0=vecipa[3];         //the nearest distance to IP in z direction
		            double  Rvphi0=vecipa[1];
		            double  costheta0 = cos(mdcTrk -> theta());
		            m_Vr_lepm = Rvxy0;
		            m_Vz_lepm = Rvz0;
		            m_cos_lepm = costheta0;

					iem.push_back(iGood[i]);
					HepLorentzVector ptrk;
					ptrk.setPx(mdcTrk->px());
					ptrk.setPy(mdcTrk->py());
					ptrk.setPz(mdcTrk->pz());
					double p3 = ptrk.mag();
					ptrk.setE(sqrt(p3*p3+me*me));

					pem.push_back(ptrk);
				}

			}
			else if(ETrk<0.4)
			{
				RecMdcKalTrack::setPidType  (RecMdcKalTrack::muon);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion
				if(mdcTrk->charge() >0) {
		            RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
		            HepVector a = mdcTrk->helix();
		            HepSymMatrix Ea = mdcTrk->err();
		            HepPoint3D point0(0.,0.,0.);   // the initial point for MDC recosntruction
		            HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]); 
		            VFHelix helixip(point0,a,Ea); 
		            helixip.pivot(IP);
		            HepVector vecipa = helixip.a();
		            double  Rvxy0=fabs(vecipa[0]);  //the nearest distance to IP in xy plane
		            double  Rvz0=vecipa[3];         //the nearest distance to IP in z direction
		            double  Rvphi0=vecipa[1];
		            double  costheta0 = cos(mdcTrk -> theta());
		            m_Vr_lepp = Rvxy0;
		            m_Vz_lepp = Rvz0;
		            m_cos_lepp = costheta0;

					imup.push_back(iGood[i]);
					HepLorentzVector ptrk;
					ptrk.setPx(mdcTrk->px());
					ptrk.setPy(mdcTrk->py());
					ptrk.setPz(mdcTrk->pz());
					double p3 = ptrk.mag();
					ptrk.setE(sqrt(p3*p3+mmu*mmu));

					pmup.push_back(ptrk);
				}
				else {
		            RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
		            HepVector a = mdcTrk->helix();
		            HepSymMatrix Ea = mdcTrk->err();
		            HepPoint3D point0(0.,0.,0.);   // the initial point for MDC recosntruction
		            HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]); 
		            VFHelix helixip(point0,a,Ea); 
		            helixip.pivot(IP);
		            HepVector vecipa = helixip.a();
		            double  Rvxy0=fabs(vecipa[0]);  //the nearest distance to IP in xy plane
		            double  Rvz0=vecipa[3];         //the nearest distance to IP in z direction
		            double  Rvphi0=vecipa[1];
		            double  costheta0 = cos(mdcTrk -> theta());
		            m_Vr_lepm = Rvxy0;
		            m_Vz_lepm = Rvz0;
		            m_cos_lepm = costheta0;

					imum.push_back(iGood[i]);
					HepLorentzVector ptrk;
					ptrk.setPx(mdcTrk->px());
					ptrk.setPy(mdcTrk->py());
					ptrk.setPz(mdcTrk->pz());
					double p3 = ptrk.mag();
					ptrk.setE(sqrt(p3*p3+mmu*mmu));

					pmum.push_back(ptrk);
				}
			}	
		}//P_MDC >1 GeV
		else if(P_MDC<1)
		{
			RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion
			if(mdcTrk->charge() >0) {
		        RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
		        HepVector a = mdcTrk->helix();
		        HepSymMatrix Ea = mdcTrk->err();
		        HepPoint3D point0(0.,0.,0.);   // the initial point for MDC recosntruction
		        HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]); 
		        VFHelix helixip(point0,a,Ea); 
		        helixip.pivot(IP);
		        HepVector vecipa = helixip.a();
		        double  Rvxy0=fabs(vecipa[0]);  //the nearest distance to IP in xy plane
		        double  Rvz0=vecipa[3];         //the nearest distance to IP in z direction
		        double  Rvphi0=vecipa[1];
		        double  costheta0 = cos(mdcTrk -> theta());
		        m_Vr_pionp = Rvxy0;
		        m_Vz_pionp = Rvz0;
		        m_cos_pionp = costheta0;

				ipip.push_back(iGood[i]);
				HepLorentzVector ptrk;
				ptrk.setPx(mdcTrk->px());
				ptrk.setPy(mdcTrk->py());
				ptrk.setPz(mdcTrk->pz());
				double p3 = ptrk.mag();
				ptrk.setE(sqrt(p3*p3+mpi*mpi));

				ppip.push_back(ptrk);
			} else {
		        RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
		        HepVector a = mdcTrk->helix();
		        HepSymMatrix Ea = mdcTrk->err();
		        HepPoint3D point0(0.,0.,0.);   // the initial point for MDC recosntruction
		        HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]); 
		        VFHelix helixip(point0,a,Ea); 
		        helixip.pivot(IP);
		        HepVector vecipa = helixip.a();
		        double  Rvxy0=fabs(vecipa[0]);  //the nearest distance to IP in xy plane
		        double  Rvz0=vecipa[3];         //the nearest distance to IP in z direction
		        double  Rvphi0=vecipa[1];
		        double  costheta0 = cos(mdcTrk -> theta());
		        m_Vr_pionm = Rvxy0;
		        m_Vz_pionm = Rvz0;
		        m_cos_pionm = costheta0;

				ipim.push_back(iGood[i]);
				HepLorentzVector ptrk;
				ptrk.setPx(mdcTrk->px());
				ptrk.setPy(mdcTrk->py());
				ptrk.setPz(mdcTrk->pz());
				double p3 = ptrk.mag();
				ptrk.setE(sqrt(p3*p3+mpi*mpi));

				ppim.push_back(ptrk);
			}
		}
		m_tuple_charged->write();
	}//For circle

	int Num_ep=iep.size();
	int Num_em=iem.size();
	int Num_mup=imup.size();
	int Num_mum=imum.size();
	int Num_pip=ipip.size();
	int Num_pim=ipim.size();


	//End record the E/p ratio of charged tracks

	double mass_2e;	
	double mass_2mu;	
	if(Num_ep==1&&Num_em==1)
	{
		mass_2e=(pep[0]+pem[0]).m();
	}
	else if(Num_mup==1&&Num_mum==1)
	{
		mass_2mu=(pmup[0]+pmum[0]).m();
	}
	else {return StatusCode::SUCCESS;} 
	Ncut6++;

	//End record the recoil mass of two isolate photons

	if(Num_pip!=1||Num_pim!=1) return StatusCode::SUCCESS;

	Ncut3++;

	//*****************************************************************
	//  Apply Kinematic 4C fit
	// ***************************************************************
	//for jpsi-->ee 
	flag=0;
	HepLorentzVector pip,pim,lp,lm,m_lv_recoili,m_lv_jpsi,m_lv_recoil;
	//double m_lv_jpsi;
	while(Num_ep==1&&Num_em==1&&Num_pip==1&&Num_pim==1)
	{
		flag=1;
		RecMdcKalTrack *epTrk = (*(evtRecTrkCol->begin()+iep[0]))->mdcKalTrack();
		RecMdcKalTrack *emTrk = (*(evtRecTrkCol->begin()+iem[0]))->mdcKalTrack();

		RecMdcKalTrack *pipTrk = (*(evtRecTrkCol->begin()+ipip[0]))->mdcKalTrack();
		RecMdcKalTrack *pimTrk = (*(evtRecTrkCol->begin()+ipim[0]))->mdcKalTrack();

		WTrackParameter wvepTrk, wvemTrk;
		wvepTrk = WTrackParameter(me, epTrk->getZHelixE(), epTrk->getZErrorE());
		wvemTrk = WTrackParameter(me, emTrk->getZHelixE(), emTrk->getZErrorE());
		WTrackParameter wvpipTrk, wvpimTrk;
		wvpipTrk = WTrackParameter(mpi, pipTrk->getZHelix(), pipTrk->getZError());
		wvpimTrk = WTrackParameter(mpi, pimTrk->getZHelix(), pimTrk->getZError());

		//
		//    Test vertex fit
		//

		HepPoint3D vx(0., 0., 0.);
		HepSymMatrix Evx(3, 0);
		double bx = 1E+6;
		double by = 1E+6;
		double bz = 1E+6;
		Evx[0][0] = bx*bx;
		Evx[1][1] = by*by;
		Evx[2][2] = bz*bz;

		VertexParameter vxpar;
		vxpar.setVx(vx);
		vxpar.setEvx(Evx);

		VertexFit* vtxfit = VertexFit::instance();
		vtxfit->init();
		vtxfit->AddTrack(0,  wvepTrk);
		vtxfit->AddTrack(1,  wvemTrk);
		vtxfit->AddTrack(2,  wvpipTrk);
		vtxfit->AddTrack(3,  wvpimTrk);
		vtxfit->AddVertex(0, vxpar,0, 1, 2, 3);
		if(!vtxfit->Fit(0)) break;
		vtxfit->Swim(0);

		WTrackParameter wep = vtxfit->wtrk(0);
		WTrackParameter wem = vtxfit->wtrk(1);
		WTrackParameter wpip = vtxfit->wtrk(2);
		WTrackParameter wpim = vtxfit->wtrk(3);

        // Secondary Vertex fit
        stat_fitSecondVertex = fitSecondVertex(wpip, wpim);

		vx=vtxfit->vx(0);
		Evx=vtxfit->Evx(0);


		bool fit_ok=false;
		double chisq = 9999.;

		KalmanKinematicFit * kmfit = KalmanKinematicFit::instance();
		kmfit->init();
		kmfit->setBeamPosition(vx);
		kmfit->setVBeamPosition(Evx);
		kmfit->AddTrack(0, wep);
		kmfit->AddTrack(1, wem);
		kmfit->AddTrack(2, wpip);
		kmfit->AddTrack(3, wpim);
		kmfit->AddFourMomentum(0, ecms);
		if(!kmfit->Fit()) return StatusCode::SUCCESS;
		bool oksq = kmfit->Fit();
		if(oksq) 
		{
			if(chisq>kmfit->chisq()){
				chisq = kmfit->chisq();
				lp = kmfit->pfit(0);
				lm = kmfit->pfit(1);
				pip = kmfit->pfit(2);
				pim = kmfit->pfit(3);

				chi_4c_pipi_ll = kmfit->chisq();
				Ncut4++;
			}

		}
		break;
	}// for jpsi-->ee



	//for jpsi-->mumu 
	while(Num_mup==1&&Num_mum==1&&Num_pip==1&&Num_pim==1)
	{
		flag=-1;
		RecMdcKalTrack *mupTrk = (*(evtRecTrkCol->begin()+imup[0]))->mdcKalTrack();
		RecMdcKalTrack *mumTrk = (*(evtRecTrkCol->begin()+imum[0]))->mdcKalTrack();

		RecMdcKalTrack *pipTrk = (*(evtRecTrkCol->begin()+ipip[0]))->mdcKalTrack();
		RecMdcKalTrack *pimTrk = (*(evtRecTrkCol->begin()+ipim[0]))->mdcKalTrack();

		WTrackParameter wvmupTrk, wvmumTrk;
		wvmupTrk = WTrackParameter(mmu, mupTrk->getZHelixMu(), mupTrk->getZErrorMu());
		wvmumTrk = WTrackParameter(mmu, mumTrk->getZHelixMu(), mumTrk->getZErrorMu());
		WTrackParameter wvpipTrk, wvpimTrk;
		wvpipTrk = WTrackParameter(mpi, pipTrk->getZHelix(), pipTrk->getZError());
		wvpimTrk = WTrackParameter(mpi, pimTrk->getZHelix(), pimTrk->getZError());

		//
		//    Test vertex fit
		//

		HepPoint3D vx(0., 0., 0.);
		HepSymMatrix Evx(3, 0);
		double bx = 1E+6;
		double by = 1E+6;
		double bz = 1E+6;
		Evx[0][0] = bx*bx;
		Evx[1][1] = by*by;
		Evx[2][2] = bz*bz;

		VertexParameter vxpar;
		vxpar.setVx(vx);
		vxpar.setEvx(Evx);

		VertexFit* vtxfit = VertexFit::instance();
		vtxfit->init();
		vtxfit->AddTrack(0,  wvmupTrk);
		vtxfit->AddTrack(1,  wvmumTrk);
		vtxfit->AddTrack(2,  wvpipTrk);
		vtxfit->AddTrack(3,  wvpimTrk);
		vtxfit->AddVertex(0, vxpar,0, 1,2,3);
		if(!vtxfit->Fit(0)) break;
		vtxfit->Swim(0);

		WTrackParameter wmup = vtxfit->wtrk(0);
		WTrackParameter wmum = vtxfit->wtrk(1);
		WTrackParameter wpip = vtxfit->wtrk(2);
		WTrackParameter wpim = vtxfit->wtrk(3);

        // Secondary Vertex fit
        stat_fitSecondVertex = fitSecondVertex(wpip, wpim);

		vx=vtxfit->vx(0);
		Evx=vtxfit->Evx(0);

		//
		//  Apply Kinematic 4C fit
		// 
		double chisq = 9999.;
		KalmanKinematicFit * kmfit = KalmanKinematicFit::instance();
		kmfit->init();
		kmfit->setBeamPosition(vx);
		kmfit->setVBeamPosition(Evx);
		kmfit->AddTrack(0, wmup);
		kmfit->AddTrack(1, wmum);
		kmfit->AddTrack(2, wpip);
		kmfit->AddTrack(3, wpim);
		kmfit->AddFourMomentum(0, ecms);
		if(!kmfit->Fit()) return StatusCode::SUCCESS;
		bool oksq = kmfit->Fit();
		if(oksq) 
		{
			if(chisq>kmfit->chisq()){
				chisq = kmfit->chisq();
				lp = kmfit->pfit(0);
				lm = kmfit->pfit(1);
				pip = kmfit->pfit(2) ;
				pim = kmfit->pfit(3);

				chi_4c_pipi_ll = kmfit->chisq();
				Ncut5++;
			}
		}
		break;
	}// for jpsi-->mumu
	p4_lp[0]= lp.px();
	p4_lp[1]= lp.py();
	p4_lp[2]= lp.pz();
	p4_lp[3]= lp.e();

	p4_lm[0]= lm.px();
	p4_lm[1]= lm.py();
	p4_lm[2]= lm.pz();
	p4_lm[3]= lm.e();

	p4_pip[0]= pip.px();
	p4_pip[1]= pip.py();
	p4_pip[2]= pip.pz();
	p4_pip[3]= pip.e();

	p4_pim[0]= pim.px();
	p4_pim[1]= pim.py();
	p4_pim[2]= pim.pz();
	p4_pim[3]= pim.e();


	m_pipi_dang = pip.vect().cosTheta(pim.vect());
	m_pimlp_dang = pip.vect().cosTheta(lp.vect());
	m_piplm_dang = pip.vect().cosTheta(lm.vect());

	m_lv_pionp=pip.m();
	m_lv_pionm=pim.m();
	m_lv_lepp=lp.m();
	m_lv_lepm=lm.m();

	m_lv_jpsi = lp+lm;
	m_lv_recoil = ecms-pip-pim;

	m_jpsi=m_lv_jpsi.m();
	m_recoil=m_lv_recoil.m();

	Hep3Vector m_boost_jpsi(m_lv_recoil.boostVector());
	HepLorentzVector m_lv_cms_lepp(boostOf(lp,-m_boost_jpsi));
	HepLorentzVector m_lv_cms_lepm(boostOf(lm,-m_boost_jpsi));
	m_cms_lepm = m_lv_cms_lepm.vect().mag();
	m_cms_lepp = m_lv_cms_lepp.vect().mag();

	m_tuple_pipi_ll->write();


    // save all McTruth info
    m_idxmc = 100;
    for (int i = 0; i< m_idxmc; i++) {
        m_motheridx[i] = 0;
        m_pdgid[i] = 0;
    }
    if (m_runNo < 0) {
        m_idxmc = idxmc;
        for (int i = 0; i< m_idxmc; i++) {
            m_motheridx[i] = motheridx[i];
            m_pdgid[i] = pdgid[i];
        }
    }
	m_tuple11->write();

}

bool pipi_jpsi::fitSecondVertex(WTrackParameter &piplus, WTrackParameter &piminus) {
    m_chi2_svf = 999.;
    m_L_svf = 999.;
    m_Lerr_svf = 99.;
    m_ctau_svf = 999.;
    Hep3Vector ip(0, 0, 0);
    HepSymMatrix ipEx(3, 0);
    IVertexDbSvc* vtxsvc;
    Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
    if (vtxsvc->isVertexValid()) {
        double* dbv = vtxsvc->PrimaryVertex();
        double* vv = vtxsvc->SigmaPrimaryVertex();
        ip.setX(dbv[0]);
        ip.setY(dbv[1]);
        ip.setZ(dbv[2]);
        ipEx[0][0] = vv[0] * vv[0];
        ipEx[1][1] = vv[1] * vv[1];
        ipEx[2][2] = vv[2] * vv[2];
    }
    else false;
    VertexParameter bs;
    bs.setVx(ip);
    bs.setEvx(ipEx);
    HepPoint3D vx(0., 0., 0.);
    HepSymMatrix Evx(3, 0);
    double bx = 1E+6;
    double by = 1E+6;
    double bz = 1E+6;
    Evx[0][0] = bx * bx;
    Evx[1][1] = by * by;
    Evx[2][2] = bz * bz;
    // vertex fit
    VertexParameter vxpar;
    vxpar.setVx(vx);
    vxpar.setEvx(Evx);
    VertexFit *vtxfit = VertexFit::instance();
    vtxfit->init();
    vtxfit->AddTrack(0, piplus);
    vtxfit->AddTrack(1, piminus);
    vtxfit->AddVertex(0, vxpar, 0, 1);
    if (!(vtxfit->Fit(0))) return false;
    vtxfit->Swim(0);
    vtxfit->BuildVirtualParticle(0);
    // second vertex fit
    SecondVertexFit *svtxfit = SecondVertexFit::instance();
    svtxfit->init();
    svtxfit->setPrimaryVertex(bs);
    svtxfit->AddTrack(0, vtxfit->wVirtualTrack(0));
    svtxfit->setVpar(vtxfit->vpar(0));
    if (!svtxfit->Fit()) return false;
    m_chi2_svf = svtxfit->chisq();
    m_ctau_svf = svtxfit->ctau();
    m_L_svf = svtxfit->decayLength();
    m_Lerr_svf = svtxfit->decayLengthError();
    return true;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode pipi_jpsi::finalize() {
	cout<<"total number:                "<<Ncut0<<endl;
	cout<<"nGood==4,nCharge==0          "<<Ncut1<<endl;
	cout<<"nGam>=1:                     "<<Ncut2<<endl;
	cout<<"Npip==1&&Npim==1:            "<<Ncut3<<endl;
	cout<<"ee Pass 4C:                  "<<Ncut4<<endl;
	cout<<"mumu Pass 4C:                "<<Ncut5<<endl;
	cout<<"Nlp==1&&Nlm==1:              "<<Ncut6<<endl;
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in finalize()" << endmsg;
	return StatusCode::SUCCESS;
}
