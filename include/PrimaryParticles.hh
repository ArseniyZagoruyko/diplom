#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"

class PrimaryParticles : public G4VUserPrimaryGeneratorAction{
public:

    PrimaryParticles();
    ~PrimaryParticles();
    
    G4double En = 0;

    virtual void GeneratePrimaries(G4Event* event);

private:

    G4ParticleGun* gun;
};