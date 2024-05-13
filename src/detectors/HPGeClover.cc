#include "HPGeClover.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
HPGe::Clover::Clover(const _spec& spec, const std::string& name)
    : HPGe(spec, name)
{
    // hull
    // auto rot = new G4RotationMatrix();
    // rot->rotateX(-90. * deg);
    new G4PVPlacement(nullptr,
                      G4ThreeVector(),
                      this->BuildHull(),
                      "HPGe_" + name + "_hull",
                      GetLogical(),
                      false,
                      0,
                      fSpec.check_overlaps);

    // TODO: Move that out. Maybe some Variant / Union stuff? Or derived data storage classes

    fCloverSpec.teflonThickness = 0.19 * mm;
    fCloverSpec.teflonLength = 80. * mm; // guess
    fCloverSpec.teflonEndcapThickness = 3. * mm;
    fCloverSpec.cavityLength = 20. * mm;

    _cloverLeaf baseLeaf;
    baseLeaf.alCupThickness = 0.51 * mm;
    baseLeaf.alCupLength = 30. * mm;
    baseLeaf.crystalOffset = 24. * mm;
    baseLeaf.holeLength = 51. * mm;
    baseLeaf.holeRadius = 4.5 * mm;
    baseLeaf.lilayerThickness = 0.7 * mm;

    if (fSpec.id != "CloziA" && fSpec.id != "CloziB")
    {
        throw std::runtime_error(fSpec.id + " is not a known Clover Type detector");
    }

    if (fSpec.id == "CloziB")
    {
        _cloverLeaf B0_leaf(baseLeaf);
        B0_leaf.name = "B0";
        B0_leaf.allayerThickness = 1.5 * mm;
        B0_leaf.crystalRadius = 28.00 * mm;
        B0_leaf.crystalLength = 59.5 * mm;
        B0_leaf.deadlayerThickness = 0.0003 * mm;
        B0_leaf.distanceToEndcap = 16.0 * mm;
        fCloverSpec.crystals.push_back(B0_leaf);

        _cloverLeaf B1_leaf(baseLeaf);
        B1_leaf.name = "B1";
        B1_leaf.allayerThickness = 1.5 * mm; // 1.5
        B1_leaf.crystalRadius = 28.00 * mm;
        B1_leaf.crystalLength = 58.5 * mm;
        B1_leaf.deadlayerThickness = 0.0003 * mm;
        B1_leaf.distanceToEndcap = 18.0 * mm;
        fCloverSpec.crystals.push_back(B1_leaf);

        _cloverLeaf B2_leaf(baseLeaf);
        B2_leaf.name = "B2";
        B2_leaf.allayerThickness = 1.5 * mm; // 1.5
        B2_leaf.crystalRadius = 27.30 * mm;
        B2_leaf.crystalLength = 58.7 * mm;
        B2_leaf.deadlayerThickness = 0.0003 * mm;
        B2_leaf.distanceToEndcap = 16.0 * mm;
        fCloverSpec.crystals.push_back(B2_leaf);

        _cloverLeaf B3_leaf(baseLeaf);
        B3_leaf.name = "B3";
        B3_leaf.allayerThickness = 1.5 * mm; // 1.5
        B3_leaf.crystalRadius = 27.85 * mm;
        B3_leaf.crystalLength = 57.2 * mm;
        B3_leaf.deadlayerThickness = 0.0003 * mm;
        B3_leaf.distanceToEndcap = 15.0 * mm;
        fCloverSpec.crystals.push_back(B3_leaf);
    }

    if (fSpec.id == "CloziA")
    {
        _cloverLeaf A0_leaf(baseLeaf);
        A0_leaf.name = "A0";
        A0_leaf.allayerThickness = 1.5 * mm;
        A0_leaf.crystalRadius = 27.00 * mm;
        A0_leaf.crystalLength = 56.8 * mm;
        A0_leaf.deadlayerThickness = 0.0003 * mm;
        A0_leaf.distanceToEndcap = 17.0 * mm;
        fCloverSpec.crystals.push_back(A0_leaf);

        _cloverLeaf A1_leaf(baseLeaf);
        A1_leaf.name = "A1";
        A1_leaf.allayerThickness = 1.5 * mm;
        A1_leaf.crystalRadius = 27.20 * mm;
        A1_leaf.crystalLength = 59.0 * mm;
        A1_leaf.deadlayerThickness = 0.0003 * mm;
        A1_leaf.distanceToEndcap = 13.0 * mm;
        fCloverSpec.crystals.push_back(A1_leaf);

        _cloverLeaf A2_leaf(baseLeaf);
        A2_leaf.name = "A2";
        A2_leaf.allayerThickness = 1.5 * mm;
        A2_leaf.crystalRadius = 27.20 * mm;
        A2_leaf.crystalLength = 56.0 * mm;
        A2_leaf.deadlayerThickness = 0.0003 * mm;
        A2_leaf.distanceToEndcap = 18.0 * mm;
        fCloverSpec.crystals.push_back(A2_leaf);

        _cloverLeaf A3_leaf(baseLeaf);
        A3_leaf.name = "A3";
        A3_leaf.allayerThickness = 1.5 * mm;
        A3_leaf.crystalRadius = 28.40 * mm;
        A3_leaf.crystalLength = 56.6 * mm;
        A3_leaf.deadlayerThickness = 0.0003 * mm;
        A3_leaf.distanceToEndcap = 20.0 * mm;
        fCloverSpec.crystals.push_back(A3_leaf);
    }

    Leaf(-1., +1., 000. * deg, fCloverSpec.crystals.at(0), fSpec.check_overlaps);
    Leaf(-1., -1., 090. * deg, fCloverSpec.crystals.at(1), fSpec.check_overlaps);
    Leaf(+1., -1., 180. * deg, fCloverSpec.crystals.at(2), fSpec.check_overlaps);
    Leaf(+1., +1., 270. * deg, fCloverSpec.crystals.at(3), fSpec.check_overlaps);

    // Teflon Insulator
    {
        auto* box = new G4Box("",
                              2 * fCloverSpec.crystals.at(0).crystalRadius,
                              fCloverSpec.teflonThickness,
                              fCloverSpec.teflonLength / 2.);

        auto* rm90 = new G4RotationMatrix();
        rm90->rotateZ(90. * deg);
        auto* insulatorSV = new G4UnionSolid(fSpec.id + "_insulatorSV", box, box, rm90, G4ThreeVector());

        auto* insulatorLV = new G4LogicalVolume(
            insulatorSV, G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"), fSpec.id + "_insulatorLV");
        auto* insulatorVA = new G4VisAttributes(G4Colour(.4, .4, .8, .3));
        insulatorVA->SetForceSolid(true);
        insulatorLV->SetVisAttributes(insulatorVA);

        G4ThreeVector insulatorPO(0., 0., -(GetLength() / 2. - 3 * fCloverSpec.crystals.at(0).distanceToEndcap));
        new G4PVPlacement(
            nullptr, insulatorPO, insulatorLV, fSpec.id + "_insulatorPV", GetLogical(), false, 0, fSpec.check_overlaps);
    }

    // Teflon endcap
    /*{
        const G4double outerRadius_Te_Cap = fSpec.hull.diameter / 2. - fSpec.hull.thickness;
        const G4double height_Te_Cap = fCloverSpec.teflonEndcapThickness / 2.;
        auto teEndcapSV = new G4Tubs(fSpec.id + "_teEndcapSV", 0., outerRadius_Te_Cap, height_Te_Cap, 0. * deg, 360. *
    deg);

        auto teEndcapLV = new G4LogicalVolume(teEndcapSV, G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
    fSpec.id + "_teEndcapLV"); auto teEndcapVA = new G4VisAttributes(G4Colour(.4, .4, .8, .3));
        teEndcapVA->SetForceSolid(true);
        teEndcapLV->SetVisAttributes(teEndcapVA);

        G4ThreeVector teEndcapPO(0., 0., fLength / 2. - fCloverSpec.teflonEndcapThickness / 2.);
        new G4PVPlacement(nullptr, teEndcapPO, teEndcapLV, fSpec.id + "_teEndcapPV", fDetectorLV, false, 0,
    fSpec.check_overlaps);
    }*/
}

void HPGe::Clover::Leaf(G4double pad_num_horizontal,
                        G4double pad_num_vertical,
                        G4double rotz,
                        const _cloverLeaf& leaf,
                        bool checkOverlap) const
{
    auto* Ge_material = G4NistManager::Instance()->FindOrBuildMaterial("G4_Ge");
    auto* Al_material = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
    auto* Li_material = G4NistManager::Instance()->FindOrBuildMaterial("G4_Li");
    auto* Au_material = G4NistManager::Instance()->FindOrBuildMaterial("G4_Au");

    const G4double cutbox_size = 30.0 * cm; // Size dons't matter, just needs to be large enough
    auto* cutbox = new G4Box("cutbox", cutbox_size, cutbox_size, cutbox_size);

    auto* tube = new G4Tubs("tube", 0., leaf.crystalRadius, leaf.crystalLength / 2., 0. * deg, 360. * deg);
    auto* hole = new G4Tubs("hole", 0., leaf.holeRadius, leaf.holeLength / 2., 0. * deg, 360. * deg);

    auto* crystal_cut1 = new G4SubtractionSolid(
        "crystal_cut1", tube, cutbox, nullptr, G4ThreeVector(0, -(leaf.crystalOffset + cutbox_size), 0));
    auto* crystal_cut2 = new G4SubtractionSolid(
        "crystal_cut2", crystal_cut1, cutbox, nullptr, G4ThreeVector((leaf.crystalOffset + cutbox_size), 0, 0));

    G4RotationMatrix rotation_matrix;
    rotation_matrix.rotateZ(rotz);

    // Kristall
    {
        auto* crystalSV =
            new G4SubtractionSolid(leaf.name + "_crystalSV",
                                   crystal_cut2,
                                   hole,
                                   nullptr,
                                   G4ThreeVector(0., 0., +((leaf.crystalLength - leaf.holeLength + 0.01) / 2.)));
        auto* crystalLV = new G4LogicalVolume(crystalSV, Ge_material, "HPGe_" + leaf.name + "_crystal_logical");
        auto* crystalVA = new G4VisAttributes(G4Colour(1.0, 1.0, 0.));
        crystalVA->SetForceSolid(true);
        crystalLV->SetVisAttributes(crystalVA);

        G4ThreeVector crystalPO(
            pad_num_horizontal * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            pad_num_vertical * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            -(GetLength() / 2. - leaf.distanceToEndcap - leaf.allayerThickness - leaf.deadlayerThickness -
              leaf.crystalLength / 2.));
        new G4PVPlacement(G4Transform3D(rotation_matrix, crystalPO),
                          crystalLV,
                          leaf.name + "_crystalPV",
                          GetLogical(),
                          false,
                          0,
                          checkOverlap);
    }

    // Todschicht Kristall
    {
        auto* inactiveSV_Step1 =
            new G4Tubs("", 0., leaf.crystalRadius, leaf.deadlayerThickness / 2., 0. * deg, 360. * deg);
        auto* inactiveSV_Step2 = new G4SubtractionSolid(
            "", inactiveSV_Step1, cutbox, nullptr, G4ThreeVector(0, -(leaf.crystalOffset + cutbox_size), 0));
        auto* inactiveSV = new G4SubtractionSolid(leaf.name + "_inactiveSV",
                                                  inactiveSV_Step2,
                                                  cutbox,
                                                  nullptr,
                                                  G4ThreeVector((leaf.crystalOffset + cutbox_size), 0, 0));

        auto* inactiveLV = new G4LogicalVolume(inactiveSV, Ge_material, leaf.name + "_inactiveLV");
        auto* inactiveVA = new G4VisAttributes(G4Colour(.0, 1.0, 0.0));
        inactiveVA->SetForceSolid(true);
        inactiveLV->SetVisAttributes(inactiveVA);

        G4ThreeVector inactivePO(
            pad_num_horizontal * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            pad_num_vertical * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            -(GetLength() / 2. - leaf.distanceToEndcap - leaf.allayerThickness - leaf.deadlayerThickness / 2.));

        new G4PVPlacement(G4Transform3D(rotation_matrix, inactivePO),
                          inactiveLV,
                          leaf.name + "_inactivePV",
                          GetLogical(),
                          false,
                          0,
                          checkOverlap);
    }

    // Aluschicht Kristall
    {
        auto* alabsSV_Step1 = new G4Tubs("", 0., leaf.crystalRadius, leaf.allayerThickness / 2., 0. * deg, 360. * deg);
        auto* alabsSV_Step2 = new G4SubtractionSolid(
            "", alabsSV_Step1, cutbox, nullptr, G4ThreeVector(0, -(leaf.crystalOffset + cutbox_size), 0));
        auto* alabsSV = new G4SubtractionSolid(leaf.name + "_alAbsSV",
                                               alabsSV_Step2,
                                               cutbox,
                                               nullptr,
                                               G4ThreeVector((leaf.crystalOffset + cutbox_size), 0, 0));

        auto* alabsLV = new G4LogicalVolume(alabsSV, Ge_material, leaf.name + "_alAbsLV");
        auto* alabsVA = new G4VisAttributes(G4Colour(1.0, .0, .0));
        alabsVA->SetForceSolid(true);
        alabsLV->SetVisAttributes(alabsVA);

        G4ThreeVector alabsPO(
            pad_num_horizontal * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            pad_num_vertical * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            -(GetLength() / 2. - leaf.distanceToEndcap - leaf.allayerThickness / 2.));

        new G4PVPlacement(G4Transform3D(rotation_matrix, alabsPO),
                          alabsLV,
                          leaf.name + "_alAbsPV",
                          GetLogical(),
                          false,
                          0,
                          checkOverlap);
    }

    // Lithium Kontatktierung Kristall
    {
        auto* liTubeSV = new G4Tubs(leaf.name + "_liTubeSV",
                                    leaf.holeRadius - leaf.lilayerThickness,
                                    leaf.holeRadius,
                                    leaf.holeLength / 2.,
                                    0. * deg,
                                    360. * deg);

        auto* liTubeLV = new G4LogicalVolume(liTubeSV, Li_material, leaf.name + "_liTubeLV");
        auto* liTubeVA = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0));
        liTubeVA->SetForceSolid(true);
        liTubeLV->SetVisAttributes(liTubeVA);

        G4ThreeVector liTubePO(
            pad_num_horizontal * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            pad_num_vertical * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            -(GetLength() / 2. - leaf.distanceToEndcap - leaf.allayerThickness - leaf.deadlayerThickness -
              (leaf.crystalLength - leaf.holeLength) - leaf.lilayerThickness - leaf.holeLength / 2.));
        new G4PVPlacement(G4Transform3D(rotation_matrix, liTubePO),
                          liTubeLV,
                          leaf.name + "_liTubePV",
                          GetLogical(),
                          false,
                          0,
                          checkOverlap);

        auto* liCapSV = new G4Tubs(leaf.name + "_liCapSV",
                                   0.,
                                   leaf.holeRadius - leaf.lilayerThickness,
                                   leaf.lilayerThickness / 2.,
                                   0. * deg,
                                   360. * deg);

        auto* liCapLV = new G4LogicalVolume(liCapSV, Li_material, leaf.name + "_liCapLV");
        liCapLV->SetVisAttributes(liTubeVA);

        G4ThreeVector liCapPO(
            pad_num_horizontal * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            pad_num_vertical * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            -(GetLength() / 2. - leaf.distanceToEndcap - leaf.allayerThickness - leaf.deadlayerThickness -
              (leaf.crystalLength - leaf.holeLength) - leaf.lilayerThickness - leaf.lilayerThickness / 2.));

        new G4PVPlacement(G4Transform3D(rotation_matrix, liCapPO),
                          liCapLV,
                          leaf.name + "_liCapPV",
                          GetLogical(),
                          false,
                          0,
                          checkOverlap);
    }

    // Kühlfinger Kristall
    {
        G4double height_finger = (fCloverSpec.cavityLength + leaf.holeLength - leaf.lilayerThickness) / 2.;
        auto* fingerSV =
            new G4Tubs(leaf.name + "_fingerSV", 0., leaf.holeRadius / 3., height_finger, 0. * deg, 360. * deg);

        auto* fingerLV = new G4LogicalVolume(fingerSV, Au_material, leaf.name + "_fingerLV");
        auto* fingerVA = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
        fingerVA->SetForceSolid(true);
        fingerLV->SetVisAttributes(fingerVA);

        G4ThreeVector fingerPO(
            pad_num_horizontal * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            pad_num_vertical * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            -(GetLength() / 2. - leaf.distanceToEndcap - leaf.allayerThickness - leaf.deadlayerThickness -
              (leaf.crystalLength - leaf.holeLength) - 2. * leaf.lilayerThickness - height_finger));
        new G4PVPlacement(G4Transform3D(rotation_matrix, fingerPO),
                          fingerLV,
                          leaf.name + "_fingerPV",
                          GetLogical(),
                          false,
                          0,
                          checkOverlap);
    }

    // Alu Einfassung Kristall
    {
        G4double cutbox1_cup_x = (leaf.alCupThickness + leaf.crystalOffset) / 2.;
        G4double cutbox1_cup_y = (2 * leaf.alCupThickness + leaf.crystalRadius + leaf.crystalOffset) / 2.;
        G4double cutbox1_cup_z = leaf.alCupLength / 2.;
        auto* cutbox1_cup = new G4Box("cutbox", cutbox1_cup_x, cutbox1_cup_y, cutbox1_cup_z);
        G4double cutbox2_cup_x = (leaf.alCupThickness + leaf.crystalRadius) / 2.;
        G4double cutbox2_cup_y = (leaf.alCupThickness + leaf.crystalOffset) / 2.;
        G4double cutbox2_cup_z = leaf.alCupLength / 2.;
        auto* cutbox2_cup = new G4Box("cutbox", cutbox2_cup_x, cutbox2_cup_y, cutbox2_cup_z);
        auto* cutbox_cup = new G4UnionSolid(
            "cutbox_union", cutbox1_cup, cutbox2_cup, nullptr, G4ThreeVector(-cutbox1_cup_y, -cutbox2_cup_x, 0.));
        auto* alCupSV = new G4SubtractionSolid(
            leaf.name + "_alCupSV",
            cutbox_cup,
            crystal_cut2,
            nullptr,
            G4ThreeVector(
                -(leaf.alCupThickness + leaf.crystalOffset) / 2, -(leaf.crystalRadius - leaf.crystalOffset) / 2., 0));

        auto* alCupLV = new G4LogicalVolume(alCupSV, Al_material, leaf.name + "_alCupLV");
        auto* alCupVA = new G4VisAttributes(G4Colour(1.0, .0, .0));
        alCupVA->SetForceSolid(true);
        alCupLV->SetVisAttributes(alCupVA);

        auto pos_y = 0.;
        auto pos_x = 0.;

        if (pad_num_horizontal * pad_num_vertical < 0)
        {
            pos_x = (((leaf.crystalOffset + leaf.alCupThickness) / 2) + fCloverSpec.teflonThickness);
            pos_y = (((2 * leaf.alCupThickness + leaf.crystalRadius + leaf.crystalOffset) / 2) +
                     fCloverSpec.teflonThickness);
        }
        else
        {
            pos_y = (((leaf.crystalOffset + leaf.alCupThickness) / 2) + fCloverSpec.teflonThickness);
            pos_x = (((2 * leaf.alCupThickness + leaf.crystalRadius + leaf.crystalOffset) / 2) +
                     fCloverSpec.teflonThickness);
        }

        G4ThreeVector alCupPO(
            pad_num_horizontal * pos_x,
            pad_num_vertical * pos_y,
            -(GetLength() / 2. - leaf.distanceToEndcap - leaf.allayerThickness - leaf.deadlayerThickness -
              (leaf.crystalLength - leaf.alCupLength) - leaf.alCupLength / 2.));
        new G4PVPlacement(G4Transform3D(rotation_matrix, alCupPO),
                          alCupLV,
                          leaf.name + "_alCupPV",
                          GetLogical(),
                          false,
                          0,
                          checkOverlap);

        G4double innerRadius_Cup = leaf.crystalRadius;
        G4double outerRadius_Cup = leaf.crystalRadius + leaf.alCupThickness;
        G4double height_Cup = leaf.alCupLength / 2.;
        G4double startAngle_Cup = 90. * deg;
        G4double spanningAngle_Cup = 90. * deg;
        auto* cup_tube = new G4Tubs(
            leaf.name + "_alTubeSV", innerRadius_Cup, outerRadius_Cup, height_Cup, startAngle_Cup, spanningAngle_Cup);

        auto* alTubeLV = new G4LogicalVolume(cup_tube, Al_material, leaf.name + "_alTubeLV");
        alTubeLV->SetVisAttributes(alCupVA);

        G4ThreeVector alTubePO(
            pad_num_horizontal * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            pad_num_vertical * (leaf.alCupThickness + fCloverSpec.teflonThickness + leaf.crystalOffset),
            -(GetLength() / 2. - leaf.distanceToEndcap - leaf.allayerThickness - leaf.deadlayerThickness -
              (leaf.crystalLength - leaf.alCupLength) - leaf.alCupLength / 2.));
        new G4PVPlacement(G4Transform3D(rotation_matrix, alTubePO),
                          alTubeLV,
                          leaf.name + "_alTubePV",
                          GetLogical(),
                          false,
                          0,
                          checkOverlap);
    }
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
