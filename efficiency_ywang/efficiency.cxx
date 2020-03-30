/*
* Get Efficiency for FEP, SEP and DEP from root files created by Geant4
* Filenames are supposed to be formatted like <energy>.root (e.g. 100.root)
* Note: In a root macro, the function with the same name as the file will be executed
* usage: $ root -l -b -q 'effi.cxx("/scratch/jmayer/G4")'
* --> for ROOT Version 6 or higher <--
*/

#include <map>
#include <utility>
#include <vector>

const std::vector<TString> detectors = {"A0", "A1", "A2", "A3", "B0", "B1", "B2", "B3"};


using eff_vec = std::pair<vector<double>, vector<double>>;

int FileSize(const char *name)
{
    int size = 0;
    if (FILE *file = fopen(name, "r")) {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fclose(file);
    }
    return size;
}

void Import_eff(map<TString, TGraphErrors*>& Ex_eff){
    for(auto const& det: detectors){
        Ex_eff[det]= new TGraphErrors("./efficiencies_10cm/"+det+"_Ra.dat","%lg %lg %lg");
    }
}

void  TransformToTGraph(std::map<TString, eff_vec>& efficiencies, std::map<TString, TGraph*>&Sim_eff ){
    for(auto const& det: detectors){
        Sim_eff[det]= new TGraph(efficiencies[det].first.size(), &efficiencies[det].first[0], &efficiencies[det].second[0]);
    }
}

void plot(map<TString, TGraph*> & Sim_eff, map<TString, TGraphErrors*>& Ex_eff){
    for(auto const& det: detectors){
        TCanvas canvas;
        Sim_eff[det]->SetTitle(det);
        Sim_eff[det]->SetMarkerColor(4);
        Sim_eff[det]->SetMarkerStyle(3);
        Sim_eff[det]->Draw();
        Sim_eff[det]->GetYaxis()->SetRangeUser(0.0, 0.022);
        Sim_eff[det]->GetXaxis()->SetTitle("gamma E (keV)");
        Sim_eff[det]->GetYaxis()->SetTitle("efficiency");
        Ex_eff[det]->Draw("SAME");
        TLegend legend(0.56,0.7,0.9,0.9);
        legend.AddEntry(Sim_eff[det],"Simulated efficiencies","lp");
        legend.AddEntry(Ex_eff[det],"measured efficiencies","le");
        legend.Draw("SAME");
        canvas.SaveAs(det+".png");
    }
}

void calculate_ratio(std::map<TString, eff_vec> &efficiencies, std::map<TString, TGraphErrors*> &Ex_eff){
    TCanvas canvas;
    canvas.Draw();
    TLegend legend(0.7,0.7,0.9,0.9);
    legend.SetNColumns(2);
    int color=1;
    for(auto const & det:detectors){
        if(efficiencies[det].second.size()!=Ex_eff[det]->GetN()){
            cout << "ERROR for ratio calculation: energy doesn't match" << endl;
        }
        else{
            std::vector<double> ratio;
            cout << "det " << det << ": " << endl;
            for(unsigned int i = 0; i < efficiencies[det].second.size();i++){
                double x, y;
                Ex_eff[det]->GetPoint(i,x,y);
                cout << efficiencies[det].second[i]/y << "  ";
                ratio.push_back(efficiencies[det].second[i]/y);
            }
            cout << endl;
            auto ratioGra = new TGraph(ratio.size(), &efficiencies[det].first[0], &ratio[0]);
            ratioGra->SetLineColor(color++);
            legend.AddEntry(ratioGra, det,"l");
            if(det == "A0"){
                ratioGra->GetYaxis()->SetRangeUser(1.0, 5.0);
                ratioGra->GetXaxis()->SetTitle("gamma E (keV)");
                ratioGra->GetYaxis()->SetTitle("ratio");
                ratioGra->Draw("");
            }
            else{
                ratioGra->Draw("SAME");
            }
        }
    }
    legend.Draw("SAME");
    canvas.SaveAs("ratio.png");
}


inline int EtoBin(int E){
    return (int)20000*E/3000.0;
}

inline double PeakVolume(const TH1D &th, const int &pos)
{
    int width = 10;
    return 2. * th.Integral(pos - width, pos + width) - 1. * th.Integral(pos - 2 * width, pos + 2 * width);
}

inline double Efficiency(const TH1D &th, const int &pos)
{
    return PeakVolume(th, EtoBin(pos)) / (th.GetEntries() * 1.);
}

std::vector<TH1D> SpectraFromFile(const TString &directory, const TString &file)
{
    std::vector<TH1D> spectra;
    TFile *f = TFile::Open(directory + file);
    f->cd("histograms");
    TDirectory *histdir = gDirectory;
    TKey *key;
    TIter nextkey(histdir->GetListOfKeys());
    while ((key = (TKey *)nextkey())) {
        if (gROOT->GetClass(key->GetClassName())->InheritsFrom(TH1D::Class())) {
            spectra.push_back(*dynamic_cast<TH1D *>(key->ReadObj()));
        }
    }

    return spectra;
}

std::vector<TString> GeBGOPairNumbers(const std::vector<TString> &in)
{
    std::vector<TString> out;

    for (const TString &l1 : in) {
        if (l1.BeginsWith("Ge")) {
            for (const TString &l2 : in) {
                if (l2.BeginsWith("BGO") && TString(l2(3, 2)).EqualTo(TString(l1(2, 2)))) {
                    out.push_back(TString(l2(3, 2)));
                }
            }
        }
    }

    return out;
}

std::vector<TString> LeavesInTChain(TChain *t)
{
    std::vector<TString> leafnames;

    TLeaf *obj;
    TObjArrayIter nextleaf(t->GetListOfLeaves());
    while ((obj = (TLeaf *)nextleaf())) {
        leafnames.push_back(TString(obj->GetName()));
    }

    return leafnames;
}

std::vector<TString> EfficiencyFilesIn(const TString &dirname)
{
    const TString ext = ".root";
    std::vector<TString> m;

    TSystemDirectory dir(dirname, dirname);
    TList *files = dir.GetListOfFiles();
    if (files) {
        TSystemFile *file;
        TString fname;
        TIter next(files);
        while ((file = (TSystemFile *)next())) {
            fname = file->GetName();
            // Check if file is not a directory, is a root file, is not a partial (_t) root file, and is of resonable size (== finished)
            if (fname.Contains(ext)) {
                m.push_back(fname);
            }
        }
    }
    std::sort(m.begin(), m.end(), [](const TString &a, const TString &b) -> bool { return a.Atoi() < b.Atoi(); });
    return m;
}

void efficiency(const TString &directory)
{   
    eff_vec decay_eff(vector<double>(0),vector<double>(0));
    std::map<TString, eff_vec> efficiencies;
    std::map<TString, TGraphErrors*> Ex_eff;
    std::map<TString, TGraph*> Sim_eff;
    for(auto const &det:detectors){
        efficiencies[det] = decay_eff;
        Ex_eff[det] = nullptr;
        Sim_eff[det] = nullptr;
    }

    const auto files = EfficiencyFilesIn(directory);
    for (const auto &file : files) {
        int energy = file.Atoi();
        // cout << "# Data for E = " << energy << " keV from file " << file << ":" << endl;

        auto spectra = SpectraFromFile(directory, file);
        for (const TH1D &h : spectra) {
            // cout << h.GetName() << "\t" << energy << "\t" << Efficiency(h, energy) << endl;
            efficiencies[h.GetName()].first.push_back(energy);
            efficiencies[h.GetName()].second.push_back(Efficiency(h, energy));
        }
    }

    // for(auto const &det: detectors){
    //     cout << "detector: " << det << endl;
    //     for(int i=0; i < efficiencies[det].first.size(); i++){
    //         cout << "energy: " << efficiencies[det].first[i] << "eff: " << efficiencies[det].second[i] << endl;
    //     }
    // }

    Import_eff(Ex_eff);
    TransformToTGraph(efficiencies, Sim_eff);
    calculate_ratio(efficiencies,Ex_eff);
    plot(Sim_eff, Ex_eff);
}
