#include "SensitiveDetector.hh"
#include <G4Step.hh>
#include <fstream>
#include <iostream>
#include <G4EventManager.hh>
#include <G4SDManager.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4SystemOfUnits.hh>
#include <G4RunManager.hh>

using namespace std;

SensitiveDetector::SensitiveDetector(G4String name, G4int id) : G4VSensitiveDetector(name), detectorID(id) 
{
    hitsFile.open("hits.txt", ios_base::out);
    //  файл для записи

}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) 
{
    // информация о частице и её треке
    G4Track* track = step->GetTrack();
    G4ParticleDefinition* particle = track->GetDefinition();

   
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();


    G4double gammaEnergy = track->GetKineticEnergy();


    G4double globalTime = track->GetGlobalTime();

    // запись в файл
    hitsFile << "DetectorID: " << detectorID << "\t";

    hitsFile << "Time: " << globalTime / second << " s\t Particle: " << particle->GetParticleName()<< "\t EnergyGamma: " << gammaEnergy / keV <<"keV\t Position: " << position / mm << " mm\n"   << endl;

    track->SetTrackStatus(fStopAndKill);

    return true;
}


//геттер
G4int SensitiveDetector::GetDetectorID() const
{
    return detectorID;
}


SensitiveDetector::~SensitiveDetector() {
 
  hitsFile.close();
}