import ROOT

# Open the ROOT files
file1 = ROOT.TFile.Open("ihcal_cdb_tsc_cos_calib.root", "READ")
file2 = ROOT.TFile.Open("HCALIN_calib_ADC_to_ETower_ana450_2024p009_old_mc_digi_scale_54912.root", "READ")

# Get the TTree from both files
tree1 = file1.Get("Multiple")
tree2 = file2.Get("Multiple")

# Create a histogram with 1000 bins from 0 to 0.01
hist = ROOT.TH1F("ratio_hist", "HCALIN_calib_ADC_to_ETower / Fihcal_abscalib_mip", 1000, 1.02, 1.04)

entries1 = tree1.GetEntries()
entries2 = tree2.GetEntries()

if entries1 != entries2:
    print("Error: Trees have different numbers of entries!")
else:
    # Loop over the entries and compute the ratio
    for i in range(entries1):
        tree1.GetEntry(i)
        tree2.GetEntry(i)
        # Compute the ratio
        if tree1.Fihcal_abscalib_mip != 0:  # Avoid division by zero
            ratio = tree2.FHCALIN_calib_ADC_to_ETower / tree1.Fihcal_abscalib_mip
            hist.Fill(ratio)

# Save histogram to a new ROOT file
out_file = "HCALIN_calib_scale_validation.root"
output_file = ROOT.TFile(out_file, "RECREATE")
hist.Write()
output_file.Close()

# Close input files
file1.Close()
file2.Close()

print(f"Histogram saved to {out_file}")
