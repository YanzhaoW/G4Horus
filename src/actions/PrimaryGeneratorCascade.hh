#pragma once

#include "G4VPrimaryGenerator.hh"
#include <map>
#include <vector>
#include "G4SystemOfUnits.hh"
#include "CascadeMessenger.hh"

class G4ParticleDefinition;
class G4Event;

class PrimaryGeneratorCascade : public G4VPrimaryGenerator {
  public:
  struct Decay {
    G4double energy;
    G4double intensity;
    std::string ini_level;
    std::string final_level;
    G4double alpha;
    G4bool is_gamma;
  };
  using LevelScheme = std::vector<Decay>;

  public:
  PrimaryGeneratorCascade();
  ~PrimaryGeneratorCascade();
  void GeneratePrimaryVertex(G4Event*) override;
  void ParticleGun(G4Event*, G4double);
  void Decay_UpDownward(G4Event *event, LevelScheme::iterator it, std::string mode);
  G4bool is_gamma_e(LevelScheme::iterator it);
  LevelScheme::iterator GetItr(G4double E);
  void SetScheme(const LevelScheme levels){fLevelScheme = levels;}
  void SetMode(G4bool Mod){fMode = Mod;}
  void SetEnergy(G4double energy){fEnergy = energy;}

  private:
  const G4ParticleDefinition* fGamma;
  G4bool CasMode=true;
  G4double fEnergy;
  LevelScheme fLevelScheme;
  G4bool fMode;
  CascadeMessenger* fMessenger;
};