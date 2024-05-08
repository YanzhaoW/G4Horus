#pragma once

#include "CascadeGeneratorAction.hh"
#include "G4VUserActionInitialization.hh"
#include "RunActionHistogram.hh"
#include <DecayScheme.hh>
#include <memory>
#include <string>

namespace G4Horus
{
    class Application;
    enum class OutputFormat
    {
        hist,
        ntuple,
        soco
    };

    enum class GeneratorType
    {
        single,
        cascade,
        scattering
    };

    auto string_to_output_format(const std::string& key) -> OutputFormat;
    auto string_to_generator_type(const std::string& key) -> GeneratorType;

    class ActionInitialization : public G4VUserActionInitialization
    {
      public:
        explicit ActionInitialization(GeneratorType gen_type = GeneratorType::single,
                                      OutputFormat mode = OutputFormat::hist);

        void BuildForMaster() const override;
        void Build() const override;

        void SetDecayHandler(Cascade::DecayHandler* handler) { decay_handler_ = handler; }
        void SetHistogramSetting(const HistogramRunActionSetting* hist_setting) { histogram_setting_ = hist_setting; }

        auto GetDecayHandler() -> const Cascade::DecayHandler* { return decay_handler_; }

      private:
        GeneratorType gen_type_;
        const Cascade::DecayHandler* decay_handler_ = nullptr;
        const HistogramRunActionSetting* histogram_setting_ = nullptr;
        OutputFormat output_format_;
        float gun_energy = 0.;

        [[nodiscard]] auto create_run_action() const -> std::unique_ptr<G4UserRunAction>;
        [[nodiscard]] auto create_event_action(G4UserRunAction* run_action) const -> std::unique_ptr<G4UserEventAction>;
        [[nodiscard]] auto create_generator_action() const -> std::unique_ptr<G4VUserPrimaryGeneratorAction>;
    };

} // namespace G4Horus
