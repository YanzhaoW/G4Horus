#include "RunAction.hh"
#include "Definitions.hh"
#include "G4Run.hh"

extern const std::vector<std::string> detectors;

namespace G4Horus
{
    constexpr auto DETECTOR_NAMES = CLOVER_DETECTOR_NAMES;

    RunActionNtuple::RunActionNtuple(const RunActionSetting* setting, int verbose_level)
        : verbose_level_{ verbose_level }
        , setting_{ setting }
    {
        if (setting_ == nullptr)
        {
            throw std::runtime_error{ "Histogram setting is nullptr!" };
        }

        analysis_manager_->SetVerboseLevel(0);
        analysis_manager_->SetDefaultFileType("root");

        if (setting_->is_tuple_enabled)
        {
            setup_ntuple(analysis_manager_);
        }
        if (setting_->is_hist_enabled)
        {
            setup_histogram(analysis_manager_);
        }
        if (setting_->is_soco_enabled)
        {
            setup_soco(analysis_manager_);
        }
    }

    void RunActionNtuple::BeginOfRunAction(const G4Run* /*aRun*/) { analysis_manager_->OpenFile(); }

    void RunActionNtuple::EndOfRunAction(const G4Run* /*aRun*/)
    {
        analysis_manager_->Write();
        analysis_manager_->CloseFile();
    }

    void RunActionNtuple::setup_ntuple(G4AnalysisManager* analysis_manager)
    {
        G4cout << "ntuple output has been enabled" << G4endl;
        analysis_manager->SetNtupleMerging(true);
        analysis_manager->SetNtupleRowWise(false, false);
        analysis_manager->SetNtupleDirectoryName("ntuple");
        analysis_manager->CreateNtuple("Clover", "Hits");

        for (auto& [key, column_id] : column_id_map_)
        {
            switch (key)
            {
                case G4Horus::NTupleColumn::detector:
                    column_id = analysis_manager->CreateNtupleSColumn(fmt::format("{}", key));
                    break;
                case G4Horus::NTupleColumn::size:
                    column_id = analysis_manager->CreateNtupleIColumn(fmt::format("{}", key));
                    break;
                default:
                    column_id = analysis_manager->CreateNtupleDColumn(fmt::format("{}", key));
                    break;
            }
        }
        analysis_manager_->FinishNtuple();
    }

    void RunActionNtuple::setup_histogram(G4AnalysisManager* analysis_manager)
    {
        G4cout << "Histogram output has been enabled" << G4endl;
        analysis_manager->SetHistoDirectoryName("histograms");
        for (auto det : DETECTOR_NAMES)
        {
            const auto hist_id = analysis_manager->CreateH1(
                det.data(), fmt::format("Edep in {}", det), setting_->bin_num, setting_->bin_min, setting_->bin_max);
            histogram_id_map_.try_emplace(det, hist_id);
        }
    }

    void RunActionNtuple::setup_soco(G4AnalysisManager* analysis_manager)
    {
        G4cout << "soco output has been enabled" << G4endl;
        analysis_manager->SetHistoDirectoryName("soco");
        G4cout << "Warning: soco has not been implemented!" << G4endl;
    }
} // namespace G4Horus
