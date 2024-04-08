#include "DecayEngine.hh"
#include "DecayTypes.hh"
#include <numeric>

namespace G4Horus::Cascade
{
    auto DecayEngine::random_select_decay(const std::vector<Decay*>& decays) const -> const Decay*
    {
        if (decays.empty())
        {
            return nullptr;
        }

        auto intensity_view = rng::views::transform(decays, [](auto* decay) { return decay->intensity; });
        auto sum = std::accumulate(intensity_view.begin(), intensity_view.end(), 0.F);
        const auto item_intensity_view = rng::zip_view(decays, intensity_view);
        return random_select(item_intensity_view, sum);
    }

} // namespace G4Horus::Cascade
