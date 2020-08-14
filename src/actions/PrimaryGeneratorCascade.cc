#include "PrimaryGeneratorCascade.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleTable.hh"
#include "G4RandomDirection.hh"
#include "G4PrimaryVertex.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Event.hh"
#include "G4PhysicalConstants.hh"
#include "CascadeMessenger.hh"

#include <string>
#include <algorithm>

PrimaryGeneratorCascade::LevelScheme::iterator RandGen(PrimaryGeneratorCascade::LevelScheme &levels){
	G4double sum_weight = 0.0;
	for(auto const& x: levels){
		sum_weight += x.intensity;
	}
	G4double rnd = G4UniformRand()*sum_weight;

	for(PrimaryGeneratorCascade::LevelScheme::iterator itp = levels.begin(); itp!=levels.end();itp++){
		if(rnd < itp->intensity) return itp;
		rnd -= itp->intensity;
	}
}

PrimaryGeneratorCascade::PrimaryGeneratorCascade()
:fGamma(G4ParticleTable::GetParticleTable()->FindParticle("gamma"))
{
	fMessenger = new CascadeMessenger(this);
}

PrimaryGeneratorCascade::~PrimaryGeneratorCascade(){
    delete fMessenger;
}

void PrimaryGeneratorCascade::GeneratePrimaryVertex(G4Event *event){
    ParticleGun(event, fEnergy);
	
	if(fMode){
		LevelScheme::iterator it0 = GetItr(fEnergy);
		Decay_UpDownward(event, it0, "UP");

		it0 = GetItr(fEnergy);
    	Decay_UpDownward(event, it0, "DOWN");
	}
}

G4bool PrimaryGeneratorCascade::is_gamma_e(LevelScheme::iterator it){
	if(it->is_gamma){
		G4double rnd = G4UniformRand()*(1+it->alpha);
        if(rnd<=1.0){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

PrimaryGeneratorCascade::LevelScheme::iterator PrimaryGeneratorCascade::GetItr(G4double E){
	LevelScheme::iterator itrp = find_if(fLevelScheme.begin(),fLevelScheme.end(), [&](const Decay &x){return x.energy == E;});
	for(LevelScheme::iterator i=itrp; i < fLevelScheme.end(); i++){
		i = find_if(i,fLevelScheme.end(), [&](const Decay &x){return x.energy == E;});
		if(i->intensity > itrp->intensity) itrp = i;
	}
	if(itrp == fLevelScheme.end()) G4cout << "ERROR: energy not found at " << E << G4endl;
	return itrp;
}

void PrimaryGeneratorCascade::Decay_UpDownward(G4Event *event, LevelScheme::iterator it, std::string mode){
	// LevelScheme::iterator it;
	// it = std::find_if(fLevelScheme.begin(),fLevelScheme.end(), [&](const Decay &x){return x.energy == E;});
	if(it != fLevelScheme.end()){
		
		LevelScheme levels(0);
		if(mode == "UP"){
			std::copy_if(fLevelScheme.begin(),fLevelScheme.end(),std::back_inserter(levels),
                [&](const Decay &x){return x.final_level==it->ini_level;});
				
		}else if(mode == "DOWN"){
			std::copy_if(fLevelScheme.begin(),fLevelScheme.end(),std::back_inserter(levels),
                [&](const Decay &x){return x.ini_level==it->final_level;});
		}else{
			G4cout << "ERROR: choose correct decay direction!" << G4endl;
		}
		
		if (levels.size() != 0)
		{
			LevelScheme::iterator itrEE = RandGen(levels);
			if(is_gamma_e(itrEE)) ParticleGun(event, itrEE->energy);
			Decay_UpDownward(event, itrEE, mode);
		}
		
		
	}
	else{
		G4cout << "ERROR: level does not exit! \t " <<"mode: " << mode<< G4endl;
	}
	
}

void PrimaryGeneratorCascade::ParticleGun(G4Event* event, G4double energy){
    auto gamma = new G4PrimaryParticle(fGamma);

	G4double phi = twopi * G4UniformRand();
    G4double cos_theta = 2. * G4UniformRand() - 1.;
    G4double sin_theta = std::sqrt(1. - cos_theta * cos_theta);
    G4double ux = sin_theta * std::cos(phi);
    G4double uy = sin_theta * std::sin(phi);
    G4double uz = cos_theta;
	gamma->SetMomentumDirection(G4ThreeVector(ux, uy, uz));

    // gamma->SetMomentumDirection(G4RandomDirection());

    gamma->SetKineticEnergy(energy);
    auto gammaVertex = new G4PrimaryVertex();
    gammaVertex->SetPrimary(gamma);
    event->AddPrimaryVertex(gammaVertex);
}