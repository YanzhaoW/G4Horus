#pragma once

#include "G4UserRunAction.hh"
#include "Shared.hh"
#include <memory>
#include <G4GenericAnalysisManager.hh>

class G4Run;

namespace G4Horus
{
    class RunActionNtuple : public G4UserRunAction
    {
      public:
        explicit RunActionNtuple(const G4Horus::HistogramRunActionSetting* setting);

        void BeginOfRunAction(const G4Run* /*aRun*/) override;
        void EndOfRunAction(const G4Run* /*aRun*/) override;

      private:
        const HistogramRunActionSetting* setting_ = nullptr;
        std::unique_ptr<G4GenericAnalysisManager> analysis_manager_;
    };
} // namespace G4Horus
