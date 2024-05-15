#pragma once

#include "G4VUserDetectorConstruction.hh"
#include <G4SystemOfUnits.hh>

extern const std::vector<std::string> detectors;

namespace G4Horus
{
    class DetectorConstruction : public G4VUserDetectorConstruction
    {
      public:
        DetectorConstruction() = default;
        explicit DetectorConstruction(bool is_overlap_checked = true)
            : is_overlap_checked_{ is_overlap_checked }
        {
        }

        auto Construct() -> G4VPhysicalVolume* override;
        void ConstructSDandField() override;
        void set_detector_distance(double distance) { detector_distance_ = distance; };

      private:
        void DefineMaterials();
        auto DefineVolumes() -> G4VPhysicalVolume*;

        G4bool is_overlap_checked_ = true;
        double detector_distance_ = 1. * cm;
    };

} // namespace G4Horus
