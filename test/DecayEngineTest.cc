#include <DecayEngine.hh>
#include <DecayTypes.hh>
#include <fmt/format.h>
#include <gtest/gtest.h>

// NOLINTBEGIN: cppcoreguidelines-avoid-magic-numbers

namespace
{
    using G4Horus::Cascade::Decay;
    using G4Horus::Cascade::DecayEngine;

    TEST(decay_engine, single_check)
    {
        auto engine = DecayEngine{};
        auto decays = std::vector<Decay*>{};

        auto decay1 = std::make_unique<Decay>();
        decay1->intensity = 0.4;
        decays.push_back(decay1.get());
        auto decay2 = std::make_unique<Decay>();
        decay2->intensity = 0.8;
        decays.push_back(decay2.get());
        auto decay3 = std::make_unique<Decay>();
        decay3->intensity = 1000.2;
        decays.push_back(decay3.get());
        auto decay4 = std::make_unique<Decay>();
        decay4->intensity = 2.4;
        decays.push_back(decay4.get());
        auto decay5 = std::make_unique<Decay>();
        decay5->intensity = 1.4;
        decays.push_back(decay5.get());

        auto* decay_selected = engine.random_select_decay(decays);
        EXPECT_FLOAT_EQ(decay_selected->intensity, 1000.2);
    }

    TEST(decay_engine, randomization_check)
    {
        auto engine = DecayEngine{};
        auto decays = std::vector<Decay*>{};

        const auto intensities = std::array{ 0.4F, 0.8F, 1.2F, 2.4F, 1.4F };
        const auto sum = std::accumulate(intensities.begin(), intensities.end(), 0.F);

        auto decay1 = std::make_unique<Decay>();
        decay1->intensity = intensities[0];
        decays.push_back(decay1.get());
        auto decay2 = std::make_unique<Decay>();
        decay2->intensity = intensities[1];
        decays.push_back(decay2.get());
        auto decay3 = std::make_unique<Decay>();
        decay3->intensity = intensities[2];
        decays.push_back(decay3.get());
        auto decay4 = std::make_unique<Decay>();
        decay4->intensity = intensities[3];
        decays.push_back(decay4.get());
        auto decay5 = std::make_unique<Decay>();
        decay5->intensity = intensities[4];
        decays.push_back(decay5.get());

        auto histograms = std::unordered_map<const Decay*, int>{
            { decay1.get(), 0 }, { decay2.get(), 0 }, { decay3.get(), 0 }, { decay4.get(), 0 }, { decay5.get(), 0 }
        };
        const auto num_of_loops = 10000000;
        for (int loop_i{ 0 }; loop_i < num_of_loops; ++loop_i)
        {
            const auto* decay_selected = engine.random_select_decay(decays);
            ++(histograms.at(decay_selected));
        }
        for (auto [decay, stat] : histograms)
        {
            EXPECT_NEAR(decay->intensity / sum, static_cast<float>(stat) / num_of_loops, 0.01);
        }
    }
} // namespace
// NOLINTEND: cppcoreguidelines-avoid-magic-numbers
