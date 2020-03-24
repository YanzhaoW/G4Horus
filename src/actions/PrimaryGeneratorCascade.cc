#include "PrimaryGeneratorCascade.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleTable.hh"
#include "G4RandomDirection.hh"
#include "G4PrimaryVertex.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Event.hh"

PrimaryGeneratorCascade::PrimaryGeneratorCascade(const DecaySeq& Decays)
:fDecays(Decays),fGamma(G4ParticleTable::GetParticleTable()->FindParticle("gamma"))
{
}

void PrimaryGeneratorCascade::GeneratePrimaryVertex(G4Event *event){
    for(auto const& decay : fDecays){
        auto gamma = new G4PrimaryParticle(fGamma);
        gamma->SetMomentumDirection(G4RandomDirection());
        // gamma->SetKineticEnergy(decay.energy);
        gamma->SetKineticEnergy(decay);
        auto gammaVertex = new G4PrimaryVertex();
        gammaVertex->SetPrimary(gamma);
        event->AddPrimaryVertex(gammaVertex);
    }
}