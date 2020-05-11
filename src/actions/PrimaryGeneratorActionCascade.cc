#include "PrimaryGeneratorActionCascade.hh"
#include "PrimaryGeneratorCascade.hh"
#include "CascadeMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

#include <string>
#include <fstream>

PrimaryGeneratorCascade::LevelScheme getScheme(){
    PrimaryGeneratorCascade::LevelScheme scheme(0);
    std::ifstream myfile;

    std::string energy;
    std::string ini_level;
    std::string final_level;
    std::string intensity;

    
    myfile.open("/data/ywang/Home/Geant4/G4Horus/scripts/Decay_Scheme_Sn111_Q2451.txt");
    myfile >> energy;
    myfile >> ini_level;
    myfile >> final_level;
    myfile >> intensity;
    while(myfile.good()){
        myfile >> energy;
        myfile >> ini_level;
        myfile >> final_level;
        myfile >> intensity;
        scheme.push_back({stod(energy)*keV, stod(intensity), ini_level, final_level});
    }
    myfile.close();

    return scheme;
}

PrimaryGeneratorActionCascade::PrimaryGeneratorActionCascade()
:fAct(372.31*keV)
{
    fMessenger = new CascadeMessenger(this);
    // G4cout << "PATH.>>>>>>>>>>>>>>>>"<<fPath<<"<<<<<<<<<<<<<<" << G4endl;
    fAct.SetScheme(getScheme());
}

PrimaryGeneratorActionCascade::~PrimaryGeneratorActionCascade(){
    delete fMessenger;
}

void PrimaryGeneratorActionCascade::GeneratePrimaries(G4Event *event)
{
    fAct.GeneratePrimaryVertex(event);
}
