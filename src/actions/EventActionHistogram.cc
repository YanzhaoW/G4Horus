#include "EventActionHistogram.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include <G4SystemOfUnits.hh>
#include <fmt/format.h>
#include <numeric>

extern const std::vector<std::string> detectors;

inline G4THitsMap<G4double>* GetHitsCollection(const G4int id, const G4Event* event)
{
    auto hits_collection = dynamic_cast<G4THitsMap<G4double>*>(event->GetHCofThisEvent()->GetHC(id));

    if (hits_collection == nullptr)
    {
        G4ExceptionDescription msg;
        msg << "Cannot access hits collection with ID: " << id;
        G4Exception("EventActionHistogram::GetHitsCollection()", "#EA001", FatalException, msg);
    }

    return hits_collection;
}

inline G4double GetSum(const G4THitsMap<G4double>* hits)
{
    return std::accumulate(hits->GetMap()->begin(),
                           hits->GetMap()->end(),
                           0.0 * keV,
                           [](const G4double sum, const std::pair<G4int, G4double*>& p) { return sum + *(p.second); });
}

EventActionHistogram::EventActionHistogram()
    : fIDsCached(false)
{
}

void EventActionHistogram::EndOfEventAction(const G4Event* event)
{
    // TODO: make this configurable from mac file
    constexpr auto min_energy = 0.01 * keV;

    if (!fIDsCached)
    {
        for (const auto& det : detectors)
        {
            fHitCollectionIDs.push_back(G4SDManager::GetSDMpointer()->GetCollectionID(det + "/edep"));
        }
        fIDsCached = true;
    }

    auto* analysis_manager = G4AnalysisManager::Instance();
    const size_t ndets = detectors.size();
    for (int det_id{}; det_id < ndets; ++det_id)
    {
        const G4double edep = GetSum(GetHitsCollection(fHitCollectionIDs.at(det_id), event));

        if (edep > min_energy)
        {
            analysis_manager->FillH1(det_id, edep);
        }
    }

    // G4PrimaryVertex* vertex = event->GetPrimaryVertex();
    // G4PrimaryParticle* par = vertex->GetPrimary();
    // G4ThreeVector vect = par->GetMomentumDirection ();
    // analysisManager->FillH2(0, vect.getTheta(), vect.getPhi());
}
