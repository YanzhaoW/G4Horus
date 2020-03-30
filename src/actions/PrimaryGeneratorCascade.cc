#include "PrimaryGeneratorCascade.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleTable.hh"
#include "G4RandomDirection.hh"
#include "G4PrimaryVertex.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Event.hh"

#include <string>
#include <algorithm>

G4double RandGen(PrimaryGeneratorCascade::LevelScheme &levels){
	G4double sum_weight = 0.0;
	for(auto const& x: levels){
		sum_weight += x.intensity;
	}
	G4double rnd = G4UniformRand()*sum_weight;
	for(unsigned int i=0; i<levels.size();i++){
		if(rnd < levels[i].intensity) return levels[i].energy;
		rnd -= levels[i].intensity;
	}
    return 0.0;
}

PrimaryGeneratorCascade::PrimaryGeneratorCascade(const LevelScheme& level_list, const double& energy)
:fLevelScheme(level_list),fGamma(G4ParticleTable::GetParticleTable()->FindParticle("gamma")), fEnergy(energy)
{
}

void PrimaryGeneratorCascade::GeneratePrimaryVertex(G4Event *event){
    ParticleGun(event, fEnergy);
    Decay_UpDownward(event, fEnergy, "UP");
    Decay_UpDownward(event, fEnergy, "DOWN");
    
}

void PrimaryGeneratorCascade::Decay_UpDownward(G4Event *event, G4double E, std::string mode){
	LevelScheme::iterator it;

	it = std::find_if(fLevelScheme.begin(),fLevelScheme.end(), [&](const Decay &x){return x.energy == E;});
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
			G4double EE = RandGen(levels);
			ParticleGun(event, EE);
			
			Decay_UpDownward(event, EE, mode);
		}
	}
	else{
		G4cout << "ERROR: energy not found!" << G4endl;
	}
}

void PrimaryGeneratorCascade::ParticleGun(G4Event* event, G4double energy){
    auto gamma = new G4PrimaryParticle(fGamma);
    gamma->SetMomentumDirection(G4RandomDirection());
    gamma->SetKineticEnergy(energy);
    auto gammaVertex = new G4PrimaryVertex();
    gammaVertex->SetPrimary(gamma);
    event->AddPrimaryVertex(gammaVertex);
}