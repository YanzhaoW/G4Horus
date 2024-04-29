#pragma once

#include "G4UserRunAction.hh"

class G4Run;

class RunActionHistogram : public G4UserRunAction {
  public:
    RunActionHistogram();
    RunActionHistogram(const RunActionHistogram&) = delete;
    RunActionHistogram(RunActionHistogram&&) = delete;
    RunActionHistogram& operator=(const RunActionHistogram&) = delete;
    RunActionHistogram& operator=(RunActionHistogram&&) = delete;

    void BeginOfRunAction(const G4Run* /*aRun*/) override;
    void EndOfRunAction(const G4Run* /*aRun*/) override;
  
  private:
    std::string output_file_name_ = "test.root";
};
