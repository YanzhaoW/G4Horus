#include "CloverSD.hh"
#include <G4SDManager.hh>
#include <fmt/format.h>

namespace G4Horus
{
    CloverSD::CloverSD(std::string_view name)
        : G4VSensitiveDetector(name.data())
    {
        collectionName.push_back(CloverHit::collection_name.data());
        // hit_map_ = std::make_unique<CloverHitsMap>(SensitiveDetectorName, CloverHit::collection_name.data());
    }

    void CloverSD::Initialize(G4HCofThisEvent* hit_collections)
    {
        auto hit_map = std::make_unique<CloverHitsMap>(SensitiveDetectorName, CloverHit::collection_name.data());
        hit_map->add_keys<1>({ 0 });
        auto collection_id = G4SDManager::GetSDMpointer()->GetCollectionID(hit_map.get());
        hit_map_ = hit_map.get();
        hit_collections->AddHitsCollection(collection_id, hit_map.release());
    }

    auto CloverSD::ProcessHits(G4Step* step, G4TouchableHistory* /*unused*/) -> bool
    {
        auto energy_deposition = step->GetTotalEnergyDeposit();

        if (energy_deposition == 0.)
        {
            return true;
        }

        auto* pre_step_point = step->GetPreStepPoint();
        const auto* touchable = pre_step_point->GetTouchable();
        auto volume_id = touchable->GetVolume()->GetCopyNo();

        auto& hit = hit_map_->emplace_value(volume_id);

        hit.set_energy(energy_deposition);
        hit.set_time(pre_step_point->GetGlobalTime());
        hit.set_position(pre_step_point->GetPosition());

        return true;
    }

} // namespace G4Horus
