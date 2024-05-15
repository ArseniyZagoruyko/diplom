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
  G4int seed = clock();


  G4double DirPhi = 2.0 * M_PI * G4UniformRand();
  G4double DirTheta = acos(1.0 - 2.0 * G4UniformRand());


  G4double Dirx = sin(DirTheta) * cos(DirPhi);
  G4double Diry = sin(DirTheta) * sin(DirPhi);
  G4double Dirz = cos(DirTheta);

  //радиус объекта исследования 
  G4double R = 2*cm* G4UniformRand();
  
  //радиус эквивалентный пробегу позитрона в среде 
  G4double r = 1.4*mm;

  // 2ой генератор (для позиции)
  G4double PosPhi = 2.0 * M_PI * G4UniformRand();
  G4double PosTheta = acos(1.0 - 2.0 * G4UniformRand());  

  G4double Posx = R*sin(PosTheta)*cos(PosPhi);
  G4double Posy = R*sin(PosTheta)*sin(PosPhi);
  G4double Posz = R*cos(PosTheta);

  // 3ий генератор направления (для учета пробега позитрона)
  G4double ShiftPhi = 2.0 * M_PI * G4UniformRand();
  G4double ShiftTheta = acos(1.0 - 2.0 * G4UniformRand());


  G4double Shiftx = sin(ShiftTheta) * cos(ShiftPhi);
  G4double Shifty = sin(ShiftTheta) * sin(ShiftPhi);
  G4double Shiftz = cos(ShiftTheta);

  //сдвиг

  Posx += r * Shiftx;
  Posy += r * Shifty;
  Posz += r * Shiftz;

  


  gun->SetParticleMomentumDirection(G4ThreeVector(0.0,1.0,0.0));
  // gun->SetParticlePosition(G4ThreeVector(1*cm, 0 , 1*cm));

  gun->SetParticlePosition(G4ThreeVector(Posx, Posy, Posz));
  // gun->SetParticleMomentumDirection(G4ThreeVector(Dirx,Diry,Dirz));

  gun->GeneratePrimaryVertex(event);


  // направление для второй частицы
  // gun->SetParticleMomentumDirection(G4ThreeVector(-Dirx,-Diry,-Dirz));
  // gun->SetParticleMomentumDirection(G4ThreeVector(0.0,-1.0,0.0));
  // gun->GeneratePrimaryVertex(event);
}