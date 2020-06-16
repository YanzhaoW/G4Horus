#pragma once

#include "G4UImessenger.hh"

class PrimaryGeneratorCascade;
class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;


class CascadeMessenger: public G4UImessenger
{
public:
    void SetNewValue(G4UIcommand*, G4String);
    CascadeMessenger(PrimaryGeneratorCascade*);
    ~CascadeMessenger();
  
private:
	PrimaryGeneratorCascade*  fPriGen;
	G4UIdirectory*                  fDir;
	G4UIcmdWithABool*               fmode;
    G4UIcmdWithADoubleAndUnit*      fEnergy;
    G4UIcmdWithAString*             fPath;
};