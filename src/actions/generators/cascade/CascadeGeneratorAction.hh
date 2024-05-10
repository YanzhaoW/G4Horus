#pragma once

#include "DecayScheme.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include <G4Gamma.hh>
#include <G4ParticleGun.hh>

class G4Event;

namespace G4Horus::Cascade
{
    class DecayHandler
    {
      public:
        DecayHandler() = default;
        explicit DecayHandler(std::unique_ptr<DecayScheme> decay_scheme)
            : decay_scheme_{ std::move(decay_scheme) }
        {
        }

        // setters:
        void set_decay_scheme(std::unique_ptr<DecayScheme> decay_scheme)
        {
            decay_scheme_ = std::move(decay_scheme);
            reference_decay_ = nullptr;
        }

        void set_enable_sequence(bool is_enabled = true) { is_sequence_enabled_ = is_enabled; }

        void set_time_min(double time) { time_range_.first = time; }
        void set_time_max(double time) { time_range_.second = time; }

        void set_decay(double energy)
        {
            if (decay_scheme_ == nullptr)
            {
                throw std::logic_error("Cannot set a decay from a empty decay scheme!");
            }
            auto decay = decay_scheme_->get_decay(energy);
            if (decay.has_value())
            {
                reference_decay_ = decay.value();
            }
            else
            {
                throw std::runtime_error(decay.error());
            }
        }

        // getters:
        [[nodiscard]] auto get_reference_decay() const -> const Decay* { return reference_decay_; }
        [[nodiscard]] auto get_decay_scheme() const -> const DecayScheme& { return *decay_scheme_; }
        [[nodiscard]] auto has_sequence_enabled() const -> bool { return is_sequence_enabled_; }
        [[nodiscard]] auto get_time_range() const -> const std::pair<double, double>& { return time_range_; }

      private:
        bool is_sequence_enabled_ = true;
        std::pair<double, double> time_range_;
        std::unique_ptr<DecayScheme> decay_scheme_;
        const Decay* reference_decay_ = nullptr;
    };

    class GeneratorAction : public G4VUserPrimaryGeneratorAction
    {
      public:
        explicit GeneratorAction(const DecayHandler* decay_handler)
            : decay_handler_{ decay_handler }
        {
        }

      private:
        const DecayHandler* decay_handler_ = nullptr;
        double decay_time_ = 0.;
        G4ParticleGun particle_gun_{ G4Gamma::Definition(), 1 };

        // private virtual methods:
        void GeneratePrimaries(G4Event* event) override;

        // private non-virtual methods
        static void init_particle_gun(G4ParticleGun& particle_gun);
        static auto generate_random_angle_4pi() -> G4ThreeVector;
        static auto check_internal_conversion(const Decay& decay) -> bool;
        static auto generate_gamma_energy(const Decay& decay) -> std::optional<double>;
        void event_register_gamma(G4Event* event, double gamma_energy);

        template <bool is_forward>
        void generate_gammas(G4Event* event, const DecayScheme& scheme);

        void generate_gamma(G4Event* event, const Decay& decay);
    };
} // namespace G4Horus::Cascade
