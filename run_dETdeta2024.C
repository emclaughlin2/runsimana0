#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
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
#include <fun4all/Fun4AllNoSyncDstInputManager.h>
#include <centrality/CentralityReco.h>
#include <calotrigger/MinimumBiasClassifier.h>
//#include <G4Setup_sPHENIX.C>
#include <energycorrection/EnergyCorrection.h>
//#include "/sphenix/u/egm2153/sPHENIX-software/MDC2/submit/fm_0_20/pass2calo_nopileup_nozero/rundir/G4_CEmc_Spacal.C"
//#include "/sphenix/u/egm2153/sPHENIX-software/MDC2/submit/fm_0_20/pass2calo_nopileup_nozero/rundir/G4_HcalIn_ref.C"
//#include "/sphenix/u/egm2153/sPHENIX-software/MDC2/submit/fm_0_20/pass2calo_nopileup_nozero/rundir/G4_HcalOut_ref.C"
//#include "/sphenix/u/egm2153/sPHENIX-software/sPHENIX-macros/common/G4_CEmc_Spacal.C"
//#include "/sphenix/u/egm2153/sPHENIX-software/sPHENIX-macros/common/G4_HcalIn_ref.C"
//#include "/sphenix/u/egm2153/sPHENIX-software/sPHENIX-macros/common/G4_HcalOut_ref.C"
#include "G4_CEmc_Spacal.C"
#include "G4_HcalIn_ref.C"
#include "G4_HcalOut_ref.C"
//#include "Sys_Calo.C"
#include "Calo_Fitting.C"
#include "Calo_Calib.C"
#include <globalvertex/GlobalVertexReco.h>
#include <fun4all/Fun4AllUtils.h>
#include <zdcinfo/ZdcReco.h>
#include <calovalid/CaloFittingQA.h>
#include <QA.C>
#include <phool/PHRandomSeed.h>

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
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libzdcinfo.so)
R__LOAD_LIBRARY(libcalovalid.so)

bool file_exists(const char* filename)
{
  std::ifstream infile(filename);
  return infile.good();
}

int run_dETdeta2024(int nproc = 0, string tag = "", int datormc = 0, int debug = 0, int nevt = 0, int correct = 1, int zs = 0, int upweightb = 0, int doupweight = 0, int runnumber = 23727)
{

  const std::string emcal_node = "TOWERINFO_CALIB_CEMC";
  const std::string ihcal_node = "TOWERINFO_CALIB_HCALIN";
  const std::string ohcal_node = "TOWERINFO_CALIB_HCALOUT";

  //cout << Enable::CDB << endl;
  int verbosity = 0;
  //string filename = "output/evt/events_"+tag+(tag==""?"":"_"); // edited to put in tg location
  string filename = "/sphenix/tg/tg01/commissioning/CaloCalibWG/egm2153/detdeta_run24auau/events_"+tag+(tag==""?"":"_"); 
  //string filename = "events_"+tag+(tag==""?"":"_");
  string dattag;
  switch(datormc) {
    case 0: dattag = "data"; break;
    case 1: dattag = "mc"; break;
    case 2: dattag = "epos"; break;
    case 3: dattag = "ampt"; break;
    case 6: dattag = "pedestal"; break;
    default: dattag = ""; std::cout << "data or mc generator value not valid" << std::endl; break;
  }
  string cortag = (correct?"cor":"unc");
  filename += dattag + "_"+cortag+"_" + to_string(nproc);
  filename += ".root";
  std::cout << filename << std::endl;
  std::cout << nproc << std::endl;
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
  if (datormc == 6) rc->set_uint64Flag("TIMESTAMP",54912);
  else if (datormc > 0 && datormc < 4) rc->set_uint64Flag("TIMESTAMP",14);
  else rc->set_uint64Flag("TIMESTAMP",runnumber);

  if (datormc == 0 || datormc == 6) { rc->set_StringFlag("CDB_GLOBALTAG","2024p009"); }
  else { rc->set_StringFlag("CDB_GLOBALTAG","MDC2"); } //"ProdA_2023");  "2023p007" 
  rc->set_IntFlag("RANDOMSEED",158804);

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
  std::ostringstream opedname;
  std::string pedestalname;

  if (datormc == 0) {
    string datafilelist = "/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_triggered_event_run2auau-000";
    datafilelist += to_string(runnumber) + ".list";
    list1.open(datafilelist);
  } else if (datormc == 1) {
    //list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_calo_nozero_hijing.list");
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_mbd_epd_hijing.list");
    list2.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_truth_hijing.list");
    //list3.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_global_hijing.list"); 
    list4.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/g4hits_hijing.list");
    //if (upweightb) { list5.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/g4hits_hijing.list"); }
  } else if (datormc == 2) {
    //list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_calo_cluster_epos.list");
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_mbd_epd_epos.list");
    list2.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_truth_epos.list");
    //list3.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_global_epos.list"); 
    list4.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/g4hits_epos.list");
  } else if (datormc == 3) {
    //list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_calo_nozero_ampt.list");
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_mbd_epd_ampt.list");
    list2.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_truth_ampt.list");
    //list3.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/dst_global_ampt.list"); 
    list4.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files_2024/g4hits_ampt.list");
  } else if (datormc == 4) {
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_hcal_pedestal_data.list");
  } else if (datormc == 5) {
    list1.open("/sphenix/user/egm2153/calib_study/detdeta/runsimana0/dst_files/dst_emcal_pedestal_data.list");
  } else if (datormc == 6) {
    opedname << "pedestal-54256-0" << std::setw(4) << std::setfill('0') << nproc << ".root";
    pedestalname = opedname.str();
  }

  if (datormc < 6) {
    for(int i=0; i<nproc+1; i++) {
      getline(list1, line1);
      if(datormc > 0 && datormc < 4) {
  	    getline(list4, line4);
  	    getline(list2, line2);
  	    //getline(list3, line3);
        //if (upweightb) { getline(list5, line5); }
  	  }
    }
  }

  Fun4AllInputManager *in_1 = new Fun4AllDstInputManager("DSTin1");
  Fun4AllInputManager *in_2 = new Fun4AllDstInputManager("DSTin2");
  //Fun4AllInputManager *in_3 = new Fun4AllDstInputManager("DSTin3");
  Fun4AllInputManager *in_4 = new Fun4AllDstInputManager("DSTin4");
  Fun4AllInputManager *in_5 = new Fun4AllDstInputManager("DSTin5");
  if (datormc < 6) {
    in_1->AddFile(line1);
    if(datormc > 0 && datormc < 4)
    {
      in_2->AddFile(line2);
      //in_3->AddFile(line3);
      in_4->AddFile(line4);
      //if (upweightb) { in_5->AddFile(line5); }
    }
  } else {
    in_1->AddFile(pedestalname);
  }
  se->registerInputManager( in_1 );
  if(datormc > 0 && datormc < 4)
    {
      se->registerInputManager(in_2);
      //se->registerInputManager(in_3);
      se->registerInputManager(in_4);
    }

  Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
  std::string geoLocation = CDBInterface::instance()->getUrl("calo_geo");
  intrue2->AddFile(geoLocation);
  se->registerInputManager(intrue2);

  if (datormc  == 0 || datormc > 3) {
    Process_Calo_Fitting(datormc);

    CaloFittingQA *ca = new CaloFittingQA("CaloFittingQA");
    if (datormc == 6) ca->set_simflag(true);
    se->registerSubsystem(ca);

    Process_Calo_Calib(datormc);
  }

  CDBInterface::instance()->Verbosity(1);

  EnergyCorrection *energycorrect;
  std::string emcal_energy_outfile = "/sphenix/tg/tg01/commissioning/CaloCalibWG/egm2153/detdeta_run24auau/EMCal_energy_" + tag + "_" + to_string(nproc);
  std::string ihcal_energy_outfile = "/sphenix/tg/tg01/commissioning/CaloCalibWG/egm2153/detdeta_run24auau/IHCal_energy_" + tag + "_" + to_string(nproc);
  std::string ohcal_energy_outfile = "/sphenix/tg/tg01/commissioning/CaloCalibWG/egm2153/detdeta_run24auau/OHCal_energy_" + tag + "_" + to_string(nproc);
  if (datormc == 1) {
    emcal_energy_outfile += "_rw_hijing.root";
    ihcal_energy_outfile += "_rw_hijing.root";
    ohcal_energy_outfile += "_rw_hijing.root";
  } else if (datormc == 2) {
    emcal_energy_outfile += "_rw_epos.root";
    ihcal_energy_outfile += "_rw_epos.root";
    ohcal_energy_outfile += "_rw_epos.root";
  } else if (datormc == 3) {
    emcal_energy_outfile += "_rw_ampt.root";
    ihcal_energy_outfile += "_rw_ampt.root";
    ohcal_energy_outfile += "_rw_ampt.root";
  }

  if(upweightb)
    {
      energycorrect = new EnergyCorrection("EnergyCorrectEMCal", emcal_energy_outfile.c_str());
      energycorrect->Verbosity(0);
      if (datormc == 1) { energycorrect->SetGeneratorType("HIJING"); }
      else if (datormc == 2) { energycorrect->SetGeneratorType("EPOS"); }
      else if (datormc == 3) { energycorrect->SetGeneratorType("AMPT"); }
      energycorrect->SetHitNodeName("G4HIT_CEMC");
      energycorrect->SetMinEta(-2.5); 
      energycorrect->SetMaxEta(2.5); 
      energycorrect->SetRapidityDep(false);
      energycorrect->SetUpweightTruth(false);
      if(upweightb && datormc) se->registerSubsystem(energycorrect);
      
      energycorrect = new EnergyCorrection("EnergyCorrectIHCal", ihcal_energy_outfile.c_str());
      energycorrect->Verbosity(0);
      if (datormc == 1) { energycorrect->SetGeneratorType("HIJING"); }
      else if (datormc == 2) { energycorrect->SetGeneratorType("EPOS"); }
      else if (datormc == 3) { energycorrect->SetGeneratorType("AMPT"); }
      energycorrect->SetHitNodeName("G4HIT_HCALIN");
      energycorrect->SetMinEta(-2.5); 
      energycorrect->SetMaxEta(2.5); 
      energycorrect->SetRapidityDep(false);
      energycorrect->SetUpweightTruth(false);
      if(upweightb && datormc) se->registerSubsystem(energycorrect);
      
      energycorrect = new EnergyCorrection("EnergyCorrectOHCal", ohcal_energy_outfile.c_str());
      energycorrect->Verbosity(0);
      if (datormc == 1) { energycorrect->SetGeneratorType("HIJING"); }
      else if (datormc == 2) { energycorrect->SetGeneratorType("EPOS"); }
      else if (datormc == 3) { energycorrect->SetGeneratorType("AMPT"); }
      energycorrect->SetHitNodeName("G4HIT_HCALOUT");
      energycorrect->SetMinEta(-2.5); 
      energycorrect->SetMaxEta(2.5); 
      energycorrect->SetRapidityDep(false);
      energycorrect->SetUpweightTruth((doupweight?true:false)); //only want to upweight the truth once
      if(upweightb && datormc) se->registerSubsystem(energycorrect);
    }

  if (upweightb || (datormc > 0 && datormc < 4)) {
    //se->registerInputManager(in_5);
    TRandom3 randGen;
    // get seed
    unsigned int seed = PHRandomSeed();
    randGen.SetSeed(seed);
    // a int from 0 to 3259
    int sequence = randGen.Integer(3260);
    // pad the name
    std::ostringstream opedfilename;
    opedfilename << "pedestal-54256-0" << std::setw(4) << std::setfill('0') << sequence << ".root";
    std::string pedestalfilename = opedfilename.str();

    Fun4AllInputManager *hitsin = new Fun4AllNoSyncDstInputManager("DST2");
    hitsin->AddFile(pedestalfilename);
    hitsin->Repeat();
    se->registerInputManager(hitsin);

    Enable::CEMC_TOWERINFO = true;
    Enable::HCALIN_TOWERINFO = true;
    Enable::HCALOUT_TOWERINFO = true;

    CEMC_Towers();
    HCALInner_Towers();
    HCALOuter_Towers();

    Process_Calo_Calib(datormc);

  }
                                   
// The calibrations have a validity range set by the beam clock which is not read out of the prdfs as of now
  MbdDigitization* mbddigi;
  MbdReco* mbdreco;
  if(datormc > 0 && datormc < 4)
    {
      mbdreco = new MbdReco();
      mbddigi = new MbdDigitization();
      se->registerSubsystem(mbddigi);
      se->registerSubsystem(mbdreco);

      GlobalVertexReco *gvertex = new GlobalVertexReco();
      se->registerSubsystem(gvertex);
    }

  if (datormc == 0) {
    mbdreco = new MbdReco();
    se->registerSubsystem(mbdreco);

    GlobalVertexReco *gvertex = new GlobalVertexReco();
    se->registerSubsystem(gvertex);
  }
  
  
  if (datormc == 0) {
    ZdcReco *zdcreco = new ZdcReco();
    zdcreco->set_zdc1_cut(0.0);
    zdcreco->set_zdc2_cut(0.0);
    se->registerSubsystem(zdcreco);

    MinimumBiasClassifier *mb = new MinimumBiasClassifier();
    mb->Verbosity(verbosity);
    mb->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/scales/cdb_centrality_scale_54912.root"); // will change run by run
    mb->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_54912.root"); // will change run by run
    se->registerSubsystem(mb);
  }
  
  if (datormc == 0) {
  CentralityReco* cent = new CentralityReco();
  //cent->Verbosity(2);
  cent->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/scales/cdb_centrality_scale_54912.root"); // will change run by run
  cent->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_54912.root"); // will change run by run
  cent->setOverwriteDivs("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/divs/cdb_centrality_54912.root");
  se->registerSubsystem( cent ); 
  }

  //if(!datormc) {
  //  Register_Tower_sys();
  //}
  
  // option for testing new emcal calibrations with direct URL instead of CDB
  int dataormc = datormc;
  if (datormc == 4) dataormc = 0;
  if (datormc == 5) dataormc = 0;
  if (datormc == 6) dataormc = 0;
  if (datormc == 2) dataormc = 1;
  if (datormc == 3) dataormc = 1;
  MDCTreeMaker *tt = new MDCTreeMaker("MDCTreeMaker", filename, dataormc, debug, correct, emcal_node, ihcal_node, ohcal_node);
  if (datormc == 6) tt->set_useMBD(false); 
  se->registerSubsystem( tt ); 
  se->Print("NODETREE");
  se->run(nevt);
  cout << "Ran all events" << endl;
  se->End();
  cout << "Ended server" << endl;
  string outfile_hist = "/sphenix/tg/tg01/commissioning/CaloCalibWG/egm2153/detdeta_run24auau/hist_events_" + tag + "_" + to_string(runnumber) + "_" + to_string(nproc) + ".root";
  if (!dataormc) QAHistManagerDef::saveQARootFile(outfile_hist.c_str());
  delete se;
  cout << "Deleted server" << endl;
  gSystem->Exit(0);
  cout << "Exited gSystem" << endl;
  return 0;

}
