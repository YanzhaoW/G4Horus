#pragma once

#include "G4UImessenger.hh"

class CascadeGenerator;
class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;


class CascadeMessenger: public G4UImessenger
{
public:
    void SetNewValue(G4UIcommand*, G4String);
    CascadeMessenger(CascadeGenerator*);
    ~CascadeMessenger();
  
private:
	CascadeGenerator*  fPriGen;
	G4UIdirectory*                  fDir;
	G4UIcmdWithABool*               fmode;
    G4UIcmdWithADoubleAndUnit*      fEnergy;
    G4UIcmdWithAString*             fPath;
};
