#include "SteppingAction.hh" 
#include "G4SteppingManager.hh"
#include "DetectorConstruction.hh" 
#include "G4Step.hh" 
#include "G4RunManager.hh" 
#include <fstream> 
#include <iostream> 
#include <G4SystemOfUnits.hh> 
#include <string> 
using namespace std; 

extern G4LogicalVolume* radiatorLogical;

SteppingAction::SteppingAction(DetectorConstruction* detectorConstruction) 
    : G4UserSteppingAction(), 
      fDetConstruction(detectorConstruction) 
{ 
    outputFile.open("output.txt", std::ios_base::app); 
    if (!outputFile.is_open()) { 
        G4cerr << "Error: Could not open output file!" << G4endl; 
    } 

} 
 
void SteppingAction::UserSteppingAction(const G4Step* step) 
{ 

    G4Track* track = step->GetTrack();
    G4String particleName = step->GetTrack()->GetDynamicParticle()->GetDefinition()->GetParticleName(); 
    G4double energy = step->GetTotalEnergyDeposit(); 
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID(); 

    G4LogicalVolume* currentVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();



    if (particleName == "e-" && energy !=0) 
    {
        outputFile << "EventID: " << eventID << ", DetectorID: " << ", Energy: " << energy/keV << " keV " << endl;
    }


} 
 
SteppingAction::~SteppingAction() 
{ 
    outputFile.close(); 
}