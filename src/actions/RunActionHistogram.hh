#pragma once

#include "G4UserRunAction.hh"
#include "Shared.hh"

class G4Run;

namespace G4Horus
{
    class RunActionHistogram : public G4UserRunAction
    {
      public:
        explicit RunActionHistogram(const HistogramRunActionSetting* setting);

        ~RunActionHistogram() override = default;
        RunActionHistogram(const RunActionHistogram&) = delete;
        RunActionHistogram(RunActionHistogram&&) = delete;
        auto operator=(const RunActionHistogram&) -> RunActionHistogram& = delete;
        auto operator=(RunActionHistogram&&) -> RunActionHistogram& = delete;

        void BeginOfRunAction(const G4Run* /*aRun*/) override;
        void EndOfRunAction(const G4Run* /*aRun*/) override;

      private:
        std::string output_file_name_ = "test.root";
        const HistogramRunActionSetting* setting_ = nullptr;
    };
} // namespace G4Horus
