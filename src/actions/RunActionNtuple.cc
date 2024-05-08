#include "RunActionNtuple.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"

extern const std::vector<std::string> detectors;

namespace G4Horus
{
    RunActionNtuple::RunActionNtuple(const HistogramRunActionSetting* setting)
        : setting_{ setting }
    {
        analysis_manager_ = std::unique_ptr<G4GenericAnalysisManager>(G4AnalysisManager::Instance());
        analysis_manager_->SetNtupleMerging(true);
        analysis_manager_->SetVerboseLevel(0);
        analysis_manager_->SetNtupleRowWise(false, false);

        G4cout << "Output: Using " << analysis_manager_->GetType() << " with ntuple" << G4endl;

        analysis_manager_->SetHistoDirectoryName("histograms");
        analysis_manager_->SetNtupleDirectoryName("ntuple");
        analysis_manager_->CreateNtuple("Horus", "Edep");
        for (const auto& det : detectors)
        {
            auto hist_id = analysis_manager_->CreateH1(
                det, "Edep in " + det, setting_->bin_num, setting_->bin_min, setting_->bin_max);
            analysis_manager_->CreateNtupleDColumn(det);
        }
        analysis_manager_->FinishNtuple();
    }

    void RunActionNtuple::BeginOfRunAction(const G4Run* /*aRun*/)
    {
        analysis_manager_->OpenFile();
    }

    void RunActionNtuple::EndOfRunAction(const G4Run* /*aRun*/)
    {
        analysis_manager_->Write();
        analysis_manager_->CloseFile();
    }

} // namespace G4Horus
