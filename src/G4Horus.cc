#include "G4Horus.hh"
#include "DetectorConstruction.hh"
#include "G4HadronicProcessStore.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "Shielding.hh"
// #include "QGSP_INCLXX.hh"

#include <filesystem>

namespace fs = std::filesystem;
namespace G4Horus
{
    void Application::init()
    {
        G4Random::setTheEngine(random_engine_.get());

        // Silence hadronic processes summary
        G4HadronicProcessStore::Instance()->SetVerbose(verbose_level_);

        run_manager_ = std::make_unique<G4MTRunManager>();
        run_manager_->SetVerboseLevel(verbose_level_);

        // SetUserInitialization claims the onwership

        auto detector_construction = std::make_unique<DetectorConstruction>(false);
        detector_construction->set_detector_distance(detector_distance_);
        run_manager_->SetUserInitialization(detector_construction.release());

        // run_manager->SetUserInitialization(new PhysicsList());

        auto shield = std::make_unique<Shielding>(0);
        run_manager_->SetUserInitialization(shield.release()); // includes G4RadioactiveDecay

        init_actions();

        run_manager_->SetNumberOfThreads(num_of_threads_);
        G4cout << "G4Horus Initialization begins..." << G4endl;
        run_manager_->Initialize();
        G4cout << "G4Horus Initialization finished." << G4endl;
    }

    void Application::read_decay_scheme_file(std::string_view filename)
    {
        auto script_folder = fs::path{ macro_file_ }.parent_path();
        auto scheme_path = fs::path{ filename };
        auto decay_scheme = std::make_unique<Cascade::DecayScheme>();

        auto scheme_absolute_path = scheme_path.is_absolute() ? scheme_path : script_folder / scheme_path;
        decay_scheme->read_json_file(scheme_absolute_path.c_str());
        decay_handler_.set_decay_scheme(std::move(decay_scheme));
    }

    void Application::set_particle_energy(double energy)
    {
        fmt::print("Set the energy to be {}\n", energy);
        particle_energy_ = energy;
        if (gen_type_ == GeneratorType::cascade)
        {
            decay_handler_.set_decay(particle_energy_);
        }
    }

    void Application::init_actions()
    {
        auto action = std::make_unique<ActionInitialization>(gen_type_, output_format_);
        action->SetDecayHandler(&decay_handler_);
        run_manager_->SetUserInitialization(action.release());
    }

    void Application::launch_visual_mode(int argc, char** argv)
    {
        auto* ui_manager = G4UImanager::GetUIpointer();
        auto vis_manager = G4VisExecutive{};
        vis_manager.Initialise();
        auto ui_exe = G4UIExecutive{ argc, argv };
        ui_manager->ApplyCommand("/control/execute ../scripts/G4Horus.mac");
        ui_exe.SessionStart();
    }

    void Application::launch_batch_mode()
    {
        G4cout << "Apply macro file" << G4endl;
        auto* ui_manager = G4UImanager::GetUIpointer();
        ui_manager->ApplyCommand("/control/execute " + macro_file_);
        G4cout << "Macro file is applied" << G4endl;
    }
} // namespace G4Horus
