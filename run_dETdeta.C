#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <g4jets/TruthJetInput.h>
#include <fstream>
#include <phool/recoConsts.h>
#include <TSystem.h>
#include "mdctreemaker/MDCTreeMaker.h"
#include <caloreco/CaloTowerCalib.h>
#include <g4mbd/MbdDigitization.h>
#include <mbd/MbdReco.h>
#include <frog/FROG.h>
#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <centrality/CentralityReco.h>
#include <calotrigger/MinimumBiasClassifier.h>
using namespace std;

R__LOAD_LIBRARY(libFROG.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libg4vertex.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libmdctreemaker.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libg4mbd.so)
R__LOAD_LIBRARY(libmbd_io.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libcalotrigger.so)

bool file_exists(const char* filename)
{
  std::ifstream infile(filename);
  return infile.good();
}

int run_dETdeta(int nproc = 0, string tag = "", int datormc = 0, int debug = 0, int nevt = 0, int correct = 1, int zs = 0, int runnumber = 23727)
{
  int verbosity = 0;
  string filename = "output/evt/events_"+tag+(tag==""?"":"_");
  string dattag;
  switch(datormc) {
    case 0: dattag = "data"; break;
    case 1: dattag = "mc"; break;
    case 2: dattag = "epos"; break;
    case 3: dattag = "ampt"; break;
    default: dattag = ""; std::cout << "data or mc generator value not valid" << std::endl; break;
  }
  string cortag = (correct?"cor":"unc");
  filename += dattag + "_"+cortag+"_" + to_string(nproc);
  filename += ".root";
  std::cout << filename << std::endl;
  FROG *fr = new FROG();
    
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libjetbackground.so");
  gSystem->Load("libcalo_io.so");
  gSystem->Load("libmdctreemaker.so");
  
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity( verbosity );
  // just if we set some flags somewhere in this macro
  recoConsts *rc =  recoConsts::instance();
  rc->set_uint64Flag("TIMESTAMP",runnumber);
  if (datormc == 0) { rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2023"); }
  else { rc->set_StringFlag("CDB_GLOBALTAG","MDC2"); } //"ProdA_2023");  "2023p007" 

  ifstream list1;
  string line1;
  ifstream list2;
  string line2;
  ifstream list3;
  string line3;
  ifstream list4;
  string line4;
  if (datormc == 0) {
    string datafilelist = "/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_calo-000";
    datafilelist += to_string(runnumber) + ".list";
    list1.open(datafilelist);
  } else if (datormc == 1) {
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_calo_nozero_hijing.list");
    list2.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_global_hijing.list");
    list3.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_truth_hijing.list");
    list4.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_mbd_epd_hijing.list");
  } else if (datormc == 2) {
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_calo_nozero_epos.list");
    list2.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_global_epos.list");
    list3.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_truth_epos.list");
    list4.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_mbd_epd_epos.list");
  } else if (datormc == 3) {
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_calo_nozero_ampt.list");
    list2.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_global_ampt.list");
    list3.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_truth_ampt.list");
    list4.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_mbd_epd_ampt.list");
  }

  for(int i=0; i<nproc+1; i++) 
    {
      getline(list1, line1);
      if(datormc > 0)
	{
	  getline(list4, line4);
	  getline(list2, line2);
	  getline(list3, line3);
	}
    }

  Fun4AllInputManager *in_1 = new Fun4AllDstInputManager("DSTin1");
  Fun4AllInputManager *in_2 = new Fun4AllDstInputManager("DSTin2");
  Fun4AllInputManager *in_3 = new Fun4AllDstInputManager("DSTin3");
  Fun4AllInputManager *in_4 = new Fun4AllDstInputManager("DSTin4");
  in_1->AddFile(line1);
  if(datormc > 0)
    {
      in_2->AddFile(line2);
      in_3->AddFile(line3);
      in_4->AddFile(line4);
    }
  se->registerInputManager( in_1 );
  if(datormc > 0)
    {
      se->registerInputManager(in_2);
      se->registerInputManager(in_3);
      se->registerInputManager(in_4);
    }
                                  
  int cont = 0;
/*
  MbdDigitization* mbddigi;
  MbdReco* mbdreco;
  if(datormc)
    {
      mbdreco = new MbdReco();
      mbddigi = new MbdDigitization();
      se->registerSubsystem(mbddigi);
      se->registerSubsystem(mbdreco);
    }
    */
  if(!datormc) {
    MinimumBiasClassifier *mb = new MinimumBiasClassifier();
    mb->Verbosity(verbosity);
    se->registerSubsystem(mb);
  }
  
  CentralityReco* cent = new CentralityReco();
  cent->Verbosity(verbosity);
  se->registerSubsystem( cent ); 
  
  /*
  DansSpecialVertex *dsv;
  if(!datormc)
    {
      dsv = new DansSpecialVertex("DansSpecialVertex", "dump.root");
      dsv->SetRunNumber(runnumber);
      dsv->Verbosity(0);
      se->registerSubsystem(dsv);
    }
  */
  Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
  
  CDBInterface *cdb = CDBInterface::instance();
  std::string geoLocation = cdb->getUrl("calo_geo");
  intrue2->AddFile(geoLocation);
  se->registerInputManager(intrue2);

  // option for testing new emcal calibrations with direct URL instead of CDB
  /*
  if (!datormc) {
    std::cout << "Calibrating EMCal" << std::endl;
    CaloTowerCalib *calibEMC = new CaloTowerCalib("CEMCCALIB");
    calibEMC->set_detector_type(CaloTowerDefs::CEMC);
    calibEMC->set_directURL("/sphenix/user/egm2153/calib_study/detdeta/testing/new_emcal_calib.root");
    se->registerSubsystem(calibEMC);
  }
  */
  // option for testing new emcal calibrations with direct URL instead of CDB

  MDCTreeMaker *tt = new MDCTreeMaker( filename, datormc, debug, correct );
  se->registerSubsystem( tt );
  se->Print("NODETREE");
  se->run(nevt);
  cout << "Ran all events" << endl;
  se->End();
  cout << "Ended server" << endl;
  delete se;
  cout << "Deleted server" << endl;
  gSystem->Exit(0);
  cout << "Exited gSystem" << endl;
  return 0;

}
