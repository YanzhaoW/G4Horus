#include "PrimaryGeneratorActionCascade.hh"
#include "PrimaryGeneratorCascade.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

#include <string>
#include <fstream>

PrimaryGeneratorCascade::DecaySeq getDecays(){
    PrimaryGeneratorCascade::DecaySeq decays(0);
    std::ifstream myfile;

    std::string energy;
    std::string intensity;
    
    myfile.open("/data/ywang/Home/Geant4/G4Horus/scripts/list_cascade.txt");
    myfile >> energy;
    myfile >> intensity;
    while(myfile.good()){
        myfile >> energy;
        myfile >> intensity;
        decays.push_back({stod(energy)*keV, stod(intensity)});
    }
    myfile.close();
    return decays;
}

PrimaryGeneratorActionCascade::PrimaryGeneratorActionCascade()
:fAct(getDecays())
{
}

void PrimaryGeneratorActionCascade::GeneratePrimaries(G4Event *event)
{
    fAct.GeneratePrimaryVertex(event);
}
