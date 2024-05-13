#include "HPGeHexagonal.hh"
#include "G4IntersectionSolid.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"
#include <sstream>
#include <string>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
HPGe::Hexagonal::Hexagonal(const _spec& spec, const std::string& name)
    : HPGe(spec, name)
{
    // hull front (endcap)
    new G4PVPlacement(nullptr,
                      G4ThreeVector(0, 0, -(GetLength() / 2 - fSpec.hull.endcap_thickness / 2)),
                      this->BuildHullFront(),
                      "HPGe_" + name + "_hull_front",
                      GetLogical(),
                      false,
                      0,
                      fSpec.check_overlaps);

    // hull
    new G4PVPlacement(nullptr,
                      G4ThreeVector(0, 0, -(GetLength() / 2 - fSpec.hull.length / 2)),
                      this->BuildHull(),
                      "HPGe_" + name + "_hull",
                      GetLogical(),
                      false,
                      0,
                      fSpec.check_overlaps);

    auto* hull_material = G4Material::GetMaterial("G4_Al");
    auto crystal_material = G4Material::GetMaterial("G4_Ge");

    G4double capsule_length = 91. * mm;
    G4double capsule_base_radius = 62.7 * mm / 2.;
    G4double capsule_thickness = 0.7 * mm;
    G4double capsule_padding = 0.7 * mm;

    // Capsule (additional hull around the crystal) is build by subtracting a smaller (~> hull thickness) solid from the
    // full solid
    auto* capsule_full_solid = HexShape("HPGe_" + name + "_capsule_full", capsule_base_radius, capsule_length / 2.);
    auto capsule_inner_solid = HexShape("HPGe_" + name + "_capsule_inner",
                                        capsule_base_radius - capsule_thickness,
                                        capsule_length / 2. - capsule_thickness);
    auto* capsule_solid = new G4SubtractionSolid(
        "HPGe_" + name + "_capsule_solid", capsule_full_solid, capsule_inner_solid, nullptr, G4ThreeVector());
    auto* capsule_logical = new G4LogicalVolume(capsule_solid, hull_material, "HPGe_" + name + "_capsule_logical");
    auto hull_vis = G4VisAttributes(G4Color(0.2, 0.3, 0.2, 0.8));
    hull_vis.SetForceSolid(true);
    capsule_logical->SetVisAttributes(hull_vis);
    auto* rm = new G4RotationMatrix;
    rm->rotateX(180. * deg);
    new G4PVPlacement(
        rm,
        G4ThreeVector(0, 0, -(GetLength() / 2. - spec.hull.thickness - spec.hull.padding - capsule_length / 2.)),
        capsule_logical,
        "HPGe_" + name + "_capsule",
        GetLogical(),
        false,
        0,
        spec.check_overlaps);

    // Germanium Dead layer
    G4double gdl_l[2] = { spec.crystal.dead_layer / 2., -spec.crystal.dead_layer / 2. };
    G4double gdl_r1[2] = { 0, 0 };
    G4double gdl_r2[2] = { spec.crystal.diameter / 2., spec.crystal.diameter / 2. };
    auto crystal_dead_layer_solid =
        new G4Polyhedra(name + "_poly_S", 0. * deg, 360. * deg, 6, 2, gdl_l, gdl_r1, gdl_r2);
    auto crystal_dead_layer_logical =
        new G4LogicalVolume(crystal_dead_layer_solid, crystal_material, "HPGe_" + name + "_crystal_dead_layer_logical");
    auto crystal_dead_layer_vis = G4VisAttributes(G4Color(1, 0.5, 1, 0.7));
    crystal_dead_layer_vis.SetForceSolid(true);
    crystal_dead_layer_logical->SetVisAttributes(crystal_dead_layer_vis);
    new G4PVPlacement(rm,
                      G4ThreeVector(0,
                                    0,
                                    -(GetLength() / 2. - spec.hull.thickness - spec.hull.padding - capsule_thickness -
                                      capsule_padding - spec.crystal.dead_layer / 2.)),
                      crystal_dead_layer_logical,
                      "HPGe_" + name + "_crystal_dead_layer",
                      GetLogical(),
                      false,
                      0,
                      spec.check_overlaps);

    // Germanium Inactive Region around cryo finger
    G4double inactiveRadius = 0.0;
    G4double inactiveExtraLength = 0.0;
    std::stringstream ending;
    // assignment of inactive Region can be done in two different ways: directly assigning the length and radius or
    // giving the detector an efficiency-value to be reduced to symetrically. This is to differentiate between the two
    // methods.
    if (spec.crystal.inactive_radius == 0.0 && spec.crystal.inactive_length == 0.0)
    {
        inactiveRadius =
            sqrt(-spec.efficiency * (pow(spec.crystal.diameter / 2., 2) - pow(spec.crystal.hole_diameter / 2., 2)) +
                 pow(spec.crystal.diameter / 2., 2));
        // inactiveExtraLength=
        // -spec.efficiency*(spec.crystal.length-spec.crystal.hole_length)+spec.crystal.length-spec.crystal.hole_length;//extra
        // length on top of the cooling finger.
        inactiveExtraLength =
            (spec.crystal.length - spec.crystal.hole_length) * pow(spec.crystal.diameter / 2., 2) *
            (1 - spec.efficiency) /
            pow(spec.crystal.hole_diameter / 2. + inactiveRadius, 2); // new and hopefully correct formula
        ending << " for " << (1 - spec.efficiency) * 100 << "% of efficiency reduction.";
    }
    else
    {
        inactiveRadius = spec.crystal.inactive_radius;
        inactiveExtraLength = spec.crystal.inactive_length;
        ending << ". (directly assigned)";
    }
    std::string flavourtext = ending.str();
    if (inactiveRadius != 0.0 && inactiveExtraLength != 0.0)
    {
        G4cout << "HPGe detector " << name << " with inactive layer has " << inactiveRadius
               << " mm of inactive material around and " << inactiveExtraLength << "mm in front of cryo-finger"
               << flavourtext << G4endl;

        auto crystal_inactive_solid_cylinder = new G4Tubs("HPGe_" + name + "_inactive_crystal_solid_with_hole",
                                                          spec.crystal.hole_diameter / 2.,
                                                          /*spec.crystal.hole_diameter / 2. +*/ inactiveRadius,
                                                          spec.crystal.hole_length / 2.,
                                                          0. * deg,
                                                          360. * deg);
        auto crystal_inactive_solid_top = new G4Tubs("HPGe_" + name + "_inactive_crystal_solid_top",
                                                     0.,
                                                     /*spec.crystal.hole_diameter / 2. +*/ inactiveRadius,
                                                     inactiveExtraLength / 2.,
                                                     0. * deg,
                                                     360. * deg);
        auto crystal_inactive_solid =
            new G4UnionSolid("HPGe_" + name + "_inactive_crystal_complete",
                             crystal_inactive_solid_cylinder,
                             crystal_inactive_solid_top,
                             0,
                             G4ThreeVector(0, 0, (spec.crystal.hole_length / 2. + inactiveExtraLength / 2.)));
        auto crystal_inactive_logical =
            new G4LogicalVolume(crystal_inactive_solid, crystal_material, "HPGe_" + name + "_crystal_inactive_logical");
        auto crystal_inactive_vis = G4VisAttributes(G4Color(1, 0.5, 0, 0.7)); // maybe change color to see geometry
        crystal_inactive_vis.SetForceSolid(true);
        crystal_inactive_logical->SetVisAttributes(crystal_inactive_vis);
        new G4PVPlacement(
            rm,
            G4ThreeVector(0,
                          0,
                          -(GetLength() / 2. - spec.hull.thickness - spec.hull.padding - capsule_thickness -
                            capsule_padding - spec.crystal.dead_layer -
                            (spec.crystal.length - spec.crystal.hole_length) - spec.crystal.hole_length / 2)),
            crystal_inactive_logical,
            "HPGe_" + name + "_inactive_crystal",
            GetLogical(),
            false,
            0,
            spec.check_overlaps);
    }
    else
    {
        G4cout << "HPGe detector " << name << " without inactive layer was set up. " << G4endl;
        G4cout
            << "No inactive region was assigned due to efficiency being set to 100% or invalid parameters being given. "
            << G4endl;
    }
    auto crystal_inactive_solid_wo_hole = new G4Tubs("HPGe_" + name + "_crystal",
                                                     0.,
                                                     /*(spec.crystal.hole_diameter / 2.) +*/ inactiveRadius,
                                                     (spec.crystal.hole_length / 2.) + inactiveExtraLength / 2.,
                                                     0. * deg,
                                                     360. * deg);
    auto crystal_inactive_solid_total =
        new G4UnionSolid("HPGe_" + name + "_crystal_total_inactive_region",
                         crystal_inactive_solid_wo_hole,
                         crystal_dead_layer_solid); // translation for dead region not specified

    // Germanium crystal
    auto crystal_solid_wo_hole =
        HexShape("HPGe_" + name + "_crystal", spec.crystal.diameter / 2., spec.crystal.length / 2.);
    // auto crystal_hole_solid = new G4Tubs("HPGe_" + name + "_crystal_hole_solid", 0. * cm, spec.crystal.hole_diameter
    // / 2., spec.crystal.hole_length / 2., 0. * deg, 360. * deg);
    auto crystal_solid = new G4SubtractionSolid(
        "HPGe_" + name + "_crystal_solid_with_hole",
        crystal_solid_wo_hole,
        crystal_inactive_solid_total,
        nullptr,
        G4ThreeVector(0, 0, -(spec.crystal.length / 2. - spec.crystal.hole_length / 2. - inactiveExtraLength / 2.)));
    auto crystal_logical = new G4LogicalVolume(crystal_solid, crystal_material, "HPGe_" + name + "_crystal_logical");
    auto crystal_vis = G4VisAttributes(G4Color(0, 0.5, 1, 0.7));
    crystal_vis.SetForceSolid(true);
    crystal_logical->SetVisAttributes(crystal_vis);
    new G4PVPlacement(rm,
                      G4ThreeVector(0,
                                    0,
                                    -(GetLength() / 2. - spec.hull.thickness - spec.hull.padding - capsule_thickness -
                                      capsule_padding - spec.crystal.dead_layer - spec.crystal.length / 2.)),
                      crystal_logical,
                      "HPGe_" + name + "_crystal",
                      GetLogical(),
                      false,
                      0,
                      spec.check_overlaps);

    // Germanium Volume check
    G4double volume = crystal_solid->GetCubicVolume() + crystal_dead_layer_solid->GetCubicVolume();
    G4cout << "HPGe: " << name << " with Id: " << spec.id << " - crystal volume:  " << volume / cm3
           << "cm3 = " << volume * crystal_material->GetDensity() / g << "g" << G4endl;
    G4cout << "HPGe: " << name << " with Id: " << spec.id << " - expected volume: " << spec.crystal.volume / cm3
           << "cm3 = " << spec.crystal.volume * crystal_material->GetDensity() / g << "g" << G4endl;
    G4cout << "HPGe: " << name << " with Id: " << spec.id
           << " - capsule mass:  " << capsule_solid->GetCubicVolume() * hull_material->GetDensity() / g << "g"
           << G4endl;
    /*if( fabs(volume/spec.crystal.volume - 1) > MAX_VOLUME_DIFFERENCE ){
        G4Exception("HPGe::Hexagonal::Hexagonal())", "Horus", JustWarning, ("Volume of detector " + spec.id + " does not
    match!").c_str() );
    }*/

    // cryo finger
    auto cryo_solid = new G4Tubs("HPGe_" + name + "_cryo_solid",
                                 0. * cm,
                                 spec.crystal.hole_diameter / 2. - 0.5 * mm,
                                 spec.crystal.hole_length / 2,
                                 0. * deg,
                                 360. * deg);
    auto cryo_logical = new G4LogicalVolume(cryo_solid, hull_material, "HPGe_" + name + "_cryo_logical");
    cryo_logical->SetVisAttributes(hull_vis);
    new G4PVPlacement(nullptr,
                      G4ThreeVector(0,
                                    0,
                                    -(GetLength() / 2. - spec.hull.thickness - spec.hull.padding - capsule_thickness -
                                      capsule_padding - spec.crystal.dead_layer -
                                      (spec.crystal.length - spec.crystal.hole_length) - spec.crystal.hole_length / 2)),
                      cryo_logical,
                      "HPGe_" + name + "_cryo",
                      GetLogical(),
                      false,
                      0,
                      spec.check_overlaps);
}

G4VSolid* HPGe::Hexagonal::HexShape(const std::string& name, const G4double& base_radius, const G4double& half_length)
{
    const G4double angle = 4.125 * deg;

    G4double top_radius = base_radius + tan(angle) * (half_length * 2.);

    G4double poly_r1[2] = { 0, 0 };
    G4double poly_r2[2] = { base_radius, top_radius };
    G4double poly_l[2] = { half_length, -half_length };

    return new G4IntersectionSolid(
        name + "_is_S",
        new G4Tubs(name + "_tubs_S", 0, top_radius, half_length, 0. * deg, 360. * deg),
        new G4Polyhedra(name + "_poly_S", 0. * deg, 360. * deg, 6, 2, poly_l, poly_r1, poly_r2),
        nullptr,
        G4ThreeVector());
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
