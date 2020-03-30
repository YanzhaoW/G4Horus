#pragma once

#include "G4VPrimaryGenerator.hh"
#include <map>
#include <vector>

class G4ParticleDefinition;
class G4Event;

class PrimaryGeneratorCascade : public G4VPrimaryGenerator {
  public:
  struct Decay {
    G4double energy;
    G4double intensity;
    std::string ini_level;
    std::string final_level; 
  };
  using LevelScheme = std::vector<Decay>;
  PrimaryGeneratorCascade(const LevelScheme&, const G4double&);
  void GeneratePrimaryVertex(G4Event*) override;
  void ParticleGun(G4Event*, G4double);
  void Decay_UpDownward(G4Event *event, G4double E, std::string mode);
  LevelScheme fLevelScheme;

  private:
  const G4ParticleDefinition* fGamma;
  G4double fEnergy;
};