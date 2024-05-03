#include "DecayScheme.hh"
#include "DecayJSONReader.hh"
#include <iostream>

namespace G4Horus::Cascade
{
    namespace
    {
        template <typename T>
        auto find_unique_ptr(const T& target, const std::vector<std::unique_ptr<T>>& collection) -> auto*
        {
            auto iter = rng::find_if(collection, [&target](const auto& item) { return *item == target; });
            return (iter == collection.end()) ? nullptr : iter->get();
        }
    } // namespace

    auto DecayScheme::add_energy_level(const EnergyLevel& level) -> LevelKey
    {
        const auto& new_level = decay_scheme_.try_emplace(level, LevelDecays{}).first->first;
        return LevelKey{ &new_level };
    }

    auto DecayScheme::check_valid_decay(const Decay& decay) -> bool
    {
        if (not decay.init_level->is_same_nuclide(*decay.final_level))
        {
            return true;
        }
        return decay.init_level->energy > decay.final_level->energy;
    }

    auto DecayScheme::add_decay(const Decay& decay) -> Decay*
    {
        auto* decay_existed = find_unique_ptr(decay, decays_);
        if (decay_existed != nullptr)
        {
            // TODO: print out level
            std::cout << "Decay has been already existed!\n";
            return decay_existed;
        }

        if (not check_valid_decay(decay))
        {
            // TODO: print out level
            throw std::logic_error(
                fmt::format("The following decay is invalid: {}\n Energy value in initial state ({} keV) is smaller "
                            "than the one from the final state ({} keV).",
                            decay,
                            decay.init_level->energy,
                            decay.final_level->energy));
        }

        auto& new_decay = decays_.emplace_back(std::make_unique<Decay>(decay));
        add_to_scheme(new_decay.get());
        return new_decay.get();
    }

    void DecayScheme::read_json_file(std::string_view filename)
    {
        auto json_reader = JSONReader(filename);
        json_reader.parse(*this);
        fmt::print("Decay scheme has been successfully read from the file {:?}\n", filename);
        fmt::print("Decay scheme info: \n\t Number of decays: {} \n\t Number of energy levels: {}\n",
                   get_num_of_decays(),
                   get_num_of_levels());
        fmt::print("\t\nDecay energies: [{}]",
                   fmt::join(decays_ | rng::views::transform([](const auto& decay) { return decay->energy; }), ", "));
    }

    auto DecayScheme::get_decays(const EnergyLevel& level) const -> const LevelDecays&
    {
        auto iter = decay_scheme_.find(level);
        if (iter == decay_scheme_.end())
        {
            // TODO: print out level
            throw std::runtime_error("Cannot find decays related to the level");
        }
        return iter->second;
    }

    auto DecayScheme::get_decays_ref(const EnergyLevel& level) -> LevelDecays&
    {
        auto iter = decay_scheme_.find(level);
        if (iter == decay_scheme_.end())
        {
            // TODO print out level
            throw std::runtime_error("Cannot find decays related to the level");
        }
        return iter->second;
    }

    void DecayScheme::add_to_scheme(Decay* decay)
    {
        if (decay->init_level.is_not_null())
        {
            get_decays_ref(*decay->init_level).forward_decays.push_back(decay);
        }
        if (decay->final_level.is_not_null())
        {
            get_decays_ref(*decay->final_level).backward_decays.push_back(decay);
        }
    }

    auto DecayScheme::get_decay(double energy) const -> std::expected<const Decay*, std::string>
    {

        auto is_energy_same = [energy](const auto& decay) -> bool { return decay.energy == energy; };
        auto to_reference = [](const auto& decay_unique_ptr) -> const Decay& { return *decay_unique_ptr; };
        auto levels_view = decays_ | rng::views::transform(to_reference) | rng::views::filter(is_energy_same);

        auto size = rng::distance(levels_view);
        if (size == 0)
        {
            return std::unexpected(fmt::format("Cannot find any decay with energy = {} keV\n", energy));
        }
        if (size > 1)
        {
            return std::unexpected(fmt::format(
                "find more energy levels with the decays equal to {}:\n {}\n", energy, fmt::join(levels_view, "\n")));
        }
        return &(*levels_view.begin());
    }
} // namespace G4Horus::Cascade
