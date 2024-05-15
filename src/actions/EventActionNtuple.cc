#include "EventActionNtuple.hh"
#include "Definitions.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "RunActionNtuple.hh"
#include <CloverHit.hh>
#include <G4THitsMap.hh>
#include <fmt/format.h>
#include <numeric>
#include <ranges>

extern const std::vector<std::string> detectors;

namespace rng = std::ranges;

namespace G4Horus
{
    namespace
    {
        auto get_minimal_time(const std::vector<CloverHit>& hits) -> double
        {
            return rng::min(hits, rng::less{}, [](const auto& hit) { return hit.get_time(); }).get_time();
        }

        auto get_total_energy(const std::vector<CloverHit>& hits) -> double
        {
            return std::accumulate(hits.begin(),
                                   hits.end(),
                                   double{},
                                   [](double init, const auto& hit) { return init + hit.get_energy(); });
        }

        auto get_max_energy_position(const std::vector<CloverHit>& hits) -> G4ThreeVector
        {
            return rng::max(hits, rng::greater{}, [](const auto& hit) { return hit.get_energy(); }).get_position();
        }

    } // namespace

    EventActionNtuple::EventActionNtuple(RunActionNtuple* run_action)
        : fIDsCached(false)
        , run_action_{ run_action }
    {
    }

    void EventActionNtuple::EndOfEventAction(const G4Event* event)
    {
        auto* hit_collection = event->GetHCofThisEvent();
        if (hit_collection == nullptr)
        {
            return;
        }
        const auto& ntuple_colume_id_map = run_action_->GetColumnIdMap();
        auto* analysis_manager = G4AnalysisManager::Instance();
        for (int num_col{}; num_col < hit_collection->GetNumberOfCollections(); ++num_col)
        {
            const auto* collection = dynamic_cast<CloverHitsMap*>(hit_collection->GetHC(num_col));
            if (collection->GetName() == CLOVER_HIT_COLLECTION_NAME)
            {
                const auto& sd_name = collection->GetSDname();
                for (const auto& [_, hits] : collection->get_map())
                {
                    const auto hit_size = hits.size();
                    if (hit_size == 0)
                    {
                        continue;
                    }
                    const auto total_energy = get_total_energy(hits);
                    const auto earliest_time = get_minimal_time(hits);
                    const auto max_energy_position = get_max_energy_position(hits);

                    analysis_manager->FillNtupleDColumn(ntuple_colume_id_map.at(NTupleColumn::time), earliest_time);
                    analysis_manager->FillNtupleDColumn(ntuple_colume_id_map.at(NTupleColumn::energy), total_energy);
                    analysis_manager->FillNtupleIColumn(ntuple_colume_id_map.at(NTupleColumn::size),
                                                        static_cast<int>(hit_size));
                    analysis_manager->FillNtupleSColumn(ntuple_colume_id_map.at(NTupleColumn::detector), sd_name);
                    analysis_manager->FillNtupleDColumn(ntuple_colume_id_map.at(NTupleColumn::position_x),
                                                        max_energy_position.x());
                    analysis_manager->FillNtupleDColumn(ntuple_colume_id_map.at(NTupleColumn::position_y),
                                                        max_energy_position.y());
                    analysis_manager->FillNtupleDColumn(ntuple_colume_id_map.at(NTupleColumn::position_z),
                                                        max_energy_position.z());
                    analysis_manager->AddNtupleRow();
                }
            }
        }
    }
} // namespace G4Horus
