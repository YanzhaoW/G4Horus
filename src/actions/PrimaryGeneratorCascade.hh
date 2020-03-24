#pragma once

#include "G4VPrimaryGenerator.hh"
#include <map>
#include <vector>

class G4ParticleDefinition;
class G4Event;

class PrimaryGeneratorCascade : public G4VPrimaryGenerator {
  public:
    // struct Decay {
    //     G4double energy;
    //     G4double intensity;
    // };

    // using DecaySeq = std::vector<Decay>;
    using DecaySeq = std::vector<G4double>;

    PrimaryGeneratorCascade(const DecaySeq&);
    void GeneratePrimaryVertex(G4Event*) override;

  private:
    const DecaySeq fDecays;
    const G4ParticleDefinition* fGamma;
};