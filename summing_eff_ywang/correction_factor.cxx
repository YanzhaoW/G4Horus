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
#include <fstream>



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

void correction_factor(const TString &directory)
{   
    const std::vector<TString> detectors = {"A0", "A1", "A2", "A3","B0", "B1", "B2", "B3"};
    using effs = std::pair<double, double>;

    effs decay_eff({0,0});
    std::map<int, effs> effs_E;
    std::map<TString, std::map<int, effs>> efficiencies;
    for(auto const &det:detectors){
        efficiencies[det] = effs_E;
    }

    const auto files = EfficiencyFilesIn(directory);
    for (const auto &file : files) {
        cout << file << endl;
        int energy = file.Atoi();
        cout << "# Data for E = " << energy << " keV from file " << file << ":" << endl;

        auto spectra = SpectraFromFile(directory, file);
        for (const TH1D &h : spectra) {
            // cout << h.GetName() << "\t" << energy << "\t" << Efficiency(h, energy) << endl;
            
            if(file == to_string(energy)+"_on.root"){
                efficiencies[h.GetName()][energy].first=Efficiency(h, energy);
            }
            else{
                efficiencies[h.GetName()][energy].second=Efficiency(h, energy);
            }
            // efficiencies[h.GetName()].first.push_back(energy);
            // efficiencies[h.GetName()].second.push_back(Efficiency(h, energy));
        }
    }

    std::ofstream myfile;
    myfile.open("factors.csv", std::ofstream::trunc);
    myfile<<"energy(keV)";
    for(auto const &det: detectors){
        myfile<<", " << det;
    }
    myfile<<"\n";
    for(const auto & e : efficiencies[detectors[0]]){
        myfile << e.first;
        for(auto const &det: detectors){
            myfile<<"," << efficiencies[det][e.first].second/efficiencies[det][e.first].first;
        }
        myfile << "\n";
    }
    myfile.close();
    for(auto const &det: detectors){
        std::cout << "detector: " << det << endl;
        for(const auto & elem : efficiencies[det]){
            std::cout <<elem.first << "\t"<< elem.second.second/elem.second.first << std::endl;
        }
    }
    std::cout << std::endl;
    std::cout << "factors.csv is created" << std::endl;
    std::cout << "correction factor successfully calcualted" << std::endl;

    // for(auto const &det: detectors){
    //     std::cout << "detector: " << det << endl;
    //     for(const auto & elem : efficiencies[det]){
    //         std::cout <<elem.first << "\t"<< elem.second.first <<"\t"<< elem.second.second << std::endl;
    //     }
    // }

    // Import_eff(Ex_eff);
}
