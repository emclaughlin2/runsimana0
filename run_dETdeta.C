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
#include <g4mbd/MbdDigitization.h>
#include <mbd/MbdReco.h>
#include <frog/FROG.h>
#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <centrality/CentralityReco.h>
#include <calotrigger/MinimumBiasClassifier.h>
#include <G4Setup_sPHENIX.C>
#include <energycorrection/EnergyCorrection.h>

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
  cout << Enable::CDB << endl;
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
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity( verbosity );
  // just if we set some flags somewhere in this macro
  recoConsts *rc =  recoConsts::instance();
  rc->set_uint64Flag("TIMESTAMP",runnumber);
  if (datormc == 0) { rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2023"); }
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

  if(datormc > 0 && !upweightb)
    {
      se->registerInputManager(in_2);
      se->registerInputManager(in_3);
      se->registerInputManager(in_4);
    }


  EnergyCorrection *energycorrect;
  if(upweightb)
    {
      energycorrect = new EnergyCorrection();
      energycorrect->SetHitNodeName("G4HIT_CEMC");
      //only want to upweight the truth once
      energycorrect->SetUpWeightTruth((doupweight?true:false));
      if(upweightb && datormc) se->registerSubsystem(energycorrect);
      energycorrect = new EnergyCorrection();
      energycorrect->SetHitNodeName("G4HIT_HCALIN");
      if(upweightb && datormc) se->registerSubsystem(energycorrect);
      energycorrect->SetUpWeightTruth(false);
      energycorrect = new EnergyCorrection();
      energycorrect->SetHitNodeName("G4HIT_HCALOUT");
      energycorrect->SetUpWeightTruth(false);
      if(upweightb && datormc) se->registerSubsystem(energycorrect);
    }
  /*
    PHG4CylinderCellReco *cemc_cells =
        new PHG4CylinderCellReco("CEMCCYLCELLRECO");
    cemc_cells->Detector("CEMC");
    for (int i = G4CEMC::Min_cemc_layer; i <= G4CEMC::Max_cemc_layer; i++) {
      const double radius = 95;
      cemc_cells->cellsize(i, 2 * M_PI / 256. * radius, 2 * M_PI / 256. *
    radius);
    }
    cemc_cells->Verbosity(1);
    se->registerSubsystem(cemc_cells);
  */

  PHG4FullProjSpacalCellReco *cemc_cells;
  if(upweightb)
    {
      
      cemc_cells = new PHG4FullProjSpacalCellReco("CEMCCYLCELLRECO");
      cemc_cells->Detector("CEMC");
      cemc_cells->Verbosity(1);
      cemc_cells->get_light_collection_model().load_data_file(
							      string(getenv("CALIBRATIONROOT")) +
							      string("/CEMC/LightCollection/Prototype3Module.xml"),
							      "data_grid_light_guide_efficiency", "data_grid_fiber_trans");
      cemc_cells->checkenergy();
      if(upweightb && datormc) se->registerSubsystem(cemc_cells);
    }

  PHG4HcalCellReco *ohc;
  PHG4HcalCellReco *ihc;
  if(upweightb)
    {
      ihc = new PHG4HcalCellReco("HCALIN_CELLRECO");
      ohc = new PHG4HcalCellReco("HCALOUT_CELLRECO");
      ihc->Detector("HCALIN");
      ihc->Verbosity(0);
      if(upweightb && datormc) se->registerSubsystem(ihc);


      ohc->Detector("HCALOUT");
      ohc->Verbosity(1);
      if(upweightb && datormc) se->registerSubsystem(ohc);
    }
  HcalRawTowerBuilder *OTowerBuilder;
  RawTowerBuilder *ETowerBuilder;
  HcalRawTowerBuilder *ITowerBuilder;
  RawTowerDigitizer *OHTowerDigitizer;
  RawTowerDigitizer *IHTowerDigitizer;
  RawTowerDigitizer *EMTowerDigitizer;
  RawTowerCalibration *EMTowerCalibration;
  RawTowerCalibration *IHTowerCalibration;
  RawTowerCalibration *OHTowerCalibration;
  if(upweightb)
    {
      ETowerBuilder = new RawTowerBuilder("EmcRawTowerBuilder");
      ETowerBuilder->Detector("CEMC");
      ETowerBuilder->set_sim_tower_node_prefix("SIM");
      ETowerBuilder->Verbosity(0);
      if(upweightb && datormc) se->registerSubsystem(ETowerBuilder);
      ETowerBuilder->checkenergy();
      ITowerBuilder = new HcalRawTowerBuilder("HcalInRawTowerBuilder");
      ITowerBuilder->Detector("HCALIN");
      ITowerBuilder->set_sim_tower_node_prefix("SIM");
      if (!isfinite(G4HCALIN::phistart)) {
	G4HCALIN::phistart =
	  0.0445549893; // offet in phi (from zero) extracted from geantinos
      }
      ITowerBuilder->set_double_param("phistart", G4HCALIN::phistart);
      if (isfinite(G4HCALIN::tower_emin)) {
	ITowerBuilder->set_double_param("emin", G4HCALIN::tower_emin);
      }
      if (G4HCALIN::tower_energy_source >= 0) {
	ITowerBuilder->set_int_param("tower_energy_source",
				     G4HCALIN::tower_energy_source);
      }
      ITowerBuilder->Verbosity(0);
      if(upweightb && datormc) se->registerSubsystem(ITowerBuilder);
      
      OTowerBuilder = new HcalRawTowerBuilder("HcalOutRawTowerBuilder");
      OTowerBuilder->Detector("HCALOUT");
      OTowerBuilder->set_sim_tower_node_prefix("SIM");
      if (!isfinite(G4HCALOUT::phistart)) {
	G4HCALOUT::phistart =
	  0.0240615415; // offet in phi (from zero) extracted from geantinos
      }
      OTowerBuilder->set_double_param("phistart", G4HCALOUT::phistart);
      if (isfinite(G4HCALOUT::tower_emin)) {
	OTowerBuilder->set_double_param("emin", G4HCALOUT::tower_emin);
      }
      if (G4HCALOUT::tower_energy_source >= 0) {
	OTowerBuilder->set_int_param("tower_energy_source",
				     G4HCALOUT::tower_energy_source);
      }
      OTowerBuilder->Verbosity(0);
      if(upweightb && datormc) se->registerSubsystem(OTowerBuilder);

      
      OHTowerDigitizer = new RawTowerDigitizer("HcalOutRawTowerDigitizer");
      //OHTowerDigitizer->set_variable_pedestal(true);
      OHTowerDigitizer->set_pedstal_width_ADC(0);
      OHTowerDigitizer->Detector("HCALOUT");
      //  TowerDigitizer->set_raw_tower_node_prefix("RAW_LG");
      OHTowerDigitizer->set_digi_algorithm(G4HCALOUT::TowerDigi);
      OHTowerDigitizer->set_pedstal_central_ADC(0);
      //OHTowerDigitizer->set_pedstal_width_ADC(0);  // From Jin's guess. No EMCal High Gain data yet! TODO: update
      OHTowerDigitizer->set_photonelec_ADC(16. / 5.);
      OHTowerDigitizer->set_photonelec_yield_visible_GeV(16. / 5 / (0.2e-3));
      OHTowerDigitizer->set_zero_suppression_ADC(-999);                  // no-zero suppression
      OHTowerDigitizer->Verbosity(verbosity);
      if (!Enable::HCALOUT_G4Hit) OHTowerDigitizer->set_towerinfo(RawTowerDigitizer::ProcessTowerType::kTowerInfoOnly);  // just use towerinfo
      se->registerSubsystem(OHTowerDigitizer);
      const double visible_sample_fraction_HCALOUT = 3.38021e-02;  // /gpfs/mnt/gpfs04/sphenix/user/jinhuang/prod_analysis/hadron_shower_res_nightly/./G4Hits_sPHENIX_pi-_eta0_16GeV.root_qa.rootQA_Draw_HCALOUT_G4Hit.pdf


      IHTowerDigitizer = new RawTowerDigitizer("HcalInRawTowerDigitizer");
      //IHTowerDigitizer->set_variable_pedestal(false);
      IHTowerDigitizer->Detector("HCALIN");
      //  TowerDigitizer->set_raw_tower_node_prefix("RAW_LG");
      IHTowerDigitizer->set_digi_algorithm(G4HCALIN::TowerDigi);
      IHTowerDigitizer->set_pedstal_central_ADC(0);
      IHTowerDigitizer->set_pedstal_width_ADC(0);  // From Jin's guess. No EMCal High Gain data yet! TODO: update
      IHTowerDigitizer->set_photonelec_ADC(32. / 5.);
      IHTowerDigitizer->set_photonelec_yield_visible_GeV(32. / 5 / (0.4e-3));
      IHTowerDigitizer->set_zero_suppression_ADC(-999);                 // no-zero suppression
      IHTowerDigitizer->Verbosity(verbosity);
      if (!Enable::HCALIN_G4Hit) IHTowerDigitizer->set_towerinfo(RawTowerDigitizer::ProcessTowerType::kTowerInfoOnly);  // just use towerinfo
      se->registerSubsystem(IHTowerDigitizer);

      // Default sampling fraction for SS310
      double visible_sample_fraction_HCALIN = 0.0631283;                                //, /gpfs/mnt/gpfs04/sphenix/user/jinhuang/prod_analysis/hadron_shower_res_nightly/./G4Hits_sPHENIX_pi-_eta0_16GeV-0000.root_qa.rootQA_Draw_HCALIN_G4Hit.pdf

      if (G4HCALIN::inner_hcal_material_Al) visible_sample_fraction_HCALIN = 0.162166;  // for "G4_Al", Abhisek Sen <sen.abhisek@gmail.com>

      float sampling_fraction = 2e-02;                 // 2017 Tilt porjective SPACAL, tower-by-tower calibration
      const double photoelectron_per_GeV = 500;  // 500 photon per total GeV deposition

      RawTowerDigitizer *EMTowerDigitizer = new RawTowerDigitizer("EmcRawTowerDigitizer");
      //EMTowerDigitizer->set_pedstal_width_ADC(0);
      EMTowerDigitizer->Detector("CEMC");
      EMTowerDigitizer->Verbosity(verbosity);
      EMTowerDigitizer->set_digi_algorithm(G4CEMC::TowerDigi);
      EMTowerDigitizer->set_variable_pedestal(true);  // read ped central and width from calibrations file comment next 2 lines if true
      //EMTowerDigitizer->set_pedstal_central_ADC(0);
      //EMTowerDigitizer->set_pedstal_width_ADC(0);  // eRD1 test beam setting
      EMTowerDigitizer->set_photonelec_ADC(1);                // not simulating ADC discretization error
      EMTowerDigitizer->set_photonelec_yield_visible_GeV(photoelectron_per_GeV / sampling_fraction);
      EMTowerDigitizer->set_variable_zero_suppression(false);  // read zs values from calibrations file comment next line if true
      EMTowerDigitizer->set_zero_suppression_ADC(-999);  // eRD1 test beam setting
      if (!Enable::CEMC_G4Hit) EMTowerDigitizer->set_towerinfo(RawTowerDigitizer::ProcessTowerType::kTowerInfoOnly);  // just use towerinfo
      //EMTowerDigitizer->GetParameters().ReadFromCDB("EMCTOWERCALIB");
      
      if (Enable::CDB)
	{
	  EMTowerDigitizer->GetParameters().ReadFromCDB("EMCTOWERCALIB");
	}
      else
	{
	  EMTowerDigitizer->GetParameters().ReadFromFile("CEMC", "xml", 0, 0,
						       string(getenv("CALIBRATIONROOT")) + string("/CEMC/TowerCalibCombinedParams_2020/"));  // calibration database
	}
      
      se->registerSubsystem(EMTowerDigitizer);


      

      RawTowerCalibration *OHTowerCalibration = new RawTowerCalibration("HcalOutRawTowerCalibration");
      OHTowerCalibration->Detector("HCALOUT");
      //  TowerCalibration->set_raw_tower_node_prefix("RAW_LG");
      //  TowerCalibration->set_calib_tower_node_prefix("CALIB_LG");
      OHTowerCalibration->set_calib_algorithm(RawTowerCalibration::kSimple_linear_calibration);
      if (G4HCALOUT::TowerDigi == RawTowerDigitizer::kNo_digitization)
	{
	  // 0.033 extracted from electron sims (edep(scintillator)/edep(total))
	  OHTowerCalibration->set_calib_const_GeV_ADC(1. / 0.033);
	}
      else
	{
	  OHTowerCalibration->set_calib_const_GeV_ADC(0.2e-3 / visible_sample_fraction_HCALOUT);
	}
      OHTowerCalibration->set_pedstal_ADC(0);
      OHTowerCalibration->Verbosity(verbosity);
      if (!Enable::HCALOUT_G4Hit) OHTowerCalibration->set_towerinfo(RawTowerCalibration::ProcessTowerType::kTowerInfoOnly);  // just use towerinfo
      se->registerSubsystem(OHTowerCalibration);

	RawTowerCalibration *IHTowerCalibration = new RawTowerCalibration("HcalInRawTowerCalibration");
      IHTowerCalibration->Detector("HCALIN");
      //  IHTowerCalibration->set_raw_tower_node_prefix("RAW_LG");
      //  IHTowerCalibration->set_calib_tower_node_prefix("CALIB_LG");
      IHTowerCalibration->set_calib_algorithm(RawTowerCalibration::kSimple_linear_calibration);
      if (G4HCALIN::TowerDigi == RawTowerDigitizer::kNo_digitization)
	{
	  // 0.176 extracted from electron sims (edep(scintillator)/edep(total))
	  IHTowerCalibration->set_calib_const_GeV_ADC(1. / 0.176);
	}
      else
	{
	  IHTowerCalibration->set_calib_const_GeV_ADC(0.4e-3 / visible_sample_fraction_HCALIN);
	}
      IHTowerCalibration->set_pedstal_ADC(0);
      IHTowerCalibration->Verbosity(verbosity);
      if (!Enable::HCALIN_G4Hit) IHTowerCalibration->set_towerinfo(RawTowerCalibration::ProcessTowerType::kTowerInfoOnly);  // just use towerinfo
      se->registerSubsystem(IHTowerCalibration);

      

      RawTowerCalibration *EMTowerCalibration = new RawTowerCalibration("EmcRawTowerCalibration");
      EMTowerCalibration->Detector("CEMC");
      EMTowerCalibration->Verbosity(verbosity);
      if (!Enable::CEMC_G4Hit) EMTowerCalibration->set_towerinfo(RawTowerCalibration::ProcessTowerType::kTowerInfoOnly);  // just use towerinfo
      if (G4CEMC::TowerDigi == RawTowerDigitizer::kNo_digitization)
	{
	  // just use sampling fraction set previously
	  EMTowerCalibration->set_calib_const_GeV_ADC(1.0 / sampling_fraction);
	}
      else
	{
	  EMTowerCalibration->set_calib_algorithm(RawTowerCalibration::kTower_by_tower_calibration);
	  //	  EMTowerCalibration->GetCalibrationParameters().ReadFromCDB("EMCTOWERCALIB");
	  if (Enable::CDB)
	    {
	      EMTowerCalibration->GetCalibrationParameters().ReadFromCDB("EMCTOWERCALIB");
	    }
	  else
	    {
	      EMTowerCalibration->GetCalibrationParameters().ReadFromFile("CEMC", "xml", 0, 0,
									string(getenv("CALIBRATIONROOT")) + string("/CEMC/TowerCalibCombinedParams_2020/"));  // calibration database
	    }
	  EMTowerCalibration->set_variable_GeV_ADC(true);                                                                                                     // read GeV per ADC from calibrations file comment next line if true
	  //    TowerCalibration->set_calib_const_GeV_ADC(1. / photoelectron_per_GeV / 0.9715);                                                             // overall energy scale based on 4-GeV photon simulations
	  EMTowerCalibration->set_variable_pedestal(true);  // read pedestals from calibrations file comment next line if true
	  //EMTowerCalibration->set_pedstal_ADC(0);
	}
      se->registerSubsystem(EMTowerCalibration);

	
    }
                                   
// The calibrations have a validity range set by the beam clock which is not read out of the prdfs as of now
  
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
  //Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");

  //CDBInterface *cdb = CDBInterface::instance();
  //std::string geoLocation = cdb->getUrl("calo_geo");
  //intrue2->AddFile(geoLocation);
  //se->registerInputManager(intrue2);

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
