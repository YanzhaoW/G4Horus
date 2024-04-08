#pragma once

#include <fmt/format.h>
#include <mutex>
#include <random>
#include <ranges>
#include <vector>

namespace G4Horus::Cascade
{
    class Decay;
    namespace rng = std::ranges;
    using rng::bidirectional_range;

    class DecayEngine
    {
      public:
        DecayEngine() = default;
        auto random_select_decay(const std::vector<Decay*>& decays) const -> const Decay*;

      private:
        std::random_device ran_device_;
        mutable std::mt19937_64 random_gen_{ ran_device_() };
        mutable std::mutex random_gen_mutex_guard_;

        auto generate_random_number(auto& distribution) const
        {
            auto scoped_lock = std::scoped_lock{ random_gen_mutex_guard_ };
            return distribution(random_gen_);
        }

        auto random_select(const rng::view auto& item_intensity_view, float sum) const -> auto*
        {
            std::uniform_real_distribution<float> distribution{ 0., sum };
            auto random_num = generate_random_number(distribution);

            auto accumulated = 0.F;

            auto res = rng::find_if(item_intensity_view,
                                    [random_num, sum, &accumulated](const auto& item_intensity) -> bool
                                    {
                                        accumulated += std::get<1>(item_intensity);
                                        return random_num <= accumulated;
                                    });
            if (res == item_intensity_view.end())
            {
                throw std::runtime_error(
                    fmt::format("Cannot random select item with the number: {}, with accumulated value: {}",
                                random_num,
                                accumulated));
            }
            return std::get<0>(*res);
        }
    };

} // namespace G4Horus::Cascade
