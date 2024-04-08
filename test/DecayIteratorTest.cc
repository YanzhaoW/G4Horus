#include <DecayScheme.hh>
#include <algorithm>
#include <fmt/format.h>
#include <gtest/gtest.h>

// NOLINTBEGIN: cppcoreguidelines-avoid-magic-numbers
namespace
{
    using G4Horus::Cascade::Decay;
    using G4Horus::Cascade::DecayScheme;
    using G4Horus::Cascade::EnergyLevel;
    namespace rng = std::ranges;

    TEST(decay_iterator, forward_backward_count)
    {
        auto decay_scheme = DecayScheme{};
        auto level = EnergyLevel{};
        auto decay = Decay{};

        level.atomic_number = 50;
        level.mass = 110;
        level.energy = 1000.F;
        auto level1 = decay_scheme.add_energy_level(level);

        level.energy = 800.F;
        auto level2 = decay_scheme.add_energy_level(level);

        level.energy = 600.F;
        auto level3 = decay_scheme.add_energy_level(level);

        level.energy = 500.F;
        auto level4 = decay_scheme.add_energy_level(level);

        level.energy = 400.F;
        auto level5 = decay_scheme.add_energy_level(level);

        level.energy = 300.F;
        auto level6 = decay_scheme.add_energy_level(level);

        level.energy = 200.F;
        auto level7 = decay_scheme.add_energy_level(level);

        decay.init_level = level1;
        decay.final_level = level2;
        const auto* ref_decay0 = decay_scheme.add_decay(decay);

        decay.init_level = level2;
        decay.final_level = level3;
        decay_scheme.add_decay(decay);

        decay.init_level = level3;
        decay.final_level = level4;
        decay_scheme.add_decay(decay);

        decay.init_level = level4;
        decay.final_level = level5;
        decay_scheme.add_decay(decay);

        decay.init_level = level5;
        decay.final_level = level6;
        decay_scheme.add_decay(decay);

        decay.init_level = level6;
        decay.final_level = level7;
        const auto* ref_decay1 = decay_scheme.add_decay(decay);

        auto sequence_forward = decay_scheme.construct_forward_sequence(*ref_decay0);
        EXPECT_EQ(std::ranges::distance(sequence_forward), 6);

        auto sequence_backward = decay_scheme.construct_backward_sequence(*ref_decay1);
        EXPECT_EQ(std::ranges::distance(sequence_backward), 6);
    }

    TEST(decay_iterator, random_forward_decay_check)
    {
        auto decay_scheme = DecayScheme{};
        auto level = EnergyLevel{};
        auto decay = Decay{};

        level.atomic_number = 50;
        level.mass = 110;
        level.energy = 1000.F;
        auto level1 = decay_scheme.add_energy_level(level);

        level.energy = 800.F;
        auto level2 = decay_scheme.add_energy_level(level);

        level.energy = 600.F;
        auto level3 = decay_scheme.add_energy_level(level);

        level.energy = 500.F;
        auto level4 = decay_scheme.add_energy_level(level);

        level.energy = 400.F;
        auto level5 = decay_scheme.add_energy_level(level);

        level.energy = 300.F;
        auto level6 = decay_scheme.add_energy_level(level);

        level.energy = 200.F;
        auto level7 = decay_scheme.add_energy_level(level);

        decay.init_level = level1;
        decay.final_level = level2;
        const auto* ref_decay0 = decay_scheme.add_decay(decay);

        decay.init_level = level2;
        decay.final_level = level3;
        decay.intensity = 0.3;
        decay_scheme.add_decay(decay);

        decay.init_level = level3;
        decay.final_level = level4;
        decay_scheme.add_decay(decay);

        decay.init_level = level2;
        decay.final_level = level5;
        decay.intensity = 0.7;
        decay_scheme.add_decay(decay);

        decay.init_level = level5;
        decay.final_level = level6;
        decay_scheme.add_decay(decay);

        decay.init_level = level6;
        decay.final_level = level7;
        decay_scheme.add_decay(decay);

        const auto num_of_loop = 10000;
        auto counts = 0;

        for (const auto _ : rng::views::iota(0, num_of_loop))
        {
            auto sequence = decay_scheme.construct_forward_sequence(*ref_decay0);
            for (const auto& decay : sequence)
            {
                const auto final_energy = decay.final_level->energy;
                if (final_energy == 200.F)
                {
                    ++counts;
                }
            }
        }
        EXPECT_NEAR(static_cast<float>(counts) / num_of_loop, 0.7, 0.02);
    }

} // namespace
// NOLINTEND: cppcoreguidelines-avoid-magic-numbers
