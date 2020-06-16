#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "PrimaryGeneratorCascade.hh"



class G4Event;

class PrimaryGeneratorActionCascade : public G4VUserPrimaryGeneratorAction {
  public:
    PrimaryGeneratorActionCascade();
    void GeneratePrimaries(G4Event* event) override;
    

  private:
    PrimaryGeneratorCascade fAct;
    bool flag = true;
    
    
};