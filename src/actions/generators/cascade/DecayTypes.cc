#include "DecayTypes.hh"
#include "DecayScheme.hh"

namespace G4Horus::Cascade
{
    namespace
    {
        template <bool is_forward>
        inline auto get_next_level(const Decay* current_decay)
        {
            if constexpr (is_forward)
            {
                return current_decay->final_level;
            }
            else
            {
                return current_decay->init_level;
            }
        }

        template <bool is_forward>
        inline auto get_next_decay(LevelKey current_level, const DecayScheme* decay_scheme) -> const Decay*
        {
            if (not current_level.is_not_null())
            {
                throw std::runtime_error("current level is empty!");
            }

            const auto& engine = decay_scheme->get_random_engine();
            const auto& decays = decay_scheme->get_decays(*current_level);
            if constexpr (is_forward)
            {
                return engine.random_select_decay(decays.forward_decays);
            }
            else
            {
                return engine.random_select_decay(decays.backward_decays);
            }
        }

        template <bool is_forward>
        auto propogate_to_next_decay(const Decay* current_decay, const DecayScheme* decay_scheme) -> const Decay*
        {
            auto next_level = get_next_level<is_forward>(current_decay);
            return get_next_decay<is_forward>(next_level, decay_scheme);
        }
    } // namespace

    template <bool is_forward>
    auto details::Iterator<is_forward>::operator==(const Sentinel& /*unused*/) const -> bool
    {
        return decay_ == nullptr;
    }

    template <bool is_forward>
    auto details::Iterator<is_forward>::operator==(const Iterator& other) const -> bool
    {
        return *decay_ == *other;
    }

    template <bool is_forward>
    auto details::Iterator<is_forward>::operator++() -> Iterator&
    {
        decay_ = propogate_to_next_decay<is_forward>(decay_, scheme_);
        return *this;
    }

    // post-increment
    template <bool is_forward>
    auto details::Iterator<is_forward>::operator++(int) -> Iterator
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    // pre-decrement
    template <bool is_forward>
    auto details::Iterator<is_forward>::operator--() -> Iterator&
    {
        decay_ = propogate_to_next_decay<is_forward>(decay_, scheme_);
        return *this;
    }

    // post-decrement
    template <bool is_forward>
    auto details::Iterator<is_forward>::operator--(int) -> Iterator
    {
        auto temp = *this;
        --(*this);
        return temp;
    }

    template class details::Iterator<true>;
    template class details::Iterator<false>;

} // namespace G4Horus::Cascade
