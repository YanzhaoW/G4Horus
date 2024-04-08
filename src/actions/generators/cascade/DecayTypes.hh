#pragma once

#include <fmt/format.h>
#include <vector>

namespace G4Horus::Cascade
{
    class DecayScheme;

    struct EnergyLevel
    {
        EnergyLevel() = default;
        int atomic_number = 0;
        int mass = 0;
        double energy = 0.F; // keV
        auto operator<=>(const EnergyLevel& other) const = default;
        [[nodiscard]] auto is_same_nuclide(const EnergyLevel& other) const -> bool
        {
            return atomic_number == other.atomic_number and mass == other.mass;
        }
    };

    struct LevelKey
    {
      public:
        static auto null() -> LevelKey { return LevelKey{}; }
        [[nodiscard]] auto is_not_null() const -> bool { return level_ != nullptr; }
        [[nodiscard]] auto get() const -> const EnergyLevel* { return level_; }
        auto operator<=>(const LevelKey& other) const = default;
        auto operator*() const -> const auto& { return *level_; }
        auto operator->() const -> const EnergyLevel* { return level_; }
        // [[nodiscard]] auto is_same_nuclide(const LevelKey& other) const -> bool
        // {
        //     return level_->is_same_nuclide(*other);
        // }

      private:
        const EnergyLevel* level_ = nullptr;

        LevelKey() = default;
        explicit LevelKey(const EnergyLevel* level)
            : level_{ level }
        {
        }
        friend class DecayScheme;
    };

    struct Decay
    {
        Decay(LevelKey init, LevelKey final)
            : init_level{ init }
            , final_level{ final }
        {
        }
        Decay() = default;
        bool is_gamma = true;
        float intensity = 1.F;
        float alpha = 1.F;
        // Energy value may not ba calculable for the decays between two different nuclides
        double energy = 0.F; // keV
        LevelKey init_level = LevelKey::null();
        LevelKey final_level = LevelKey::null();
        auto operator<=>(const Decay& other) const = default;
    };

    // All decays related a level
    struct LevelDecays
    {
        LevelDecays() = default;
        std::vector<Decay*> backward_decays;
        std::vector<Decay*> forward_decays;
    };

    namespace details
    {
        class Sentinel
        {
        };

        template <bool is_forward>
        class Iterator
        {
          public:
            using difference_type = std::ptrdiff_t;
            using value_type = const Decay;

            Iterator() = default;
            Iterator(const Decay* decay, const DecayScheme* scheme)
                : decay_{ decay }
                , scheme_{ scheme }
            {
            }

            auto operator==(const Sentinel&) const -> bool;
            auto operator==(const Iterator& other) const -> bool;
            auto operator*() const -> value_type& { return *decay_; }
            // pre-increment
            auto operator++() -> Iterator&;
            // post-increment
            auto operator++(int) -> Iterator;
            // pre-decrement
            auto operator--() -> Iterator&;
            // post-decrement
            auto operator--(int) -> Iterator;

          private:
            const Decay* decay_ = nullptr;
            const DecayScheme* scheme_ = nullptr;
        };

        template <bool is_forward>
        class Sequence
        {
          public:
            // DecaySequence() = default;
            explicit Sequence(const Decay* decay, const DecayScheme* scheme)
                : decay_ref_{ decay }
                , scheme_{ scheme }
            {
            }

            [[nodiscard]] auto begin() const -> Iterator<is_forward>
            {
                return Iterator<is_forward>{ decay_ref_, scheme_ };
            }
            static auto end() -> Sentinel { return Sentinel{}; }

          private:
            const Decay* decay_ref_;
            const DecayScheme* scheme_ = nullptr;
        };
    } // namespace details
} // namespace G4Horus::Cascade

template <>
class fmt::formatter<G4Horus::Cascade::EnergyLevel>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const G4Horus::Cascade::EnergyLevel& level, FmtContent& ctn) const
    {
        return format_to(ctn.out(), "{{energy level: {} keV, z: {}, n: {}}}", level.energy, level.atomic_number, level.mass);
    }
};

template <>
class fmt::formatter<G4Horus::Cascade::Decay>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const G4Horus::Cascade::Decay& decay, FmtContent& ctn) const
    {
        auto init_string = decay.init_level.is_not_null() ? fmt::format("{}", *decay.init_level) : "NULL";
        auto final_string = decay.final_level.is_not_null() ? fmt::format("{}", *decay.final_level) : "NULL";
        return format_to(ctn.out(),
                         "energy: {} keV, intensity: {}, alpha: {}, is_gamma: {} \n Decay path: {} --> {}",
                         decay.energy,
                         decay.intensity,
                         decay.alpha,
                         decay.is_gamma,
                         init_string,
                         final_string);
    }
};
