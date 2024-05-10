#include "CascadeGeneratorAction.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include <Randomize.hh>

using CLHEP::twopi;

namespace G4Horus::Cascade
{

    void GeneratorAction::init_particle_gun(G4ParticleGun& particle_gun)
    {
        particle_gun.SetParticlePosition(G4ThreeVector{ 0. * cm, 0. * cm, 0. * cm });
        particle_gun.SetParticleEnergy(0. * keV);
    }

    void GeneratorAction::generate_gamma(G4Event* event, const Decay& decay)
    {

        auto gamma_energy = generate_gamma_energy(decay);
        if (gamma_energy.has_value())
        {
            event_register_gamma(event, gamma_energy.value());
        }
    }

    template <bool is_forward>
    void GeneratorAction::generate_gammas(G4Event* event, const DecayScheme& scheme)
    {
        const auto* reference_decay = decay_handler_->get_reference_decay();
        const auto& sequence = scheme.construct_decay_sequence<is_forward>(*reference_decay);
        for (const auto& decay : sequence | std::ranges::views::drop(1))
        {
            generate_gamma(event, decay);
        }
    }

    void GeneratorAction::GeneratePrimaries(G4Event* event)
    {
        // generate decay_time with uniform distribution
        const auto& time_window = decay_handler_->get_time_range();
        decay_time_ = (time_window.second - time_window.first) * G4UniformRand() + time_window.first;

        const auto& decay_scheme = decay_handler_->get_decay_scheme();
        const auto* reference_decay = decay_handler_->get_reference_decay();
        if (reference_decay == nullptr)
        {
            throw std::runtime_error("reference decay doesn't exist!");
        }

        generate_gamma(event, *reference_decay);
        if (decay_handler_->has_sequence_enabled())
        {
            generate_gammas<true>(event, decay_scheme);
            generate_gammas<false>(event, decay_scheme);
        }
    }

    auto GeneratorAction::generate_random_angle_4pi() -> G4ThreeVector
    {
        G4double phi = twopi * G4UniformRand();
        // PDF(theta) ~ sin(theta) leads to PDF(cos(theta)) ~ 1
        G4double cos_theta = 2 * G4UniformRand() - 1.;
        auto momentum = G4ThreeVector{};
        momentum.setRhoPhiZ(1., phi, cos_theta);
        return momentum;
    }

    void GeneratorAction::event_register_gamma(G4Event* event, double gamma_energy)
    {
        init_particle_gun(particle_gun_);
        particle_gun_.SetParticleMomentumDirection(generate_random_angle_4pi());
        particle_gun_.SetParticleEnergy(gamma_energy * keV);
        particle_gun_.GeneratePrimaryVertex(event);
        particle_gun_.SetParticleTime(decay_time_);
    }

    auto GeneratorAction::check_internal_conversion(const Decay& decay) -> bool
    {
        // internal conversion: alpha = (num of electrons) / (num of gammas)
        const auto alpha_check_value = 1. / (decay.alpha + 1.);
        return G4UniformRand() < alpha_check_value;
    }

    auto GeneratorAction::generate_gamma_energy(const Decay& decay) -> std::optional<double>
    {
        if (not decay.is_gamma)
        {
            return {};
        }

        // filter possible negative energies
        if (decay.energy <= 0.)
        {
            return {};
        }

        if (not check_internal_conversion(decay))
        {
            return {};
        }

        return decay.energy;
    }
} // namespace G4Horus::Cascade
