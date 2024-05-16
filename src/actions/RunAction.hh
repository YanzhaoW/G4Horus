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
        time,       // seconds
        energy,     // keV
        position_x, // cm
        position_y, // cm
        position_z, // cm
        size,
        detector
    };

    class RunAction : public G4UserRunAction
    {
      public:
        explicit RunAction(const G4Horus::RunActionSetting* setting, int verbose_level);

        void BeginOfRunAction(const G4Run* /*aRun*/) override;
        void EndOfRunAction(const G4Run* /*aRun*/) override;
        [[nodiscard]] auto GetColumnIdMap() const -> const auto& { return column_id_map_; }
        [[nodiscard]] auto GetHistgramIdMap() const -> const auto& { return histogram_id_map_; }
        [[nodiscard]] auto GetSettings() const -> const auto* { return setting_; }

      private:
        int verbose_level_ = 0;
        const RunActionSetting* setting_ = nullptr;
        G4GenericAnalysisManager* analysis_manager_ = G4AnalysisManager::Instance(); // no-owning
        std::unordered_map<NTupleColumn, int> column_id_map_{
            { NTupleColumn::time, 0 },       { NTupleColumn::energy, 0 },     { NTupleColumn::position_x, 0 },
            { NTupleColumn::position_y, 0 }, { NTupleColumn::position_z, 0 }, { NTupleColumn::detector, 0 },
            { NTupleColumn::size, 0 }
        };
        std::unordered_map<std::string_view, int> histogram_id_map_;

        // non-virtual functions:
        void setup_ntuple(G4AnalysisManager* analysis_manager);
        void setup_histogram(G4AnalysisManager* analysis_manager);
        void setup_soco(G4AnalysisManager* analysis_manager);
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
