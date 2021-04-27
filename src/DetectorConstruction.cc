#include "DetectorConstruction.hh"
#include "G4Box.hh"
#include "G4Material.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4NistManager.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"

#include "AstroTargetkammer.hh"
#include "AstroTargetkammerV2.hh"
#include "BGO.hh"
#include "CologneCloverSetup.hh"
#include "DetectorLibrary.hh"
#include "DummyChamber.hh"
#include "HPGeClover.hh"
#include "Horus.hh"
#include "PIPS.hh"
#include "SONIC.hh"
#include "SonicV3.hh"
#include "SonicV3dEE.hh"

extern const std::vector<std::string> detectors = {"A0", "A1", "A2", "A3", "B0", "B1", "B2", "B3"};

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
    // World
    const G4double worldSizeXYZ = 5. * m / 2;
    auto worldS = new G4Box("World", worldSizeXYZ, worldSizeXYZ, worldSizeXYZ);
    auto worldLV = new G4LogicalVolume(worldS, G4Material::GetMaterial("Galactic"), "World");
    worldLV->SetVisAttributes(G4VisAttributes::Invisible);

    auto clover = new CologneCloverSetup(worldLV);
    clover->PlaceDetector("CloziA", "A", 1.3 * cm);
    clover->PlaceDetector("CloziB", "B", 1.3 * cm);

    auto worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World", nullptr, false, 0, fCheckOverlaps);
    return worldPV;
}

DetectorConstruction::DetectorConstruction()
    : fCheckOverlaps(true)
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    DefineMaterials();
    return DefineVolumes();
}

void DetectorConstruction::DefineMaterials()
{
    G4NistManager* nistManager = G4NistManager::Instance();
    nistManager->FindOrBuildMaterial("G4_Pb");
    nistManager->FindOrBuildMaterial("G4_Cu");
    nistManager->FindOrBuildMaterial("G4_Al");
    nistManager->FindOrBuildMaterial("G4_Ge");
    nistManager->FindOrBuildMaterial("G4_Ta");
    nistManager->FindOrBuildMaterial("G4_Si");
    nistManager->FindOrBuildMaterial("G4_POLYCARBONATE");

    G4Element* O = nistManager->FindOrBuildElement("O");
    G4Element* Bi = nistManager->FindOrBuildElement("Bi");
    G4Element* Ge = nistManager->FindOrBuildElement("Ge");
    G4Element* C = nistManager->FindOrBuildElement("C");
    G4Element* H = nistManager->FindOrBuildElement("H");
    G4Element* Cu = nistManager->FindOrBuildElement("Cu");
    G4Element* Zn = nistManager->FindOrBuildElement("Zn");

    G4Material* BGO_Material = new G4Material("BGO", 7.13 * g / cm3, 3);
    BGO_Material->AddElement(O, 12);
    BGO_Material->AddElement(Bi, 4);
    BGO_Material->AddElement(Ge, 3);

    G4Material* PET = new G4Material("PET", 1.4 * g / cm3, 3);
    PET->AddElement(C, 10);
    PET->AddElement(O, 3);
    PET->AddElement(H, 8);

    G4Material* Brass = new G4Material("Brass", 8.5 * g / cm3, 2);
    Brass->AddElement(Cu, 70 * perCent);
    Brass->AddElement(Zn, 30 * perCent);

    new G4Material("Galactic", 1, 1.01 * g / mole, universe_mean_density, kStateGas, 2.73 * kelvin, 3.e-18 * pascal);
    new G4Material("Nothing", 1, 0, universe_mean_density, kStateUndefined, 0, 0);
}

void DetectorConstruction::ConstructSDandField()
{
    for (const auto& det : detectors) {
        auto activeVolume = new G4MultiFunctionalDetector(det);
        G4SDManager::GetSDMpointer()->AddNewDetector(activeVolume);
        activeVolume->RegisterPrimitive(new G4PSEnergyDeposit("edep"));

        if (det.rfind("BGO", 0) == 0) {
            SetSensitiveDetector("BGO_" + det + "_bgo_lv", activeVolume);
            continue;
        }
        if (det.rfind("Si", 0) == 0) {
            SetSensitiveDetector("PIPS_" + det + "_active_logical", activeVolume);
            continue;
        }
        if (det.rfind("Ge", 0) == 0 || det.rfind('A', 0) == 0 || det.rfind('B', 0) == 0) {
            SetSensitiveDetector("HPGe_" + det + "_crystal_logical", activeVolume);
            continue;
        }

        G4Exception("DetectorConstruction::ConstructSDandField", "Unknown detector type", FatalException, det.c_str());
    }
}
