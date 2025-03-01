#ifndef CALO_CALIB_H
#define CALO_CALIB_H

#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloTowerStatus.h>
#include <caloreco/RawClusterBuilderTemplate.h>
#include <caloreco/RawClusterDeadHotMask.h>
#include <caloreco/RawClusterPositionCorrection.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <phool/recoConsts.h>

#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>  // for Fun4AllServer

#include <TSystem.h>  // for gSystem

R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libfun4allutils.so)

void Process_Calo_Calib(int data = 0)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  //////////////////////////////
  // set statuses on raw towers
  std::cout << "status setters" << std::endl;
  CaloTowerStatus *statusEMC = new CaloTowerStatus("CEMCSTATUS");
  statusEMC->set_detector_type(CaloTowerDefs::CEMC);
  statusEMC->set_time_cut(1);
  se->registerSubsystem(statusEMC);

  CaloTowerStatus *statusHCalIn = new CaloTowerStatus("HCALINSTATUS");
  statusHCalIn->set_detector_type(CaloTowerDefs::HCALIN);
  statusHCalIn->set_time_cut(2);
  se->registerSubsystem(statusHCalIn);

  CaloTowerStatus *statusHCALOUT = new CaloTowerStatus("HCALOUTSTATUS");
  statusHCALOUT->set_detector_type(CaloTowerDefs::HCALOUT);
  statusHCALOUT->set_time_cut(2);
  se->registerSubsystem(statusHCALOUT);

  if (data == 0 || data == 6) {
    std::cout << "using local calo_calib" << std::endl;
    ////////////////////
    // Calibrate towers
    std::cout << "Calibrating EMCal" << std::endl;
    CaloTowerCalib *calibEMC = new CaloTowerCalib("CEMCCALIB");
    calibEMC->set_detector_type(CaloTowerDefs::CEMC);
    calibEMC->setFieldName("Femc_datadriven_qm1_correction");
    calibEMC->set_directURL("/sphenix/user/egm2153/calib_study/emcal_calib_year1/ana450_2024p009_54912_54921/local_calib_copy_iter26.root");
    if (data == 0) calibEMC->set_directURL_ZScrosscalib("/sphenix/user/egm2153/calib_study/detdeta/analysis/Run2024/CEMC_ZSCrossCalib_ana450_2024p009_54912.root");
    se->registerSubsystem(calibEMC);

    std::cout << "Calibrating OHcal" << std::endl;
    CaloTowerCalib *calibOHCal = new CaloTowerCalib("HCALOUT");
    calibOHCal->set_detector_type(CaloTowerDefs::HCALOUT);
    //calibOHCal->setFieldName("ohcal_cosmic_calibration"); 
    //calibOHCal->set_directURL("/sphenix/user/hanpuj/HCalCosmics/offline/calibration_factor/ohcal_cosmic_calibration_12.root");
    calibOHCal->set_directURL("/sphenix/u/bseidlitz/work/macros/calibrations/calo/hcal_towerSlope_y2/tsc_cos_comb/AuAuOutput/ohcal_cdb_tsc_cos_calib.root");
    if (data == 0) calibOHCal->set_directURL_ZScrosscalib("/sphenix/user/egm2153/calib_study/detdeta/analysis/Run2024/HCALOUT_ZSCrossCalib_ana450_2024p009_54912.root");
    se->registerSubsystem(calibOHCal);

    std::cout << "Calibrating IHcal" << std::endl;
    CaloTowerCalib *calibIHCal = new CaloTowerCalib("HCALIN");
    calibIHCal->set_detector_type(CaloTowerDefs::HCALIN);
    //calibIHCal->setFieldName("ihcal_cosmic_calibration"); 
    //calibIHCal->set_directURL("/sphenix/user/hanpuj/HCalCosmics/offline/calibration_factor/ihcal_cosmic_calibration_4.root");
    calibIHCal->set_directURL("/sphenix/u/bseidlitz/work/macros/calibrations/calo/hcal_towerSlope_y2/tsc_cos_comb/AuAuOutput/ihcal_cdb_tsc_cos_calib.root");
    if (data == 0) calibIHCal->set_directURL_ZScrosscalib("/sphenix/user/egm2153/calib_study/detdeta/analysis/Run2024/HCALIN_ZSCrossCalib_ana450_2024p009_54912.root");
    se->registerSubsystem(calibIHCal);
  } else {
    ////////////////////
    // Calibrate towers
    std::cout << "Calibrating EMCal" << std::endl;
    CaloTowerCalib *calibEMC = new CaloTowerCalib("CEMCCALIB");
    calibEMC->set_detector_type(CaloTowerDefs::CEMC);
    se->registerSubsystem(calibEMC);

    std::cout << "Calibrating OHcal" << std::endl;
    CaloTowerCalib *calibOHCal = new CaloTowerCalib("HCALOUT");
    calibOHCal->set_detector_type(CaloTowerDefs::HCALOUT);
    se->registerSubsystem(calibOHCal);

    std::cout << "Calibrating IHcal" << std::endl;
    CaloTowerCalib *calibIHCal = new CaloTowerCalib("HCALIN");
    calibIHCal->set_detector_type(CaloTowerDefs::HCALIN);
    se->registerSubsystem(calibIHCal);
  }

  ////////////////
  // MC Calibration
  if (data > 0 && data < 4)
  {
    std::string MC_Calib = CDBInterface::instance()->getUrl("CEMC_MC_RECALIB");
    if (MC_Calib.empty())
    {
      std::cout << "No MC calibration found :( )" << std::endl;
      gSystem->Exit(0);
    }
    CaloTowerCalib *calibEMC_MC = new CaloTowerCalib("CEMCCALIB_MC");
    calibEMC_MC->set_detector_type(CaloTowerDefs::CEMC);
    calibEMC_MC->set_inputNodePrefix("TOWERINFO_CALIB_");
    calibEMC_MC->set_outputNodePrefix("TOWERINFO_CALIB_");
    calibEMC_MC->set_directURL(MC_Calib);
    calibEMC_MC->set_doCalibOnly(true);
    se->registerSubsystem(calibEMC_MC);
  }

  //////////////////
  // Clusters
  std::cout << "Building clusters" << std::endl;
  RawClusterBuilderTemplate *ClusterBuilder = new RawClusterBuilderTemplate("EmcRawClusterBuilderTemplate");
  ClusterBuilder->Detector("CEMC");
  ClusterBuilder->set_threshold_energy(0.070);  // for when using basic calibration
  std::string emc_prof = getenv("CALIBRATIONROOT");
  emc_prof += "/EmcProfile/CEMCprof_Thresh30MeV.root";
  ClusterBuilder->LoadProfile(emc_prof);
  ClusterBuilder->set_UseTowerInfo(1);  // to use towerinfo objects rather than old RawTower
  se->registerSubsystem(ClusterBuilder);

  // currently NOT included! 
  //std::cout << "Applying Position Dependent Correction" << std::endl;
  //RawClusterPositionCorrection *clusterCorrection = new RawClusterPositionCorrection("CEMC");
  //clusterCorrection->set_UseTowerInfo(1);  // to use towerinfo objects rather than old RawTower
 // se->registerSubsystem(clusterCorrection);

}

#endif
