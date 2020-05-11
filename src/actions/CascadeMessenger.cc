#include "CascadeMessenger.hh"
#include "PrimaryGeneratorActionCascade.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

CascadeMessenger::CascadeMessenger(PrimaryGeneratorActionCascade* PriGen):fPriGen(PriGen){
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

    G4cout << "PATH.>>>>>>>>>>>>>>>><<<<<<<<<<<<<<" << G4endl;
}

CascadeMessenger::~CascadeMessenger(){
    delete fDir;
    delete fmode;
    delete fEnergy;
    delete fPath;

}

void CascadeMessenger::SetNewValue(G4UIcommand* command, G4String newValues){
    
    if(command==fmode){
        // fPriGen->SetMode(fmode->GetNewBoolValue(newValues));
    }
    else if(command==fEnergy){
        // fPriGen->SetEnergy(fEnergy->GetNewDoubleValue(newValues));
    }
    else if(command==fPath){
        fPriGen->SetPath(newValues);
        
    }
}