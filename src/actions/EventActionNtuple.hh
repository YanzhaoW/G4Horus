#pragma once

#include "G4UserEventAction.hh"
#include <vector>

namespace G4Horus
{
    class RunActionNtuple;
    class EventActionNtuple : public G4UserEventAction
    {
      public:
        explicit EventActionNtuple(RunActionNtuple* run_action);
        void EndOfEventAction(const G4Event* event) override;

      private:
        bool fIDsCached;
        RunActionNtuple* run_action_ = nullptr;
        std::vector<int> fHitCollectionIDs;
    };
} // namespace G4Horus
