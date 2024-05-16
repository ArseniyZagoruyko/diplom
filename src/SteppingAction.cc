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
    outputFile.open("output.txt", ios::app); 
    if (!outputFile.is_open()) { 
        G4cerr << "Error: Could not open output file!" << G4endl; 
    } 

} 
 
void SteppingAction::UserSteppingAction(const G4Step* step) 
{ 

    G4Track* track = step->GetTrack();
    G4String particleName = step->GetTrack()->GetDynamicParticle()->GetDefinition()->GetParticleName(); 
    // G4double energy = step->GetTotalEnergyDeposit()/keV; // почему GetTotalEnergyDeposit?
    G4double kinEnergy = track->GetKineticEnergy()/keV; 
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID(); 

    G4LogicalVolume* currentVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();




    // // Проверка порога Черенкова: 0.083 MeV - 0.092 MeV

    // G4String procName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    // cout << procName << endl;

    // if (procName == "Cerenkov")
    // {
    //     cout << "get it" << endl;
    // }




    // Проверка пиков от свинца - имеются как и в статье (данные в output.txt) или просто построение спектра электронов в свинцовом стекле

    G4StepPoint* prePoint = step->GetPreStepPoint();
    G4TouchableHandle touch1 = prePoint->GetTouchableHandle();
    G4VPhysicalVolume* volume = touch1->GetVolume();
    G4String vname = volume->GetName();


    if (vname.contains("Radiator") and particleName == "e-")
    {
        outputFile << kinEnergy << endl;
        track->SetTrackStatus(fStopAndKill);
    }



    // // Проверка спектра рассеяных гамма в водном фантоме

    // G4StepPoint* prePoint = step->GetPreStepPoint();
    // G4TouchableHandle touch1 = prePoint->GetTouchableHandle();
    // G4VPhysicalVolume* volume = touch1->GetVolume();
    // G4String vname = volume->GetName();


    // if (vname.contains("scat_phys"))
    // {
    //     if (kinEnergy == 600 and particleName == "gamma") // эта затычка не используется если не 511 (уже вовсе не используется)
    //     {
    //         // cout << "here" << endl;
    //         track->SetTrackStatus(fStopAndKill);
    //     }
    //     else
    //     {
    //         if (particleName == "e-")
    //         {
    //             outputFile << kinEnergy << endl;
    //             track->SetTrackStatus(fStopAndKill);
    //         }

    //     }
    // }



    // if (particleName == "e-" && energy !=0) 
    // {
    //     outputFile << "EventID: " << eventID << ", DetectorID: " << ", Energy: " << energy/keV << " keV " << endl;
    // }


} 
 
SteppingAction::~SteppingAction() 
{ 
    outputFile.close(); 
}