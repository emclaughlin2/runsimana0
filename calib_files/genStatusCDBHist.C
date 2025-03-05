#include "cdbHistConv.C"


void genStatusCDBHist(){
        TFile* fout = new TFile("ihcal_hist_tsc_cos_calib_old_mc_digi_scale.root","RECREATE");
        TH2F *h1 = CaloCDBTreeToHist("ihcal_cdb_tsc_cos_calib.root", "ihcal_abscalib_mip", 1);
        fout->cd();
        h1->Write();
        fout->Close();

}