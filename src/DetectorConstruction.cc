#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include <G4SystemOfUnits.hh>
#include <G4PVReplica.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4Tubs.hh>
#include <G4SDManager.hh>
#include <G4VisAttributes.hh>
#include <G4Cons.hh>
#include <G4Orb.hh>
#include <G4EllipticalTube.hh>
#include <G4Transform3D.hh>
#include <G4Trd.hh>
#include <G4UnionSolid.hh>
#include <G4PropagatorInField.hh>
#include <G4UniformMagField.hh>
#include <G4Mag_UsualEqRhs.hh>
#include <G4HelixHeum.hh>
#include <G4MagIntegratorDriver.hh>
#include <G4ChordFinder.hh>
#include <G4FieldManager.hh>
#include <G4QuadrupoleMagField.hh>
#include <G4RotationMatrix.hh>
#include <G4GlobalMagFieldMessenger.hh>
#include <G4MagIntegratorStepper.hh>
#include <G4ClassicalRK4.hh>
#include <globals.hh>
#include <G4SubtractionSolid.hh>
#include <G4OpticalSurface.hh>
#include <G4Sphere.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4GeometryManager.hh>
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4Torus.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4Para.hh"
#include "G4Tubs.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4RunManager.hh"
#include <vector>

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction()
{
    G4double world_size = 5 * m;

    nist = G4NistManager::Instance();


    world_mat = nist->FindOrBuildMaterial("G4_Galactic");

    world = new G4Box("world", world_size / 2, world_size / 2, world_size / 2);
    log_world = new G4LogicalVolume(world, world_mat, "world_log");
    phys_world = new G4PVPlacement(0, G4ThreeVector(), log_world, "phys", 0, false, 0);

}

DetectorConstruction::~DetectorConstruction()
{
    delete world;
    delete log_world;
    delete phys_world;
}

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    G4double distance1 = 121 * mm;
    G4double distance2 = 107 * mm;

    ConstructQuartzRadiator("QuartzRadiator1", distance1);
    ConstructQuartzRadiator("QuartzRadiator2", distance2);

    // поворот для тонких объемов
    G4RotationMatrix* rotThin = new G4RotationMatrix();
    rotThin->rotateX(90.0 * deg);
    rotThin->rotateZ(90.0 * deg);

    //размеры тонких объемов 
    G4double WidthVolume = 1.0 * mm;
    G4double HightVolume = 5.0 * cm;
    G4double LenghtVolume = 10.0 * cm;

    // тонкий объем 1
    G4Box* thinVolumeSolid1 = new G4Box("ThinVolume1", LenghtVolume / 2, HightVolume / 2, WidthVolume / 2);
    G4LogicalVolume* thinVolumeLogical1 = new G4LogicalVolume(thinVolumeSolid1, world_mat, "ThinVolume1_log");

    G4VisAttributes* visAttributes1 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    thinVolumeLogical1->SetVisAttributes(visAttributes1);

    new G4PVPlacement(rotThin, G4ThreeVector(0, distance1 +1*cm + WidthVolume/2 , 0), thinVolumeLogical1, "ThinVolume1_phys", log_world, false, 0);

    SensitiveDetector* sensitiveDetector1 = new SensitiveDetector("SensitiveDetector1", 1);
    thinVolumeLogical1->SetSensitiveDetector(sensitiveDetector1);

    // тонкий объем2
    G4Box* thinVolumeSolid2 = new G4Box("ThinVolume2", LenghtVolume / 2, HightVolume / 2, WidthVolume / 2);
    G4LogicalVolume* thinVolumeLogical2 = new G4LogicalVolume(thinVolumeSolid2, world_mat, "ThinVolume2_log");

    G4VisAttributes* visAttributes2 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    thinVolumeLogical2->SetVisAttributes(visAttributes2);

    new G4PVPlacement(rotThin, G4ThreeVector(0, -distance2-1*cm - WidthVolume/2 , 0), thinVolumeLogical2, "ThinVolume2_phys", log_world, false, 0);

    SensitiveDetector* sensitiveDetector2 = new SensitiveDetector("SensitiveDetector2", 2);
    thinVolumeLogical2->SetSensitiveDetector(sensitiveDetector2);

    log_world->SetVisAttributes(G4VisAttributes::GetInvisible());

    return phys_world;
}


void DetectorConstruction::ConstructSDandField()
{

}

void DetectorConstruction::ConstructQuartzRadiator(const G4String &name, G4double distanceFromCenter)
{
    // материал
    G4Material* quartz = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

    // размеры 
    G4double radiatorHeight = 10.0 * cm;
    G4double radiatorLength = 5.0 * cm;
    G4double radiatorWidth = 2 * cm;

    //  радиатор
    G4Box* radiatorSolid = new G4Box(name, radiatorWidth / 2, radiatorLength / 2, radiatorHeight / 2);

   
    G4LogicalVolume* radiatorLogical = new G4LogicalVolume(radiatorSolid, quartz, name + "_log");


    G4VisAttributes* radiatorVisAttributes = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8)); 
    radiatorVisAttributes->SetVisibility(true);
    radiatorLogical->SetVisAttributes(radiatorVisAttributes);

    // вектор для расположения
    G4ThreeVector position(0.0, distanceFromCenter, 0.0);

    // поворот радиатора
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateZ(90.0 * deg);


    // расположение 1 и 2 радиатора
    if (name == "QuartzRadiator1") {
        new G4PVPlacement(rot, G4ThreeVector(0, distanceFromCenter, 0), radiatorLogical, name, log_world, false, 0);
    } else if (name == "QuartzRadiator2") {
        new G4PVPlacement(rot, G4ThreeVector(0, -distanceFromCenter, 0), radiatorLogical, name, log_world, false, 0);
    }



}