#pragma once

#include "DecayEngine.hh"
#include "DecayTypes.hh"
#include <expected>
#include <map>
#include <memory>

namespace G4Horus::Cascade
{
    class DecayScheme
    {
      public:
        DecayScheme() = default;

        void read_json_file(std::string_view filename);
        auto add_energy_level(const EnergyLevel& level) -> LevelKey;
        auto add_decay(const Decay& decay) -> Decay*;
        template <bool is_forward>
        auto construct_decay_sequence(const Decay& decay) const
        {
            return details::Sequence<is_forward>{ &decay, this };
        }
        inline auto construct_forward_sequence(const Decay& decay) const
        {
            return construct_decay_sequence<true>(decay);
        }
        inline auto construct_backward_sequence(const Decay& decay) const
        {
            return construct_decay_sequence<false>(decay);
        }

        // Getters:
        [[nodiscard]] auto get_decay(double energy) const -> std::expected<const Decay*, std::string>;
        auto get_decays_ref(const EnergyLevel& level) -> LevelDecays&;
        [[nodiscard]] auto get_decays(const EnergyLevel& level) const -> const LevelDecays&;
        [[nodiscard]] auto get_num_of_levels() const { return decay_scheme_.size(); }
        [[nodiscard]] auto get_num_of_decays() const { return decays_.size(); }
        [[nodiscard]] auto get_random_engine() const -> const auto& { return decay_engine_; }

      private:
        DecayEngine decay_engine_;
        std::vector<std::unique_ptr<Decay>> decays_;
        // TODO: using hash table could make it faster
        std::map<EnergyLevel, LevelDecays> decay_scheme_;

        void add_to_scheme(Decay* decay);
        static auto check_valid_decay(const Decay& decay) -> bool;
    };

} // namespace G4Horus::Cascade
