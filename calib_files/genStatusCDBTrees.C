#include "cdbHistConv.C"

void genStatusCDBTrees(){
      //TFile* fin = new TFile("hist_new_calib_12_12_24_trig10_events_ana450_2024p009_54912.root");
      //TFile* fin = new TFile("hist_events_ana450_2024p009_54911_fixed_build_54911.root");
      TFile* fin = new TFile("ihcal_hist_tsc_cos_calib_old_mc_digi_scale.root");

      std::string tag = "ana450_2024p009_old_mc_digi_scale";
      std::string runNumber = "54912";

      cout << "doing " << tag.c_str() << "  " << runNumber.c_str() << endl;
      string payloadName;
      
      
      // HCALIN 
      string detector = "HCALIN";
      TH2F* h1 = (TH2F*) fin->Get("h_temp");
      if (h1) {
        payloadName = detector + "_calib_ADC_to_ETower"+ "_" + tag + "_" + runNumber +".root";
        histToCaloCDBTree(payloadName, "HCALIN_calib_ADC_to_ETower", 1, h1);
      }
      
      /*
      // HCALOUT 
      string detector = "HCALOUT";
      TH2F* h1 = (TH2F*) fin->Get("h_temp");
      if (h1) {
        payloadName = detector + "_calib_ADC_to_ETower"+ "_" + tag + "_" + runNumber +".root";
        histToCaloCDBTree(payloadName, "HCALOUT_calib_ADC_to_ETower", 1, h1);
      } 
      */

      fin->Close();

}
