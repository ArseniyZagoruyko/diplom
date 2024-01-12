#include "PrimaryParticles.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "time.h"
#include "G4Geantino.hh"
#include "G4IonTable.hh"
#include "G4ChargedGeantino.hh"
#include "G4NuclideTable.hh"
#include "G4VIsotopeTable.hh"
#include "G4Ions.hh"

#include <cmath>

using namespace std;

PrimaryParticles::PrimaryParticles()
{
    gun = new G4ParticleGun(1);
    gun->SetParticleEnergy(511 * keV);

    auto particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    gun->SetParticleDefinition(particleDefinition);
}

PrimaryParticles::~PrimaryParticles()
{
    delete gun;
}

void PrimaryParticles::GeneratePrimaries(G4Event* event)
{
    ac = ac + 1;

    if (ac == 10000 * i1)
    {
        i1++;
        cout << ac << endl;
    }

    G4int seed = clock();

    // ограничения для углов theta и phi
    G4double thetaMin = (M_PI/2) + atan(5.0 / 10.7);
    G4double thetaMax = (M_PI/2) - atan(5.0 / 10.7);
    G4double phiMin = (M_PI/2) - atan(2.5 / 10.7);
    G4double phiMax = (M_PI/2) + atan(2.5 / 10.7);

    G4double phi = phiMin + (phiMax - phiMin) * G4UniformRand();
    G4double theta = thetaMin + (thetaMax - thetaMin) * G4UniformRand();

    G4double x = sin(theta) * cos(phi);
    G4double y = sin(theta) * sin(phi);
    G4double z = cos(theta);

    gun->SetParticleMomentumDirection(G4ThreeVector(x, y, z));
    gun->SetParticlePosition(G4ThreeVector(0.0, 0.0, 0.0));
    gun->GeneratePrimaryVertex(event);

    x = -sin(theta) * cos(phi);
    y = -sin(theta) * sin(phi);
    z = -cos(theta);

    gun->SetParticleMomentumDirection(G4ThreeVector(x, y, z));
    gun->GeneratePrimaryVertex(event);
}