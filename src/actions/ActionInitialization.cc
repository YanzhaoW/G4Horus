#include "ActionInitialization.hh"
#include "CascadeGeneratorAction.hh"
#include "EventActionHistogram.hh"
#include "EventActionNtuple.hh"
#include "EventActionSoco.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionScattering.hh"
#include "RunActionHistogram.hh"
#include "RunActionNtuple.hh"
#include "RunActionSoco.hh"

// Build() and BuildForMaster() have to be able to create new instances of Run and Event Actions.
// To be able to choose different actions, create creators (~factories) for the different types.
namespace G4Horus
{
    namespace
    {
        const auto output_format_strings = std::map<std::string, OutputFormat>{ { "hist", OutputFormat::hist },
                                                                                { "ntuple", OutputFormat::ntuple },
                                                                                { "soco", OutputFormat::soco } };
        const auto generator_type_strings =
            std::map<std::string, GeneratorType>{ { "single", GeneratorType::single },
                                                  { "cascade", GeneratorType::cascade },
                                                  { "scattering", GeneratorType::scattering } };
    } // namespace

    auto string_to_output_format(const std::string& key) -> OutputFormat
    {
        if (output_format_strings.find(key) == output_format_strings.end())
        {
            throw std::runtime_error(
                fmt::format("Cannot find an output format with the key {:?}. Please use one of the following keys: {}",
                            key,
                            fmt::join(output_format_strings | std::views::keys, ", ")));
        }
        return output_format_strings.at(key);
    }

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
        switch (output_format_)
        {
            case OutputFormat::hist:
                return std::make_unique<RunActionHistogram>(histogram_setting_);
            case OutputFormat::ntuple:
                return std::make_unique<RunActionNtuple>();
            case OutputFormat::soco:
                return std::make_unique<RunActionSoco>();
            default:
                throw std::runtime_error("Error ocurred!");
        }
    }

    auto ActionInitialization::create_event_action() const -> std::unique_ptr<G4UserEventAction>
    {
        switch (output_format_)
        {
            case OutputFormat::hist:
                return std::make_unique<EventActionHistogram>();
            case OutputFormat::ntuple:
                return std::make_unique<EventActionNtuple>();
            case OutputFormat::soco:
                return std::make_unique<EventActionSoco>();
            default:
                throw std::runtime_error("Error ocurred!");
        }
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

    ActionInitialization::ActionInitialization(GeneratorType gen_type, OutputFormat mode)
        : gen_type_{ gen_type }
        , output_format_{ mode }
    {
    }

    void ActionInitialization::BuildForMaster() const
    {
        // release ownerships. Geant4 takes care them.
        G4cout << "Running build for master" << G4endl;
        SetUserAction(create_run_action().release());

        // if (gen_type_ == GeneratorType::cascade)
        // {
        //     set_reference_decay();
        // }
    }

    void ActionInitialization::Build() const
    {
        G4cout << "Running build for workers" << G4endl;
        // release ownerships. Geant4 takes care them.
        SetUserAction(create_run_action().release());
        SetUserAction(create_generator_action().release());
        SetUserAction(create_event_action().release());
    }
} // namespace G4Horus
