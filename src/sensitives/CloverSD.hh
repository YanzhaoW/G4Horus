#pragma once

#include "CloverHit.hh"
#include <G4VSensitiveDetector.hh>

namespace G4Horus
{
    class CloverSD : public G4VSensitiveDetector
    {
      public:
        explicit CloverSD(std::string_view name = "CloverSD");

      private:
        CloverHitsMap* hit_map_ = nullptr;

        void Initialize(G4HCofThisEvent* hit_collections) override;
        auto ProcessHits(G4Step* step, G4TouchableHistory* /*unused*/) -> bool override;
    };
} // namespace G4Horus