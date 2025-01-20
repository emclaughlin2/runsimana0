#ifndef CALO_FITTING_H
#define CALO_FITTING_H

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloWaveformProcessing.h>

#include <fun4all/Fun4AllServer.h>

R__LOAD_LIBRARY(libcalo_reco.so)

void Process_Calo_Fitting(int data = 0)
{
  Fun4AllServer *se = Fun4AllServer::instance();

  CaloTowerDefs::BuilderType buildertype;
  if (data == 0) { // beam data
    buildertype = CaloTowerDefs::kPRDFTowerv4;

    CaloTowerBuilder *caZDC = new CaloTowerBuilder("ZDCBUILDER");
    caZDC->set_detector_type(CaloTowerDefs::ZDC);
    caZDC->set_builder_type(buildertype);
    caZDC->set_processing_type(CaloWaveformProcessing::FAST);
    caZDC->set_nsamples(16);
    caZDC->set_offlineflag();
    se->registerSubsystem(caZDC);

    CaloTowerBuilder *ctbEMCal = new CaloTowerBuilder("EMCalBUILDER");
    ctbEMCal->set_detector_type(CaloTowerDefs::CEMC);
    ctbEMCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
    ctbEMCal->set_builder_type(buildertype);
    ctbEMCal->set_offlineflag(true);
    ctbEMCal->set_nsamples(12);
    ctbEMCal->set_softwarezerosuppression(true, 100);
    ctbEMCal->set_bitFlipRecovery(true);
    se->registerSubsystem(ctbEMCal);

    CaloTowerBuilder *ctbIHCal = new CaloTowerBuilder("HCALINBUILDER");
    ctbIHCal->set_detector_type(CaloTowerDefs::HCALIN);
    ctbIHCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
    ctbIHCal->set_builder_type(buildertype);
    ctbIHCal->set_offlineflag();
    ctbIHCal->set_nsamples(12);
    ctbIHCal->set_softwarezerosuppression(true, 50);
    ctbIHCal->set_bitFlipRecovery(true);
    se->registerSubsystem(ctbIHCal);

    CaloTowerBuilder *ctbOHCal = new CaloTowerBuilder("HCALOUTBUILDER");
    ctbOHCal->set_detector_type(CaloTowerDefs::HCALOUT);
    ctbOHCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
    ctbOHCal->set_builder_type(buildertype);
    ctbOHCal->set_offlineflag();
    ctbOHCal->set_nsamples(12);
    ctbOHCal->set_softwarezerosuppression(true, 50);
    ctbOHCal->set_bitFlipRecovery(true);
    se->registerSubsystem(ctbOHCal);

    CaloTowerBuilder *caEPD = new CaloTowerBuilder("SEPDBUILDER");
    caEPD->set_detector_type(CaloTowerDefs::SEPD);
    caEPD->set_builder_type(buildertype);
    caEPD->set_processing_type(CaloWaveformProcessing::FAST);
    caEPD->set_nsamples(12);
    caEPD->set_offlineflag();
    se->registerSubsystem(caEPD);
 
  } else if (data == 6) { // pedestal data
    buildertype = CaloTowerDefs::kWaveformTowerv2;
    
    CaloTowerBuilder *ctbEMCal = new CaloTowerBuilder("EMCalBUILDER");
    ctbEMCal->set_detector_type(CaloTowerDefs::CEMC);
    ctbEMCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
    ctbEMCal->set_builder_type(buildertype);
    ctbEMCal->set_nsamples(12);
    ctbEMCal->set_dataflag(false);
    ctbEMCal->set_inputNodePrefix("PEDESTAL_");
    ctbEMCal->set_softwarezerosuppression(true,100);
    ctbEMCal->set_bitFlipRecovery(true);
    se->registerSubsystem(ctbEMCal);

    CaloTowerBuilder *ctbIHCal = new CaloTowerBuilder("HCALINBUILDER");
    ctbIHCal->set_detector_type(CaloTowerDefs::HCALIN);
    ctbIHCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
    ctbIHCal->set_builder_type(buildertype);
    ctbIHCal->set_nsamples(12);
    ctbIHCal->set_dataflag(false);
    ctbIHCal->set_inputNodePrefix("PEDESTAL_");
    ctbIHCal->set_softwarezerosuppression(true, 50);
    ctbIHCal->set_bitFlipRecovery(true);
    se->registerSubsystem(ctbIHCal);

    CaloTowerBuilder *ctbOHCal = new CaloTowerBuilder("HCALOUTBUILDER");
    ctbOHCal->set_detector_type(CaloTowerDefs::HCALOUT);
    ctbOHCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
    ctbOHCal->set_builder_type(buildertype);
    ctbOHCal->set_nsamples(12);
    ctbOHCal->set_dataflag(false);
    ctbOHCal->set_inputNodePrefix("PEDESTAL_");
    ctbOHCal->set_softwarezerosuppression(true, 50);
    ctbOHCal->set_bitFlipRecovery(true);
    se->registerSubsystem(ctbOHCal);
  }
}

#endif
