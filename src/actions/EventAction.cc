#include "EventAction.hh"
#include "Definitions.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "RunAction.hh"
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

    EventActionNtuple::EventActionNtuple(RunActionNtuple* run_action, int verbose_level)
        : verbose_level_{ verbose_level }
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
        for (int num_col{}; num_col < hit_collection->GetNumberOfCollections(); ++num_col)
        {
            const auto* collection = dynamic_cast<CloverHitsMap*>(hit_collection->GetHC(num_col));
            if (collection->GetName() == CLOVER_HIT_COLLECTION_NAME)
            {
                const auto& sd_name = collection->GetSDname();
                for (const auto& [_, hits] : collection->get_map())
                {
                    if (hits.empty())
                    {
                        continue;
                    }
                    write_data(hits, sd_name);
                }
            }
        }
    }

    void EventActionNtuple::write_data(const std::vector<CloverHit>& hits, std::string_view sd_name)
    {
        auto* analysis_manager = G4AnalysisManager::Instance();
        const auto& ntuple_colume_id_map = run_action_->GetColumnIdMap();
        const auto& histogram_id_map = run_action_->GetHistgramIdMap();
        const auto* settings = run_action_->GetSettings();
        const auto total_energy = get_total_energy(hits);

        if (settings->is_tuple_enabled)
        {
            const auto earliest_time = get_minimal_time(hits);
            const auto max_energy_position = get_max_energy_position(hits);
            const auto hit_size = hits.size();

            analysis_manager->FillNtupleDColumn(ntuple_colume_id_map.at(NTupleColumn::time), earliest_time / second);
            analysis_manager->FillNtupleDColumn(ntuple_colume_id_map.at(NTupleColumn::energy), total_energy / keV);
            analysis_manager->FillNtupleIColumn(ntuple_colume_id_map.at(NTupleColumn::size),
                                                static_cast<int>(hit_size));
            analysis_manager->FillNtupleSColumn(ntuple_colume_id_map.at(NTupleColumn::detector), sd_name.data());
            analysis_manager->FillNtupleDColumn(ntuple_colume_id_map.at(NTupleColumn::position_x),
                                                max_energy_position.x() / cm);
            analysis_manager->FillNtupleDColumn(ntuple_colume_id_map.at(NTupleColumn::position_y),
                                                max_energy_position.y() / cm);
            analysis_manager->FillNtupleDColumn(ntuple_colume_id_map.at(NTupleColumn::position_z),
                                                max_energy_position.z() / cm);
            analysis_manager->AddNtupleRow();
        }

        if (verbose_level_ > 0)
        {
            for (const auto& hit : hits)
            {
                G4cout << fmt::format("SD {:?} contains a hit: {}", sd_name, hit) << G4endl;
            }
        }

        if (settings->is_hist_enabled)
        {
            analysis_manager->FillH1(histogram_id_map.at(sd_name), total_energy);
        }

        if (settings->is_soco_enabled)
        {
            // TODO: Implement soco here
        }
    }
} // namespace G4Horus
