#include "EventActionNtuple.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "RunActionNtuple.hh"
#include <G4THitsMap.hh>
#include <numeric>

extern const std::vector<std::string> detectors;
namespace G4Horus
{
    inline auto GetHitsCollection(const int detector_id, const G4Event* event) -> G4THitsMap<double>*
    {
        auto* hits_collection = dynamic_cast<G4THitsMap<G4double>*>(event->GetHCofThisEvent()->GetHC(detector_id));

        if (hits_collection == nullptr)
        {
            G4ExceptionDescription msg;
            msg << "Cannot access hits collection with ID: " << detector_id;
            G4Exception("EventActionNtuple::GetHitsCollection()", "#EA001", FatalException, msg);
        }

        return hits_collection;
    }

    inline auto GetSum(const G4THitsMap<double>* hits) -> double
    {
        return std::accumulate(hits->GetMap()->begin(),
                               hits->GetMap()->end(),
                               0.,
                               [](const G4double sum, const std::pair<G4int, G4double*>& point)
                               { return sum + *(point.second); });
    }

    EventActionNtuple::EventActionNtuple(RunActionNtuple* run_action)
        : fIDsCached(false)
        , run_action_{ run_action }
    {
    }

    void EventActionNtuple::EndOfEventAction(const G4Event* event)
    {
        // if (!fIDsCached)
        // {
        //     for (const auto& det : detectors)
        //     {
        //         fHitCollectionIDs.push_back(G4SDManager::GetSDMpointer()->GetCollectionID(det + "/edep"));
        //     }
        //     fIDsCached = true;
        // }

        // auto* analysisManager = G4AnalysisManager::Instance();
        // const size_t ndets = detectors.size();
        // for (size_t i = 0; i < ndets; i++)
        // {
        //     const G4double edep = GetSum(GetHitsCollection(fHitCollectionIDs.at(i), event));
        //     analysisManager->FillH1(i, edep);
        //     analysisManager->FillNtupleDColumn(i, edep);
        // }
        // analysisManager->AddNtupleRow();
    }
} // namespace G4Horus
