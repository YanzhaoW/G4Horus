#pragma once

#include "G4SystemOfUnits.hh"
#include <G4THitsMap.hh>
#include <G4ThreeVector.hh>
#include <G4VHit.hh>
#include <fmt/format.h>
#include <ranges>
#include <unordered_map>

namespace G4Horus
{
    class CloverHit : public G4VHit
    {
      public:
        CloverHit() = default;

        static constexpr auto collection_name = std::string_view{ "CloverHits" };

        // Setters:
        void set_time(double time) { time_ = time; }
        void set_energy(double energy) { energy_ = energy; }
        void set_position(const G4ThreeVector& position) { position_ = position; }

        // Getters:
        [[nodiscard]] auto get_time() const -> double { return time_; }
        [[nodiscard]] auto get_energy() const -> double { return energy_; }
        [[nodiscard]] auto get_position() const -> const auto& { return position_; }

      private:
        double time_ = 0.;
        double energy_ = 0. * keV;
        G4ThreeVector position_{};

        void Draw() override{};
        void Print() override{};

        [[nodiscard]] auto GetAttDefs() const -> const std::map<G4String, G4AttDef>* override { return nullptr; }
        [[nodiscard]] auto CreateAttValues() const -> std::vector<G4AttValue>* override { return nullptr; }
    };

    template <typename KeyType, typename ValueType>
    class HitsMap : public G4HitsCollection
    {
      public:
        HitsMap() = default;
        HitsMap(std::string_view name, std::string_view col_name)
            : G4HitsCollection(name.data(), col_name.data())
        {
        }

        void clear() { map_.clear(); }
        void clear_values()
        {
            for (auto& [_, key] : map_)
            {
                key.clear();
            }
        }

        auto add_key(KeyType key) { return map_.try_emplace(key, std::vector<ValueType>{}); }

        template <int size>
        void add_keys(std::array<KeyType, size> keys)
        {
            for (const auto key : keys)
            {
                add_key(key);
            }
        }

        auto emplace_value(KeyType key) -> auto&
        {
            auto iter = map_.find(key);
            if (iter == map_.end())
            {
                throw std::runtime_error(
                    fmt::format("Cannot find the key {} in the collection {:?}. \n Available keys: {}",
                                key,
                                GetName(),
                                fmt::join(map_ | std::ranges::views::keys, ", ")));
            }
            return iter->second.emplace_back();
        }

        // getters:
        auto get_map() const -> const auto& { return map_; }

      private:
        std::unordered_map<KeyType, std::vector<ValueType>> map_;
    };

    using CloverHitsMap = HitsMap<int, CloverHit>;
} // namespace G4Horus
