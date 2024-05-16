#pragma once
#include <array>
#include <string_view>

namespace G4Horus
{
    constexpr auto CLOVER_HIT_COLLECTION_NAME = std::string_view{ "CloverHits" };
    constexpr auto CLOVER_DETECTOR_NAMES =
        std::array<std::string_view, 8>{ "A0", "A1", "A2", "A3", "B0", "B1", "B2", "B3" };

    enum class CloverDet
    {
        A0,
        A1,
        A2,
        A3,
        B0,
        B1,
        B2,
        B3
    };

    constexpr auto DEFAULT_PHYSICS_LIST = std::string_view{ "QGSP_BERT_EMV" };

} // namespace G4Horus
