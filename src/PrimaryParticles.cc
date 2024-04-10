#include "PrimaryParticles.hh"

#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "time.h"
#include "G4Geantino.hh"
#include "G4IonTable.hh"
#include "G4ChargedGeantino.hh"
#include "cmath"

#include "G4NuclideTable.hh"
#include "G4VIsotopeTable.hh"

#include "G4Ions.hh"

using namespace std;

PrimaryParticles::PrimaryParticles(){

    gun = new G4ParticleGun(1);
    gun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
    gun->SetParticlePosition(G4ThreeVector(0, 0, 0));
    gun->SetParticleEnergy(511*keV);

}

PrimaryParticles::~PrimaryParticles(){

  delete gun;
}


void PrimaryParticles::GeneratePrimaries(G4Event* event){

    G4int Z = 9, A = 18;
    G4double ionCharge   = 0;
    G4double excitEnergy = 0*MeV;

    G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
    gun->SetParticleDefinition(ion);
    gun->SetParticleCharge(ionCharge);

    gun->GeneratePrimaryVertex(event);


    G4double phi =  2 * M_PI * G4UniformRand();
    G4double costheta = -1.0 + 2*1.0 * G4UniformRand();
    G4double u = G4UniformRand();

    G4double theta =  acos(costheta);
    double r = 0.4 * pow(u, 1.0 / 3.0);

    gun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 0.0) );


    double x = r * sin(theta) * cos(phi);
    double y = r * sin(theta) * sin(phi);
    double z = r * cos(theta); 

    gun->SetParticlePosition(G4ThreeVector(x, y, z));


}