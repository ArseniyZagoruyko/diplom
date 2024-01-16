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

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    distance1 = 121 * mm;
    distance2 = 107 * mm;

    ConstructQuartzRadiator("QuartzRadiator1");
    ConstructQuartzRadiator("QuartzRadiator2");

    // поворот для тонких объемов
    G4RotationMatrix* rotThin = new G4RotationMatrix();
    rotThin->rotateX(90.0 * deg);

    // размеры тонких объемов
    G4double RadiusVolume = 10.0 * cm;
    G4double HightVolume = 0.1 * cm;

    // тонкий объем 1
    G4Tubs* thinVolumeSolid1 = new G4Tubs("ThinVolume1", 0, RadiusVolume, HightVolume / 2, 0, 360 * deg);
    thinVolumeLogical1 = new G4LogicalVolume(thinVolumeSolid1, world_mat, "ThinVolume1_log");

    G4VisAttributes* visAttributes1 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    thinVolumeLogical1->SetVisAttributes(visAttributes1);

    new G4PVPlacement(rotThin, G4ThreeVector(0, distance1 + 2.5 * cm, 0), thinVolumeLogical1, "ThinVolume1_phys", log_world, false, 0);

    // тонкий объем 2
    G4Tubs* thinVolumeSolid2 = new G4Tubs("ThinVolume2", 0, RadiusVolume, HightVolume / 2, 0, 360 * deg);
    thinVolumeLogical2 = new G4LogicalVolume(thinVolumeSolid2, world_mat, "ThinVolume2_log");

    G4VisAttributes* visAttributes2 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    thinVolumeLogical2->SetVisAttributes(visAttributes2);

    new G4PVPlacement(rotThin, G4ThreeVector(0, -distance2 - 2.5 * cm, 0), thinVolumeLogical2, "ThinVolume2_phys", log_world, false, 0);

    log_world->SetVisAttributes(G4VisAttributes::GetInvisible());

    return phys_world;
}

void DetectorConstruction::ConstructSDandField()
{
    G4SDManager* SDman = G4SDManager::GetSDMpointer();

    //чувствительные детекторы
    sensitiveDetector1 = new SensitiveDetector("SensitiveDetector1", 1);
    SDman->AddNewDetector(sensitiveDetector1);
    thinVolumeLogical1->SetSensitiveDetector(sensitiveDetector1);

    sensitiveDetector2 = new SensitiveDetector("SensitiveDetector2", 2);
    SDman->AddNewDetector(sensitiveDetector2);
    thinVolumeLogical2->SetSensitiveDetector(sensitiveDetector2);

}

void DetectorConstruction::ConstructQuartzRadiator(const G4String &name)
{

    // материал
    G4Material* quartz = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

    // размеры
    G4double radiatorHeight = 5.0 * cm;
    G4double radiatorLength = 5.0 * cm;
    G4double RadiusRadiator = 10.0 * cm;

    // радиатор
    G4Tubs* radiatorSolid = new G4Tubs(name, 0, RadiusRadiator, radiatorHeight / 2, 0, 360 * deg);

    G4LogicalVolume* radiatorLogical = new G4LogicalVolume(radiatorSolid, quartz, name + "_log");

    G4VisAttributes* radiatorVisAttributes = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8));
    radiatorVisAttributes->SetVisibility(true);
    radiatorLogical->SetVisAttributes(radiatorVisAttributes);

    // поворот радиатора
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateX(90.0 * deg);

    // расположение 1 и 2 радиатора
    if (name == "QuartzRadiator1") {
        new G4PVPlacement(rot, G4ThreeVector(0, distance1, 0), radiatorLogical, name, log_world, false, 0);
    } else if (name == "QuartzRadiator2") {
        new G4PVPlacement(rot, G4ThreeVector(0, -distance2, 0), radiatorLogical, name, log_world, false, 0);
    }
}