#pragma once

#include "G4UserEventAction.hh"
#include <CloverHit.hh>
#include <vector>

namespace G4Horus
{
    class RunAction;
    class EventAction : public G4UserEventAction
    {
      public:
        explicit EventAction(RunAction* run_action, int verbose_level);
        void EndOfEventAction(const G4Event* event) override;

      private:
        int verbose_level_ = 0;
        RunAction* run_action_ = nullptr;

        void write_data(const std::vector<CloverHit>& hits, std::string_view sd_name);
    };
} // namespace G4Horus
