#include <G4VUserDetectorConstruction.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4PVPlacement.hh>
#include <G4AssemblyVolume.hh>
#include <G4SubtractionSolid.hh>


class DetectorConstruction : public G4VUserDetectorConstruction
{
private:
    G4Box* world;
    G4LogicalVolume* log_world;
    G4VPhysicalVolume* phys_world;

    G4NistManager* nist;
    G4Material* world_mat;

    G4double distance1;
    G4double distance2;

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

public:
    void ConstructQuartzRadiator(const G4String &name);

    DetectorConstruction();
    ~DetectorConstruction();
};