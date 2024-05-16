#include "ActionInitialization.hh"
#include "CascadeGeneratorAction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionScattering.hh"
#include "RunAction.hh"

namespace G4Horus
{
    namespace
    {
        const auto generator_type_strings =
            std::map<std::string, GeneratorType>{ { "single", GeneratorType::single },
                                                  { "cascade", GeneratorType::cascade },
                                                  { "scattering", GeneratorType::scattering } };
    } // namespace

    auto string_to_generator_type(const std::string& key) -> GeneratorType
    {
        if (generator_type_strings.find(key) == generator_type_strings.end())
        {
            throw std::runtime_error(
                fmt::format("Cannot find an output format with the key {:?}. Please use one of the following keys: {}",
                            key,
                            fmt::join(generator_type_strings | std::views::keys, ", ")));
        }
        return generator_type_strings.at(key);
    }

    auto ActionInitialization::create_run_action() const -> std::unique_ptr<G4UserRunAction>
    {
        return std::make_unique<RunAction>(run_action_setting_, verbose_level_);
    }

    auto ActionInitialization::create_event_action(G4UserRunAction* run_action) const
        -> std::unique_ptr<G4UserEventAction>
    {
        return std::make_unique<EventAction>(dynamic_cast<RunAction*>(run_action), verbose_level_);
    }

    auto ActionInitialization::create_generator_action() const -> std::unique_ptr<G4VUserPrimaryGeneratorAction>
    {
        switch (gen_type_)
        {
            case GeneratorType::single:
                return std::make_unique<PrimaryGeneratorAction>();
            case GeneratorType::cascade:
            {
                auto action = std::make_unique<Cascade::GeneratorAction>(decay_handler_);
                return action;
            }
            case GeneratorType::scattering:
                return std::make_unique<PrimaryGeneratorActionScattering>();
            default:
                throw std::runtime_error("Error ocurred!");
        }
    }

    ActionInitialization::ActionInitialization(GeneratorType gen_type, int verbose_level)
        : gen_type_{ gen_type }
        , verbose_level_{ verbose_level }
    {
    }

    void ActionInitialization::BuildForMaster() const
    {
        // release ownerships. Geant4 takes care of them.
        G4cout << "Running build for master" << G4endl;
        SetUserAction(create_run_action().release());
    }

    void ActionInitialization::Build() const
    {
        G4cout << "Running build for workers" << G4endl;
        // release ownerships. Geant4 takes care of them.
        auto run_action = create_run_action();
        auto event_action = create_event_action(run_action.get());
        SetUserAction(event_action.release());
        SetUserAction(run_action.release());

        SetUserAction(create_generator_action().release());
    }
} // namespace G4Horus
