#include <CLI/CLI.hpp>
#include <G4Horus.hh>

auto main(int argc, char** argv) -> int
{
    auto program = CLI::App{ "G4Horus - Gamma-ray detection efficiency simulation." };

    auto num_of_threads = 1;
    auto macro_file = std::string{};
    auto is_visual_mode = false;
    auto verbose_level = 0;

    program.add_flag("-d,--display-visual", is_visual_mode, "Run in visual mode")->capture_default_str();
    program.add_option("-t,--threads", num_of_threads, "Number of threads for event processing")
        ->capture_default_str()
        ->check(CLI::PositiveNumber);
    program.add_option("-v,--verbose", verbose_level, "Set verbose level")->capture_default_str();
    program.add_option("-m,--macro", macro_file, "Process macro file in batch mode")
        ->default_str("")
        ->check(CLI::ExistingFile);

    CLI11_PARSE(program, argc, argv);

    auto g4horus = G4Horus::Application{};
    g4horus.set_num_of_threads(num_of_threads);
    g4horus.set_visual_mode(is_visual_mode);
    g4horus.set_macro_file(macro_file);
    g4horus.set_verbose_level(verbose_level);

    if (is_visual_mode)
    {
        G4Horus::Application::launch_visual_mode(argc, argv);
    }
    else
    {
        g4horus.launch_batch_mode();
    }

    // g4horus.init();

    return 0;
}
