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
#include <iomanip>
#include <sstream>

using namespace std;

SensitiveDetector::SensitiveDetector(G4String name, G4int id) : G4VSensitiveDetector(name), detectorID(id) 
{

}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) 
{
    // информация о частице
    G4Track* track = step->GetTrack();
   
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
    G4double energy = track->GetKineticEnergy();
    G4double globalTime = track->GetGlobalTime();

    // потоковый буфер
    ostringstream dataStream;
    dataStream << "DetectorID: " << left << setw(5) << setfill(' ') << detectorID << "    ";
    dataStream << "Time: " << left << fixed << "    " << setprecision(15) << globalTime / second << " s    ";
    dataStream << "Energy: " << left << fixed << setw(15) << setprecision(4) << energy / keV << " keV    ";
    dataStream << "Position: " << left << fixed << position / mm << " mm\n";

    // запись из п б в файл
    ofstream hitsFile("hits.txt", ios_base::app);
    if (hitsFile.is_open()) {
        hitsFile << dataStream.str();
        hitsFile.close();
    } else {
        cerr << "Error opening file!" << endl;
    }

    track->SetTrackStatus(fStopAndKill);

    return true;
}

// геттер
G4int SensitiveDetector::GetDetectorID() const
{
    return detectorID;
}

SensitiveDetector::~SensitiveDetector() 
{

}