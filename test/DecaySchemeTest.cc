#include <DecayScheme.hh>
#include <gtest/gtest.h>
#include <ranges>

// NOLINTBEGIN: cppcoreguidelines-avoid-magic-numbers
namespace
{
    using G4Horus::Cascade::Decay;
    using G4Horus::Cascade::DecayScheme;
    using G4Horus::Cascade::EnergyLevel;
    namespace details = G4Horus::Cascade::details;
    template <bool is_forward>
    using Sequence = G4Horus::Cascade::details::Sequence<is_forward>;

    TEST(decay_scheme, static_checking)
    {
        static_assert(std::bidirectional_iterator<details::Iterator<true>>);
        static_assert(std::bidirectional_iterator<std::ranges::iterator_t<Sequence<true>>>);
        static_assert(std::ranges::bidirectional_range<Sequence<true>>);
    }

    TEST(decay_scheme, add_energy_level_no_dup)
    {
        auto decay_scheme = DecayScheme{};
        auto level = EnergyLevel{};
        level.atomic_number = 50;
        level.mass = 110;
        level.energy = 400.;
        decay_scheme.add_energy_level(level);
        ASSERT_EQ(decay_scheme.get_num_of_levels(), 1);

        decay_scheme.add_energy_level(level);
        ASSERT_EQ(decay_scheme.get_num_of_levels(), 1);

        level.energy = 400.01;
        decay_scheme.add_energy_level(level);
        ASSERT_EQ(decay_scheme.get_num_of_levels(), 2);
    }

    TEST(decay_scheme, add_decay_no_dup)
    {
        auto decay_scheme = DecayScheme{};
        auto level = EnergyLevel{};
        level.atomic_number = 50;
        level.mass = 110;
        level.energy = 500.;
        auto level1 = decay_scheme.add_energy_level(level);

        level.energy = 400.;
        auto level2 = decay_scheme.add_energy_level(level);

        ASSERT_EQ(decay_scheme.get_num_of_levels(), 2);

        auto decay = Decay{ level1, level2 };
        decay.intensity = 1.F;
        decay.alpha = 0.5F;
        decay_scheme.add_decay(decay);
        ASSERT_EQ(decay_scheme.get_num_of_decays(), 1);

        decay_scheme.add_decay(decay);
        ASSERT_EQ(decay_scheme.get_num_of_decays(), 1);
    }

    TEST(decay_scheme, check_level_decays)
    {
        auto decay_scheme = DecayScheme{};
        auto level = EnergyLevel{};
        auto decay = Decay{};

        level.atomic_number = 50;
        level.mass = 110;
        level.energy = 500.F;
        auto level1 = decay_scheme.add_energy_level(level);

        level.energy = 400.F;
        auto level2 = decay_scheme.add_energy_level(level);

        level.energy = 700.F;
        auto level3 = decay_scheme.add_energy_level(level);

        level.energy = 300.F;
        auto level4 = decay_scheme.add_energy_level(level);

        decay.init_level = level1;
        decay.final_level = level2;
        decay_scheme.add_decay(decay);

        decay.init_level = level3;
        decay.final_level = level1;
        decay_scheme.add_decay(decay);

        decay.init_level = level1;
        decay.final_level = level4;
        decay_scheme.add_decay(decay);

        decay.init_level = level2;
        decay.final_level = level4;
        decay_scheme.add_decay(decay);

        const auto& level_decays1 = decay_scheme.get_decays(*level1.get());
        EXPECT_EQ(level_decays1.forward_decays.size(), 2);
        EXPECT_EQ(level_decays1.backward_decays.size(), 1);

        const auto& level_decays2 = decay_scheme.get_decays(*level2.get());
        EXPECT_EQ(level_decays2.forward_decays.size(), 1);
        EXPECT_EQ(level_decays2.backward_decays.size(), 1);

        const auto& level_decays3 = decay_scheme.get_decays(*level3.get());
        EXPECT_EQ(level_decays3.forward_decays.size(), 1);
        EXPECT_EQ(level_decays3.backward_decays.size(), 0);

        const auto& level_decays4 = decay_scheme.get_decays(*level4.get());
        EXPECT_EQ(level_decays4.forward_decays.size(), 0);
        EXPECT_EQ(level_decays4.backward_decays.size(), 2);
    }
} // namespace
// NOLINTEND: cppcoreguidelines-avoid-magic-numbers
