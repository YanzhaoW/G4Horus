#include "CologneCloverSetup.hh"
#include "G4SystemOfUnits.hh"

CologneCloverSetup::CologneCloverSetup(G4LogicalVolume* theMother)
    : Setup{ theMother, { { "A", { 0., 0 } }, { "B", { 180. * deg, 0. } } } }
{
}
