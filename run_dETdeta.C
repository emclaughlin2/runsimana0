#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
//#include <jetbase/FastJetAlgo.h>
//#include <jetbase/JetReco.h>
//#include <jetbase/TowerJetInput.h>
//#include <g4jets/TruthJetInput.h>
#include <fstream>
#include <phool/recoConsts.h>
#include <TSystem.h>
#include "mdctreemaker/MDCTreeMaker.h"
#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloTowerStatus.h>
#include <g4mbd/MbdDigitization.h>
#include <mbd/MbdReco.h>
#include <frog/FROG.h>
#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <centrality/CentralityReco.h>
#include <calotrigger/MinimumBiasClassifier.h>
//#include <G4Setup_sPHENIX.C>
#include <energycorrection/EnergyCorrection.h>
#include "/sphenix/u/egm2153/sPHENIX-software/MDC2/submit/fm_0_20/pass2calo_nopileup_nozero/rundir/G4_CEmc_Spacal.C"
#include "/sphenix/u/egm2153/sPHENIX-software/MDC2/submit/fm_0_20/pass2calo_nopileup_nozero/rundir/G4_HcalIn_ref.C"
#include "/sphenix/u/egm2153/sPHENIX-software/MDC2/submit/fm_0_20/pass2calo_nopileup_nozero/rundir/G4_HcalOut_ref.C"
#include "Sys_Calo.C"

using namespace std;

R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libEnergyCorrection.so)
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

int run_dETdeta(int nproc = 0, string tag = "", int datormc = 0, int debug = 0, int nevt = 0, int correct = 1, int zs = 0, int upweightb = 0, int doupweight = 0, int runnumber = 23727)
{

  const std::string emcal_node = "TOWERINFO_CALIB_CEMC";
  const std::string ihcal_node = "TOWERINFO_CALIB_HCALIN";
  const std::string ohcal_node = "TOWERINFO_CALIB_HCALOUT";

  //cout << Enable::CDB << endl;
  int verbosity = 0;
  string filename = "output/evt/events_"+tag+(tag==""?"":"_"); 
  //string filename = "events_"+tag+(tag==""?"":"_");
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
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity( verbosity );
  // just if we set some flags somewhere in this macro
  recoConsts *rc =  recoConsts::instance();
  rc->set_uint64Flag("TIMESTAMP",runnumber);
  if (datormc == 0) { rc->set_StringFlag("CDB_GLOBALTAG","2023p011"); }
  else { rc->set_StringFlag("CDB_GLOBALTAG","MDC2"); } //"ProdA_2023");  "2023p007" 
  //rc->set_IntFlag("RANDOMSEED",158804);

  ifstream list1;
  string line1;
  ifstream list2;
  string line2;
  ifstream list3;
  string line3;
  ifstream list4;
  string line4;
  ifstream list5;
  string line5;

  if (datormc == 0) {
    string datafilelist = "/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_calo-000";
    datafilelist += to_string(runnumber) + ".list";
    list1.open(datafilelist);
  } else if (datormc == 1) {
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_calo_nozero_hijing.list");
    list2.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_global_hijing.list");
    list3.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_truth_hijing.list");
    //list4.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_mbd_epd_hijing.list"); // want nopileup option
    list4.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/g4hits_hijing.list");
    //if (upweightb) { list5.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/g4hits_hijing.list"); }
  } else if (datormc == 2) {
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_calo_cluster_epos.list");
    list2.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_global_epos.list");
    list3.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_truth_epos.list");
    //list4.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_mbd_epd_epos.list");
    if (upweightb) { list5.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/g4hits_epos.list"); }
  } else if (datormc == 3) {
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_calo_nozero_ampt.list");
    list2.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_global_ampt.list");
    list3.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_truth_ampt.list");
    //list4.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_mbd_epd_ampt.list");
    if (upweightb) { list5.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/g4hits_ampt.list"); }
  } else if (datormc == 4) {
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_hcal_pedestal_data.list");
  }

  for(int i=0; i<nproc+1; i++) 
    {
      getline(list1, line1);
      if(datormc > 0 && datormc < 4)
	{
	  getline(list4, line4);
	  getline(list2, line2);
	  getline(list3, line3);
    //if (upweightb) { getline(list5, line5); }
	}
    }

  Fun4AllInputManager *in_1 = new Fun4AllDstInputManager("DSTin1");
  Fun4AllInputManager *in_2 = new Fun4AllDstInputManager("DSTin2");
  Fun4AllInputManager *in_3 = new Fun4AllDstInputManager("DSTin3");
  Fun4AllInputManager *in_4 = new Fun4AllDstInputManager("DSTin4");
  Fun4AllInputManager *in_5 = new Fun4AllDstInputManager("DSTin5");
  in_1->AddFile(line1);
  if(datormc > 0 && datormc < 4)
    {
      in_2->AddFile(line2);
      in_3->AddFile(line3);
      in_4->AddFile(line4);
      //if (upweightb) { in_5->AddFile(line5); }
    }
  if (!upweightb) se->registerInputManager( in_1 );
  if(datormc > 0 && datormc < 4)
    {
      se->registerInputManager(in_2);
      se->registerInputManager(in_3);
      se->registerInputManager(in_4);
    }

    
  EnergyCorrection *energycorrect;
  if(upweightb)
    {
      energycorrect = new EnergyCorrection();
      energycorrect->Verbosity(1);
      if (datormc == 1) { energycorrect->SetGeneratorType("HIJING"); }
      else if (datormc == 2) { energycorrect->SetGeneratorType("EPOS"); }
      else if (datormc == 3) { energycorrect->SetGeneratorType("AMPT"); }
      energycorrect->SetHitNodeName("G4HIT_CEMC");
      energycorrect->SetMinEta(-2.5); 
      energycorrect->SetMaxEta(2.5); 
      energycorrect->SetUpweightTruth((doupweight?true:false)); //only want to upweight the truth once
      if(upweightb && datormc) se->registerSubsystem(energycorrect);
      
      energycorrect = new EnergyCorrection();
      energycorrect->Verbosity(1);
      if (datormc == 1) { energycorrect->SetGeneratorType("HIJING"); }
      else if (datormc == 2) { energycorrect->SetGeneratorType("EPOS"); }
      else if (datormc == 3) { energycorrect->SetGeneratorType("AMPT"); }
      energycorrect->SetHitNodeName("G4HIT_HCALIN");
      energycorrect->SetMinEta(-2.5); 
      energycorrect->SetMaxEta(2.5); 
      energycorrect->SetUpweightTruth(false);
      if(upweightb && datormc) se->registerSubsystem(energycorrect);
      
      energycorrect = new EnergyCorrection();
      energycorrect->Verbosity(1);
      if (datormc == 1) { energycorrect->SetGeneratorType("HIJING"); }
      else if (datormc == 2) { energycorrect->SetGeneratorType("EPOS"); }
      else if (datormc == 3) { energycorrect->SetGeneratorType("AMPT"); }
      energycorrect->SetHitNodeName("G4HIT_HCALOUT");
      energycorrect->SetMinEta(-2.5); 
      energycorrect->SetMaxEta(2.5); 
      energycorrect->SetUpweightTruth(false);
      if(upweightb && datormc) se->registerSubsystem(energycorrect);
    }

  if (upweightb) {
    //se->registerInputManager(in_5);

    Enable::CEMC = true;
    Enable::CEMC_CELL = Enable::CEMC && true;
    Enable::CEMC_TOWER = Enable::CEMC_CELL && true;
    Enable::CEMC_CLUSTER = Enable::CEMC_TOWER && false;

    Enable::HCALIN = true;
    Enable::HCALIN_CELL = Enable::HCALIN && true;
    Enable::HCALIN_TOWER = Enable::HCALIN_CELL && true;
    Enable::HCALIN_CLUSTER = Enable::HCALIN_TOWER && false;
    G4HCALIN::tower_emin = 0.;

    Enable::HCALOUT = true;
    Enable::HCALOUT_CELL = Enable::HCALOUT && true;
    Enable::HCALOUT_TOWER = Enable::HCALOUT_CELL && true;
    Enable::HCALOUT_CLUSTER = Enable::HCALOUT_TOWER && false;
    G4HCALOUT::tower_emin = 0.;

    //------------------
    // Detector Reconstruction
    //------------------

    if (Enable::CEMC_CELL) CEMC_Cells();

    if (Enable::HCALIN_CELL) HCALInner_Cells();

    if (Enable::HCALOUT_CELL) HCALOuter_Cells();

    //-----------------------------
    // CEMC towering and clustering
    //-----------------------------

    if (Enable::CEMC_TOWER) CEMC_Towers();
    if (Enable::CEMC_CLUSTER) CEMC_Clusters();

    //-----------------------------
    // HCAL towering and clustering
    //-----------------------------

    if (Enable::HCALIN_TOWER) HCALInner_Towers();
    if (Enable::HCALIN_CLUSTER) HCALInner_Clusters();

    if (Enable::HCALOUT_TOWER) HCALOuter_Towers();
    if (Enable::HCALOUT_CLUSTER) HCALOuter_Clusters();
  }
                                   
// The calibrations have a validity range set by the beam clock which is not read out of the prdfs as of now
  
  int cont = 0;
  
  MbdDigitization* mbddigi;
  MbdReco* mbdreco;
  if(datormc)
    {
      mbdreco = new MbdReco();
      mbddigi = new MbdDigitization();
      se->registerSubsystem(mbddigi);
      se->registerSubsystem(mbdreco);
    }


  if(!datormc) {
    MinimumBiasClassifier *mb = new MinimumBiasClassifier();
    mb->Verbosity(verbosity);
    se->registerSubsystem(mb);
  }
  
  if (datormc != 4) {
  CentralityReco* cent = new CentralityReco();
  //cent->Verbosity(2);
  se->registerSubsystem( cent ); 
  }

  if(!datormc) {
    Register_Tower_sys();
  }

  //CDBInterface *cdb = CDBInterface::instance();
  //std::string geoLocation = cdb->getUrl("calo_geo");
  //intrue2->AddFile(geoLocation);
  //se->registerInputManager(intrue2);

  // option for testing new emcal calibrations with direct URL instead of CDB
  
  if (!datormc) {
    std::cout << "Calibrating EMCal" << std::endl;
    CaloTowerCalib *calibEMC = new CaloTowerCalib("CEMCCALIB");
    calibEMC->set_detector_type(CaloTowerDefs::CEMC);
    calibEMC->set_directURL("/sphenix/u/bseidlitz/work/macros/calibrations/calo/emcal_calib_year1/23726_23746/calib_emcal_23726_23746.root");
    //calibEMC->set_directURL("/sphenix/user/egm2153/calib_study/detdeta/testing/new_emcal_calib.root");
    se->registerSubsystem(calibEMC);
  }

  if (!datormc) {
    CaloTowerStatus *statusHCalIn = new CaloTowerStatus("HCALINSTATUS");
    statusHCalIn->set_detector_type(CaloTowerDefs::HCALIN);
    statusHCalIn->set_time_cut(2);
    statusHCalIn->set_fraction_badChi2_threshold(0.02);
    se->registerSubsystem(statusHCalIn);

    std::cout << "Calibrating IHcal" << std::endl;
    CaloTowerCalib *calibIHCal = new CaloTowerCalib("HCALIN");
    calibIHCal->set_detector_type(CaloTowerDefs::HCALIN);
    se->registerSubsystem(calibIHCal);
  }
  
  // option for testing new emcal calibrations with direct URL instead of CDB
  if (datormc == 4) datormc = 0;
  MDCTreeMaker *tt = new MDCTreeMaker("MDCTreeMaker", filename, datormc, debug, correct, emcal_node, ihcal_node, ohcal_node);
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
