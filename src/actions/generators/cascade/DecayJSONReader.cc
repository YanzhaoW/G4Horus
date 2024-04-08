#define JSON_USE_IMPLICIT_CONVERSIONS 0

#include "DecayJSONReader.hh"
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace G4Horus::Cascade
{
    namespace
    {
        auto parse_json_file(std::string_view filename)
        {
            auto input_stream = std::ifstream(filename.data(), std::ios::in);
            if (not input_stream.is_open())
            {
                throw std::runtime_error(fmt::format("Cannot read the file {:?}!", filename));
            }
            return json::parse(input_stream);
        }

    } // namespace

    void to_json(json& json_obj, const JSONEntry& entry)
    {
        json_obj = json{ { "is_gamma", entry.is_gamma },
                         { "parent_z", entry.parent_z },
                         { "parent_n", entry.parent_n },
                         { "child_z", entry.child_z },
                         { "child_n", entry.child_n },
                         { "energy", entry.energy },
                         { "energy_error", entry.energy_error },
                         { "intensity", entry.intensity },
                         { "intensity_error", entry.intensity_error },
                         { "init_level_energy", entry.init_level_energy },
                         { "final_level_energy", entry.final_level_energy },
                         { "alpha", entry.alpha },
                         { "parent_energy", entry.parent_energy },
                         { "half_life_sec", entry.half_life_sec },
                         { "parent_nuclide", entry.parent_nuclide },
                         { "child_nuclide", entry.child_nuclide } };
    }

    void from_json(const json& json_obj, JSONEntry& entry)
    {
        json_obj.at("is_gamma").get_to(entry.is_gamma);
        json_obj.at("parent_z").get_to(entry.parent_z);
        json_obj.at("parent_n").get_to(entry.parent_n);
        json_obj.at("child_z").get_to(entry.child_z);
        json_obj.at("child_n").get_to(entry.child_n);
        json_obj.at("energy").get_to(entry.energy);
        json_obj.at("energy_error").get_to(entry.energy_error);
        json_obj.at("intensity").get_to(entry.intensity);
        json_obj.at("intensity_error").get_to(entry.intensity_error);
        json_obj.at("init_level_energy").get_to(entry.init_level_energy);
        json_obj.at("final_level_energy").get_to(entry.final_level_energy);
        json_obj.at("alpha").get_to(entry.alpha);
        json_obj.at("parent_energy").get_to(entry.parent_energy);
        json_obj.at("half_life_sec").get_to(entry.half_life_sec);
        json_obj.at("parent_nuclide").get_to(entry.parent_nuclide);
        json_obj.at("child_nuclide").get_to(entry.child_nuclide);
    }

    void JSONReader::reset_buffer()
    {
        init_energy_level_buffer_ = EnergyLevel{};
        final_energy_level_buffer_ = EnergyLevel{};
        decay_buffer_ = Decay{};
    }

    auto JSONReader::check_json_entry(const JSONEntry& entry) -> bool
    {
        auto result = true;
        if (entry.is_gamma)
        {
            result = (entry.init_level_energy >= 0.) and (entry.final_level_energy >= 0.);
        }
        else
        {
            result = (entry.parent_energy >= 0.) and (entry.final_level_energy >= 0.);
        }
        return result;
    }

    void JSONReader::add_json_entry(const JSONEntry& entry, DecayScheme& scheme)
    {
        reset_buffer();
        decay_buffer_.is_gamma = entry.is_gamma;
        if (decay_buffer_.is_gamma)
        {
            init_energy_level_buffer_.atomic_number = entry.child_z;
            init_energy_level_buffer_.mass = entry.child_n;
            init_energy_level_buffer_.energy = entry.init_level_energy;
            auto init_level_key = scheme.add_energy_level(init_energy_level_buffer_);

            final_energy_level_buffer_.atomic_number = entry.child_z;
            final_energy_level_buffer_.mass = entry.child_n;
            final_energy_level_buffer_.energy = entry.final_level_energy;
            auto final_level_key = scheme.add_energy_level(final_energy_level_buffer_);
            decay_buffer_.init_level = init_level_key;
            decay_buffer_.final_level = final_level_key;
        }
        else
        {
            init_energy_level_buffer_.atomic_number = entry.parent_z;
            init_energy_level_buffer_.mass = entry.parent_n;
            init_energy_level_buffer_.energy = entry.parent_energy;
            auto init_level_key = scheme.add_energy_level(init_energy_level_buffer_);

            final_energy_level_buffer_.atomic_number = entry.child_z;
            final_energy_level_buffer_.mass = entry.child_n;
            final_energy_level_buffer_.energy = entry.final_level_energy;
            auto final_level_key = scheme.add_energy_level(final_energy_level_buffer_);

            decay_buffer_.init_level = init_level_key;
            decay_buffer_.final_level = final_level_key;
        }

        decay_buffer_.intensity = entry.intensity;
        decay_buffer_.energy = entry.energy;
        decay_buffer_.alpha = (entry.alpha > 0.F) ? entry.alpha : 0.F;
        scheme.add_decay(decay_buffer_);
    }

    void JSONReader::parse(DecayScheme& decay_scheme)
    {
        if (not fs::exists(filename_))
        {
            throw std::runtime_error(fmt::format("file {:?} doesn't exist!", filename_));
        }
        auto json_array = parse_json_file(filename_);

        auto entry_struct = JSONEntry{};
        for (const auto& entry : json_array.items())
        {
            entry.value().get_to(entry_struct);
            if (not check_json_entry(entry_struct))
            {
                continue;
            }
            add_json_entry(entry_struct, decay_scheme);
        }
    }
} // namespace G4Horus::Cascade
