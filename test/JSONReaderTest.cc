#include <DecayJSONReader.hh>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <ranges>

namespace
{
    using G4Horus::Cascade::DecayScheme;

    TEST(JSON_reader, check_file)
    {
        auto decay_scheme = DecayScheme();
        decay_scheme.read_json_file("../../test/test.json");

        const auto decay = decay_scheme.get_decay(212.43);

        if (not decay.has_value())
        {
            throw std::runtime_error(decay.error());
        }
        auto forward_seq = decay_scheme.construct_forward_sequence(*decay.value());
        auto backward_seq = decay_scheme.construct_backward_sequence(*decay.value());
        fmt::print("levels : {}, decays: {}\n", decay_scheme.get_num_of_levels(), decay_scheme.get_num_of_decays());

        fmt::print("Simulating decay sequence for the decay: {}\n\n", *decay.value());

        for (const auto& decay : forward_seq)
        {
            fmt::print("Forward decay: {}\n", decay);
        }
        fmt::print("\n");
        for (const auto& decay : backward_seq | std::ranges::views::drop(1))
        {
            fmt::print("Backward decay: {}\n", decay);
        }
    }

} // namespace
