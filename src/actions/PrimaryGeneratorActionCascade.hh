#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "PrimaryGeneratorCascade.hh"
#include "CascadeMessenger.hh"


class G4Event;

class PrimaryGeneratorActionCascade : public G4VUserPrimaryGeneratorAction {
  public:
    PrimaryGeneratorActionCascade();
    ~PrimaryGeneratorActionCascade();
    void GeneratePrimaries(G4Event* event) override;
    void SetMode(G4bool Mod){fMode = Mod;}
    void SetPath(G4String file){fPath = file;}

  private:
    PrimaryGeneratorCascade fAct;
    bool flag = true;
    CascadeMessenger* fMessenger;
    G4bool fMode;
    G4String fPath;
};