#include "RunActionHistogram.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include <fmt/format.h>

extern const std::vector<std::string> detectors;

RunActionHistogram::RunActionHistogram()
{
    auto* analysis_manager = G4AnalysisManager::Instance();
    analysis_manager->SetDefaultFileType("root");
    analysis_manager->SetFileName("hist");
    analysis_manager->SetVerboseLevel(0);

    // G4cout << "Output: Using " << analysis_manager->GetType() << G4endl;

    analysis_manager->SetHistoDirectoryName("");
    for (const auto& det : detectors)
    {
        auto hist_id = analysis_manager->CreateH1(det, "Edep in " + det, 20000, 0., 3. * MeV);
    }
    // analysis_manager->CreateH2("angle_dis", "spacial distribution", 200, 0., pi , 200, -pi, pi);
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
