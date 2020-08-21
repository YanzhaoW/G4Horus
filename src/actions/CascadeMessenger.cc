#include "CascadeMessenger.hh"
#include "PrimaryGeneratorCascade.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

#include <string>
#include <fstream>
#include <experimental/filesystem>
#include <mutex>

namespace fs = std::experimental::filesystem;

PrimaryGeneratorCascade::LevelScheme getScheme(G4String FilePath){
    PrimaryGeneratorCascade::LevelScheme scheme(0);
    std::ifstream myfile;

    std::string energy;
    std::string intensity;
    std::string ini_level;
    std::string final_level;
    std::string alpha;
    std::string gamma;

    G4cout << "reading file " <<FilePath << "..." << G4endl;
    myfile.open(FilePath);
    getline(myfile,energy,',');
    getline(myfile,intensity,',');
    getline(myfile,ini_level,',');
    getline(myfile,final_level,',');
    getline(myfile,alpha,',');
    getline(myfile,gamma,'\n');
    while(myfile.good()){
        getline(myfile,energy,',');
        getline(myfile,intensity,',');
        getline(myfile,ini_level,',');
        getline(myfile,final_level,',');
        getline(myfile,alpha,',');
        getline(myfile,gamma,'\n');
        if(myfile.good()) scheme.push_back({stod(energy)*keV, stod(intensity), ini_level, final_level,stod(alpha),gamma=="1"});
    }
    myfile.close();

    if(scheme.size() == 0){
        G4cerr << "ERROR: FAILED TO READ: " << FilePath << G4endl;
        exit (EXIT_FAILURE);
    }
    else{
        G4cout << FilePath << " read successfuly" << G4endl;
        G4cout << "number of decays: " << scheme.size() << G4endl;
    }
    return scheme;
}

CascadeMessenger::CascadeMessenger(PrimaryGeneratorCascade* PriGen):fPriGen(PriGen){
    fDir = new G4UIdirectory("/cascade/");
    fDir -> SetGuidance("cascade information input");

    fmode = new G4UIcmdWithABool("/cascade/mode", this);
    fmode->SetGuidance("setting the cascade on or off");
    fmode->SetParameterName("mode", true);
    fmode->SetDefaultValue(true);

    fEnergy = new G4UIcmdWithADoubleAndUnit("/cascade/energy", this);
    fEnergy->SetGuidance("setting the energy of reference gamma ray");
    fEnergy->SetParameterName("E", true);
    fEnergy->SetDefaultUnit("keV");
    fEnergy->SetUnitCandidates("eV keV MeV GeV TeV");

    fPath = new G4UIcmdWithAString("/cascade/file", this);
    fPath->SetGuidance("setting the file of cascade list");
    fPath->SetParameterName("fFile",false);

    
}

CascadeMessenger::~CascadeMessenger(){
    delete fDir;
    delete fmode;
    delete fEnergy;
    delete fPath;

}

void CascadeMessenger::SetNewValue(G4UIcommand* command, G4String newValues){
    
    if(command==fmode){
        fPriGen->SetMode(fmode->GetNewBoolValue(newValues));
    }
    else if(command==fEnergy){
        fPriGen->SetEnergy(fEnergy->GetNewDoubleValue(newValues));
    }
    else if(command==fPath){
        fs::path p = std::string("./../../scripts/"+ newValues+ ".csv").c_str();
        fPriGen->SetScheme(getScheme(G4String(fs::canonical(p))));
    }
}