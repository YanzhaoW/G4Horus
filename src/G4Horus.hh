#pragma once

#include "ActionInitialization.hh"
#include "Messenger.hh"
#include <G4MTRunManager.hh>
#include <G4SystemOfUnits.hh>
#include <memory>
#include <string>

class G4UImanager;

namespace G4Horus
{

    class Application
    {
      public:
        Application() = default;

        // setters:
        void set_num_of_threads(int num) { num_of_threads_ = num; }
        void set_visual_mode(bool mode = true) { is_visual_mode_ = mode; }
        void set_macro_file(std::string_view filename) { macro_file_ = filename; }
        void set_output_format(OutputFormat format) { output_format_ = format; }
        void set_generator_type(GeneratorType type) { gen_type_ = type; }
        void set_cascade_mode(bool mode) { decay_handler_.set_enable_sequence(mode); }
        void read_decay_scheme_file(std::string_view filename);
        void set_particle_energy(double energy);
        void set_detector_distance(double distance) { detector_distance_ = distance; };
        void set_time_min(double time) { decay_handler_.set_time_min(time); }
        void set_time_max(double time) { decay_handler_.set_time_max(time); }
        void set_physics_list(std::string_view list_name) { physics_list_ = list_name; }

        // getters:
        auto get_hist_setting_ref() -> auto& { return hist_setting_; }

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
        std::string physics_list_ = "QGSP_BERT_EMV";
        OutputFormat output_format_ = OutputFormat::hist;
        GeneratorType gen_type_ = GeneratorType::single;
        HistogramRunActionSetting hist_setting_;
        std::unique_ptr<G4MTRunManager> run_manager_;
        std::unique_ptr<CLHEP::RanecuEngine> random_engine_ = std::make_unique<CLHEP::RanecuEngine>();
        Messenger messnger_{ this };

        void init_actions();
    };
} // namespace G4Horus
