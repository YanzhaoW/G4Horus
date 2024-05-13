#pragma once

#include "HPGe.hh"

namespace HPGe
{

    class Clover : public HPGe
    {
      public:
        Clover(const _spec& spec, const std::string& name);

      protected:
        // G4LogicalVolume* BuildHull() override;
        // G4LogicalVolume* BuildHullFront() override;

      private:
        struct _cloverLeaf
        {
            std::string name;
            G4double alCupThickness;
            G4double alCupLength;
            G4double allayerThickness;
            G4double crystalRadius;
            G4double crystalOffset;
            G4double crystalLength;
            G4double deadlayerThickness;
            G4double distanceToEndcap;
            G4double holeRadius;
            G4double holeLength;
            G4double lilayerThickness;
        };

        struct _cloverSpec
        {
            G4double teflonThickness;
            G4double teflonLength;
            G4double teflonEndcapThickness;
            G4double cavityLength;
            std::vector<_cloverLeaf> crystals;
        };

        _cloverSpec fCloverSpec;

        void Leaf(G4double pad_num_horizontal,
                  G4double pad_num_vertical,
                  G4double rotz,
                  const _cloverLeaf& leaf,
                  bool checkOverlap) const;
    };

} // namespace HPGe
