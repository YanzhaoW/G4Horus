#include "G4Horus.hh"
#include "DetectorConstruction.hh"

#include <G4HadronicProcessStore.hh>
#include <G4PhysListFactory.hh>
#include <G4RunManagerFactory.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>
#include <Shielding.hh>

#include <chrono>
#include <filesystem>
#include <fmt/chrono.h>

namespace fs = std::filesystem;
namespace G4Horus
{
    void Application::init()
    {
        G4Random::setTheEngine(random_engine_.get());

        // Silence hadronic processes summary
        G4HadronicProcessStore::Instance()->SetVerbose(verbose_level_);
        run_manager_ = std::unique_ptr<G4RunManager>(G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default));
        run_manager_->SetVerboseLevel(verbose_level_);

        // SetUserInitialization claims the onwership
        init_detector_construction();
        init_physics_list();
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
        auto action = std::make_unique<ActionInitialization>(gen_type_, verbose_level_);
        action->SetDecayHandler(&decay_handler_);
        action->SetRunActionSetting(&run_setting_);
        run_manager_->SetUserInitialization(action.release());
    }

    void Application::init_physics_list()
    {
        auto physics_list_factory = G4PhysListFactory{};
        physics_list_factory.SetVerbose(verbose_level_);
        G4VUserPhysicsList* physics_list = physics_list_factory.GetReferencePhysList(physics_list_str_);
        run_manager_->SetUserInitialization(physics_list);
    }

    void Application::init_detector_construction()
    {

        auto detector_construction = std::make_unique<DetectorConstruction>(false);
        detector_construction->set_detector_distance(detector_distance_);
        run_manager_->SetUserInitialization(detector_construction.release());
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
        auto time_start = std::chrono::steady_clock::now();
        G4cout << "Starting batch mode..." << G4endl;
        auto* ui_manager = G4UImanager::GetUIpointer();
        ui_manager->ApplyCommand("/control/execute " + macro_file_);
        auto time_end = std::chrono::steady_clock::now();
        G4cout << "Batch mode runs succesfully!" << G4endl;
        G4cout << fmt::format("Time spent: {:%H h, %M min, %S sec}", time_end - time_start) << G4endl;
    }
    void Application::set_output_format(std::string_view format)
    {
        if (format.contains("hist"))
        {
            run_setting_.is_hist_enabled = true;
        }
        if (format.contains("soco"))
        {
            run_setting_.is_soco_enabled = true;
        }
        if (format.contains("ntuple"))
        {
            run_setting_.is_tuple_enabled = true;
        }
    }
} // namespace G4Horus
