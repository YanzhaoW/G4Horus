#include "RunActionHistogram.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include <fmt/format.h>

extern const std::vector<std::string> detectors;

namespace G4Horus
{
    RunActionHistogram::RunActionHistogram(const HistogramRunActionSetting* setting)
        : setting_{ setting }
    {
        if (setting_ == nullptr)
        {
            throw std::runtime_error{ "Histogram setting is nullptr!" };
        }

        auto* analysis_manager = G4AnalysisManager::Instance();
        analysis_manager->SetDefaultFileType("root");
        analysis_manager->SetFileName("hist");
        analysis_manager->SetVerboseLevel(0);

        // G4cout << "Output: Using " << analysis_manager->GetType() << G4endl;

        // analysis_manager->SetHistoDirectoryName("");
        for (const auto& det : detectors)
        {
            auto hist_id = analysis_manager->CreateH1(
                det, "Edep in " + det, setting_->bin_num, setting_->bin_min, setting_->bin_max);
        }
    }

    void RunActionHistogram::BeginOfRunAction(const G4Run* /*aRun*/)
    {
        auto* analysis_manager = G4AnalysisManager::Instance();
        analysis_manager->OpenFile();
    }

    void RunActionHistogram::EndOfRunAction(const G4Run* /*aRun*/)
    {
        auto* analysis_manager = G4AnalysisManager::Instance();
        analysis_manager->Write();
        analysis_manager->CloseFile();
    }

} // namespace G4Horus
