#pragma once
#include "G4SystemOfUnits.hh"

namespace G4Horus
{
    struct HistogramRunActionSetting
    {
        int bin_num = 1;
        double bin_min = 0. * MeV;
        double bin_max = 1. * MeV;
    };
} // namespace G4Horus
