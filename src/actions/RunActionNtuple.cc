#include "RunActionNtuple.hh"
#include "G4Run.hh"

extern const std::vector<std::string> detectors;

namespace G4Horus
{
    RunActionNtuple::RunActionNtuple(const HistogramRunActionSetting* setting)
        : setting_{ setting }
    {
        analysis_manager_->SetNtupleMerging(true);
        analysis_manager_->SetVerboseLevel(0);
        analysis_manager_->SetNtupleRowWise(false, false);
        analysis_manager_->SetDefaultFileType("root");

        G4cout << "Output: Using " << analysis_manager_->GetType() << " with ntuple" << G4endl;

        analysis_manager_->SetHistoDirectoryName("histograms");
        analysis_manager_->SetNtupleDirectoryName("ntuple");
        analysis_manager_->CreateNtuple("Clover", "Hits");

        for (auto& [key, column_id] : column_id_map_)
        {
            switch (key)
            {
                case G4Horus::NTupleColumn::detector:
                    column_id = analysis_manager_->CreateNtupleSColumn(fmt::format("{}", key));
                    break;
                case G4Horus::NTupleColumn::size:
                    column_id = analysis_manager_->CreateNtupleIColumn(fmt::format("{}", key));
                    break;
                default:
                    column_id = analysis_manager_->CreateNtupleDColumn(fmt::format("{}", key));
                    break;
            }
        }
        analysis_manager_->FinishNtuple();
    }

    void RunActionNtuple::BeginOfRunAction(const G4Run* /*aRun*/) { analysis_manager_->OpenFile(); }

    void RunActionNtuple::EndOfRunAction(const G4Run* /*aRun*/)
    {
        analysis_manager_->Write();
        analysis_manager_->CloseFile();
    }

} // namespace G4Horus
