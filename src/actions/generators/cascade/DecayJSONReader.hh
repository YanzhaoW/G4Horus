#pragma once

#include "DecayScheme.hh"
#include <string>

namespace G4Horus::Cascade
{
    struct JSONEntry
    {
        bool is_gamma = true;
        int parent_z = 0;
        int parent_n = 0;
        int child_z = 0.F;
        int child_n = 0.F;
        double energy = 0.F;
        double energy_error = 0.F;
        float intensity = 0.F;
        float intensity_error = 0.F;
        double init_level_energy = 0.F;
        double final_level_energy = 0.F;
        float alpha = 0.F;
        double parent_energy = 0.F;
        double half_life_sec = 0.F;
        std::string parent_nuclide;
        std::string child_nuclide;
    };

    class JSONReader
    {
      public:
        explicit JSONReader(std::string_view filename)
            : filename_{ filename }
        {
        }
        void parse(DecayScheme& decay_scheme);

      private:
        std::string filename_;
        EnergyLevel init_energy_level_buffer_;
        EnergyLevel final_energy_level_buffer_;
        Decay decay_buffer_;

        auto check_json_entry(const JSONEntry& entry) -> bool;
        void add_json_entry(const JSONEntry& entry, DecayScheme& scheme);
        void reset_buffer();
    };
}; // namespace G4Horus::Cascade
