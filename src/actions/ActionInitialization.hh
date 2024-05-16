#pragma once

#include "CascadeGeneratorAction.hh"
#include "G4VUserActionInitialization.hh"
#include "Shared.hh"
#include <DecayScheme.hh>
#include <memory>
#include <string>

namespace G4Horus
{
    class Application;

    enum class GeneratorType
    {
        single,
        cascade,
        scattering
    };

    auto string_to_generator_type(const std::string& key) -> GeneratorType;

    class ActionInitialization : public G4VUserActionInitialization
    {
      public:
        explicit ActionInitialization(GeneratorType gen_type = GeneratorType::single, int verbose_level = 0);

        void BuildForMaster() const override;
        void Build() const override;

        void SetDecayHandler(Cascade::DecayHandler* handler) { decay_handler_ = handler; }
        void SetRunActionSetting(const RunActionSetting* hist_setting) { run_action_setting_ = hist_setting; }

        auto GetDecayHandler() -> const Cascade::DecayHandler* { return decay_handler_; }

      private:
        GeneratorType gen_type_;
        int verbose_level_ = 0;
        const Cascade::DecayHandler* decay_handler_ = nullptr;
        const RunActionSetting* run_action_setting_ = nullptr;
        float gun_energy = 0.;

        [[nodiscard]] auto create_run_action() const -> std::unique_ptr<G4UserRunAction>;
        [[nodiscard]] auto create_event_action(G4UserRunAction* run_action) const -> std::unique_ptr<G4UserEventAction>;
        [[nodiscard]] auto create_generator_action() const -> std::unique_ptr<G4VUserPrimaryGeneratorAction>;
    };

} // namespace G4Horus
