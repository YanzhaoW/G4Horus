#include "Messenger.hh"
#include "G4Horus.hh"
#include <G4SystemOfUnits.hh>

namespace G4Horus
{

    Messenger::Messenger(Application* app)
        : app_{ app }
    {
        main_directory_ = std::make_unique<G4UIdirectory>("/g4horus/");
        main_directory_->SetGuidance("User controls on G4Horus simulation.");

        gun_directory_ = std::make_unique<G4UIdirectory>("/g4horus/gun/");
        gun_directory_->SetGuidance("User controls on G4Horus generator.");

        detector_directory_ = std::make_unique<G4UIdirectory>("/g4horus/detector/");
        detector_directory_->SetGuidance("User controls on G4Horus detector parameters.");

        output_directory_ = std::make_unique<G4UIdirectory>("/g4horus/output/");
        output_directory_->SetGuidance("User controls on G4Horus output parameters.");

        set_list_of_commands();
    }

    void Messenger::set_list_of_commands()
    {
        do_init_ = std::make_unique<G4UIcmdWithoutParameter>("/g4horus/init", this);
        do_init_->SetGuidance("Initialize G4Horus.");
        do_init_->AvailableForStates(G4State_PreInit);

        output_type_ = std::make_unique<G4UIcmdWithAString>("/g4horus/output/type", this);
        output_type_->SetGuidance("Specify the output format.");
        output_type_->SetParameterName("type", true);
        output_type_->SetDefaultValue("hist");
        output_type_->AvailableForStates(G4State_PreInit);

        hist_bin_num_ = std::make_unique<G4UIcmdWithAnInteger>("/g4horus/output/hist_bin_num", this);
        hist_bin_num_->SetGuidance("Specify the number of bins in histogram.");
        hist_bin_num_->SetParameterName("distance", true);
        hist_bin_num_->SetDefaultValue(1);

        hist_min_ = std::make_unique<G4UIcmdWithADoubleAndUnit>("/g4horus/output/hist_min", this);
        hist_min_->SetGuidance("Specify the minimal value of the histogram.");
        hist_min_->SetParameterName("hist_min", true);
        hist_min_->SetDefaultUnit("MeV");
        hist_min_->SetDefaultValue(0.);
        hist_min_->SetUnitCandidates("eV keV MeV GeV");

        hist_max_ = std::make_unique<G4UIcmdWithADoubleAndUnit>("/g4horus/output/hist_max", this);
        hist_max_->SetGuidance("Specify the maximal value of the histogram.");
        hist_max_->SetParameterName("hist_max", true);
        hist_max_->SetDefaultUnit("MeV");
        hist_max_->SetDefaultValue(1.);
        hist_max_->SetUnitCandidates("eV keV MeV GeV");

        detector_distance_ = std::make_unique<G4UIcmdWithADoubleAndUnit>("/g4horus/detector/distance", this);
        detector_distance_->SetGuidance("Set the distance of detectors.");
        detector_distance_->SetParameterName("distance", true);
        detector_distance_->SetDefaultUnit("cm");
        detector_distance_->SetDefaultValue(1.);
        detector_distance_->SetUnitCandidates("cm mm m");
        detector_distance_->AvailableForStates(G4State_PreInit);

        generator_file_name = std::make_unique<G4UIcmdWithAString>("/g4horus/gun/read_decay_scheme", this);
        generator_file_name->SetGuidance("Specify the input file.");
        generator_file_name->SetParameterName("decay_scheme", true);
        generator_file_name->SetDefaultValue("../test/test.json");
        generator_file_name->AvailableForStates(G4State_PreInit, G4State_Idle);

        generator_type_ = std::make_unique<G4UIcmdWithAString>("/g4horus/gun/type", this);
        generator_type_->SetGuidance("Set the generator type.");
        generator_type_->SetParameterName("generator_type", true);
        generator_type_->SetDefaultValue("single");
        generator_type_->AvailableForStates(G4State_PreInit);

        particle_energy_ = std::make_unique<G4UIcmdWithAString>("/g4horus/gun/energy", this);
        particle_energy_->SetGuidance("Set the energy of beam.");
        particle_energy_->SetParameterName("energy", true);
        // particle_energy_->SetDefaultUnit("keV");
        particle_energy_->SetDefaultValue("0.");
        // particle_energy_->SetUnitCandidates("eV keV MeV GeV");
        particle_energy_->AvailableForStates(G4State_PreInit, G4State_Idle);

        set_cascade_mode_ = std::make_unique<G4UIcmdWithABool>("/g4horus/gun/cascade", this);
        set_cascade_mode_->SetGuidance("Set cascade mode.");
        set_cascade_mode_->AvailableForStates(G4State_PreInit, G4State_Idle);
    }

    void Messenger::SetNewValue(G4UIcommand* command, G4String new_values)
    {
        if (command == do_init_.get())
        {
            app_->init();
        }

        if (command == output_type_.get())
        {
            app_->set_output_format(string_to_output_format(new_values));
        }

        if (command == hist_bin_num_.get())
        {
            app_->get_hist_setting_ref().bin_num = hist_bin_num_->GetNewIntValue(new_values);
        }

        if (command == hist_max_.get())
        {
            app_->get_hist_setting_ref().bin_max = hist_max_->GetNewDoubleValue(new_values);
        }

        if (command == hist_min_.get())
        {
            app_->get_hist_setting_ref().bin_min = hist_min_->GetNewDoubleValue(new_values);
        }

        if (command == generator_file_name.get())
        {
            app_->read_decay_scheme_file(new_values);
        }

        if (command == generator_type_.get())
        {
            app_->set_generator_type(string_to_generator_type(new_values));
        }

        if (command == particle_energy_.get())
        {
            app_->set_particle_energy(std::stod(new_values));
        }

        if (command == set_cascade_mode_.get())
        {
            app_->set_cascade_mode(set_cascade_mode_->GetNewBoolValue(new_values));
        }

        if (command == detector_distance_.get())
        {
            app_->set_detector_distance(detector_distance_->GetNewDoubleValue(new_values));
        }
    }
} // namespace G4Horus
