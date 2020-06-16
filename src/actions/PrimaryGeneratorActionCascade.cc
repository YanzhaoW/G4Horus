#include "PrimaryGeneratorActionCascade.hh"
#include "PrimaryGeneratorCascade.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorActionCascade::PrimaryGeneratorActionCascade()
:fAct()
{
}

void PrimaryGeneratorActionCascade::GeneratePrimaries(G4Event *event)
{
    fAct.GeneratePrimaryVertex(event);
}
