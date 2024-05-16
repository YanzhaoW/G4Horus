#pragma once
#include "G4SystemOfUnits.hh"

namespace G4Horus
{
    struct RunActionSetting
    {
        bool is_tuple_enabled = false;
        bool is_hist_enabled = false;
        bool is_soco_enabled = false;
        int bin_num = 1;
        double bin_min = 0. * MeV;
        double bin_max = 1. * MeV;
    };
} // namespace G4Horus
