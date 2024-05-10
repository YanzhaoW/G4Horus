#pragma once

#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UImessenger.hh>
#include <memory>

namespace G4Horus
{
    class Application;
    class Messenger : public G4UImessenger
    {
      public:
        explicit Messenger(Application* app);

      private:
        std::unique_ptr<G4UIdirectory> main_directory_;
        std::unique_ptr<G4UIdirectory> gun_directory_;
        std::unique_ptr<G4UIdirectory> detector_directory_;
        std::unique_ptr<G4UIdirectory> output_directory_;

        // command lists
        std::unique_ptr<G4UIcmdWithoutParameter> do_init_;
        std::unique_ptr<G4UIcmdWithAString> output_type_;
        std::unique_ptr<G4UIcmdWithAString> generator_type_;
        std::unique_ptr<G4UIcmdWithAString> generator_file_name;
        std::unique_ptr<G4UIcmdWithAString> particle_energy_;
        std::unique_ptr<G4UIcmdWithABool> set_cascade_mode_;

        std::unique_ptr<G4UIcmdWithAnInteger> hist_bin_num_;
        std::unique_ptr<G4UIcmdWithADoubleAndUnit> hist_min_;
        std::unique_ptr<G4UIcmdWithADoubleAndUnit> hist_max_;
        std::unique_ptr<G4UIcmdWithADoubleAndUnit> time_max_;
        std::unique_ptr<G4UIcmdWithADoubleAndUnit> time_min_;

        std::unique_ptr<G4UIcmdWithADoubleAndUnit> detector_distance_;
        Application* app_ = nullptr;

        void SetNewValue(G4UIcommand* command, G4String new_values) override;
        void set_list_of_commands();
    };
} // namespace G4Horus
