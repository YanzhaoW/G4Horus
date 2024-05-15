#pragma once

#include "G4UserRunAction.hh"
#include "Shared.hh"
#include <G4AnalysisManager.hh>
#include <fmt/format.h>
#include <unordered_map>

class G4Run;

namespace G4Horus
{
    enum class NTupleColumn
    {
        time,
        energy,
        position_x,
        position_y,
        position_z,
        size,
        detector
    };

    class RunActionNtuple : public G4UserRunAction
    {
      public:
        explicit RunActionNtuple(const G4Horus::HistogramRunActionSetting* setting);

        void BeginOfRunAction(const G4Run* /*aRun*/) override;
        void EndOfRunAction(const G4Run* /*aRun*/) override;
        [[nodiscard]] auto GetColumnIdMap() const -> const auto& { return column_id_map_; }

      private:
        const HistogramRunActionSetting* setting_ = nullptr;
        G4GenericAnalysisManager* analysis_manager_ = G4AnalysisManager::Instance(); // no-owning
        std::unordered_map<NTupleColumn, int> column_id_map_{
            { NTupleColumn::time, 0 },       { NTupleColumn::energy, 0 },     { NTupleColumn::position_x, 0 },
            { NTupleColumn::position_y, 0 }, { NTupleColumn::position_z, 0 }, { NTupleColumn::detector, 0 },
            { NTupleColumn::size, 0 }
        };
    };
} // namespace G4Horus

template <>
class fmt::formatter<G4Horus::NTupleColumn>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(G4Horus::NTupleColumn column, FmtContent& ctn) const
    {
        switch (column)
        {
            case G4Horus::NTupleColumn::time:
                return fmt::format_to(ctn.out(), "time");
            case G4Horus::NTupleColumn::energy:
                return fmt::format_to(ctn.out(), "energy");
            case G4Horus::NTupleColumn::position_x:
                return fmt::format_to(ctn.out(), "x");
            case G4Horus::NTupleColumn::position_y:
                return fmt::format_to(ctn.out(), "y");
            case G4Horus::NTupleColumn::position_z:
                return fmt::format_to(ctn.out(), "z");
            case G4Horus::NTupleColumn::size:
                return fmt::format_to(ctn.out(), "size");
            case G4Horus::NTupleColumn::detector:
                return fmt::format_to(ctn.out(), "detector");
            default:
                return fmt::format_to(ctn.out(), "Unknown");
        }
    }
};
