#include "PrimaryGeneratorActionCascade.hh"
#include "PrimaryGeneratorCascade.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4SIunits.hh"


PrimaryGeneratorActionCascade::PrimaryGeneratorActionCascade()
:fAct({1000*keV})
{
    // PrimaryGeneratorCascade::DecaySeq decays(
    //     {{500*keV, 1.0},{200*keV, 1.0}, {100*keV, 1.0}}
    // );
    // PrimaryGeneratorCascade fAct(decays);
}

void PrimaryGeneratorActionCascade::GeneratePrimaries(G4Event *event)
{
    fAct.GeneratePrimaryVertex(event);
}
