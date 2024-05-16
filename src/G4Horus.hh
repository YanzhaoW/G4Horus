#pragma once

#include "ActionInitialization.hh"
#include "Messenger.hh"

#include "Definitions.hh"
#include <CLHEP/Random/RanecuEngine.h>
#include <G4RunManager.hh>
#include <G4SystemOfUnits.hh>
#include <memory>
#include <string>

class G4UImanager;

namespace G4Horus
{
    struct nop
    {
        template <typename T>
        void operator()(T const& /*unused*/) const noexcept
        {
            G4cout << "No deletion of run manager" << G4endl;
        }
    };

    class Application
    {
      public:
        Application() = default;

        // setters:
        void set_num_of_threads(int num) { num_of_threads_ = num; }
        void set_visual_mode(bool mode = true) { is_visual_mode_ = mode; }
        void set_macro_file(std::string_view filename) { macro_file_ = filename; }
        void set_output_format(std::string_view format);
        void set_generator_type(GeneratorType type) { gen_type_ = type; }
        void set_cascade_mode(bool mode) { decay_handler_.set_enable_sequence(mode); }
        void read_decay_scheme_file(std::string_view filename);
        void set_particle_energy(double energy);
        void set_detector_distance(double distance) { detector_distance_ = distance; };
        void set_time_min(double time) { decay_handler_.set_time_min(time); }
        void set_time_max(double time) { decay_handler_.set_time_max(time); }
        void set_physics_list(std::string_view list_name) { physics_list_str_ = list_name; }
        void set_verbose_level(int verbose_level) { verbose_level_ = verbose_level; }

        void set_hist_bin_num(int bin_num) { run_setting_.bin_num = bin_num; }
        void set_hist_max(double max_val) { run_setting_.bin_max = max_val; }
        void set_hist_min(double min_val) { run_setting_.bin_min = min_val; }

        // interfaces:
        void init();
        void launch();
        static void launch_visual_mode(int argc, char** argv);
        void launch_batch_mode();

      private:
        bool is_visual_mode_ = false;
        int num_of_threads_ = 1;
        int verbose_level_ = 0;
        double detector_distance_ = 1. * cm;
        double particle_energy_ = 0.;
        Cascade::DecayHandler decay_handler_;
        std::string macro_file_;
        std::string physics_list_str_{ DEFAULT_PHYSICS_LIST };
        GeneratorType gen_type_ = GeneratorType::single;
        RunActionSetting run_setting_;
        std::unique_ptr<G4RunManager> run_manager_;
        std::unique_ptr<CLHEP::RanecuEngine> random_engine_ = std::make_unique<CLHEP::RanecuEngine>();
        Messenger messnger_{ this };

        void init_actions();
        void init_physics_list();
        void init_detector_construction();
    };
} // namespace G4Horus
