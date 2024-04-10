#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include <G4SystemOfUnits.hh>
#include <G4PVReplica.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include<G4Tubs.hh>
#include<G4SDManager.hh>
#include<G4VisAttributes.hh>
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
    water_mat = nist->FindOrBuildMaterial("G4_WATER");

    G4double a, z, density;
    G4int nelements;

    // Air
    G4Element* N = new G4Element("Nitrogen", "N", z=7 , a=14.01*g/mole);
    G4Element* O = new G4Element("Oxygen"  , "O", z=8 , a=16.00*g/mole);

    G4Material* air = new G4Material("Air", density=0.0000000000000000012*g/cm3, nelements=2);
    air->AddElement(N, 70.*perCent);
    air->AddElement(O, 30.*perCent);
    G4double photonEnergy_air[] = { 1.90744615*eV, 1.95409565*eV, 2.00308412*eV, 2.054592*eV,   2.10881877*eV, 2.16598554*eV,
                                 2.22633808*eV, 2.29015032*eV, 2.35772852*eV, 2.42941622*eV, 2.5056*eV,     2.58671655*eV,
                                 2.67326097*eV, 2.76579692*eV, 2.86496892*eV, 2.97151736*eV, 3.086297*eV,   3.2103*eV,
                                 3.34468465*eV, 3.49081165*eV, 3.65029036*eV, 3.8250383*eV,  4.01735866*eV, 4.23004235*eV,
                                 4.46650435*eV, 4.73096842*eV, 5.02872168*eV, 5.36647164*eV, 5.7528576*eV,  6.1992*eV    };

    const G4int nEntries_air = sizeof(photonEnergy_air)/sizeof(G4double);

    G4double refractiveIndex_air[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    G4MaterialPropertiesTable* myMPT2 = new G4MaterialPropertiesTable();
    myMPT2->AddProperty("RINDEX", photonEnergy_air, refractiveIndex_air, nEntries_air);

    air->SetMaterialPropertiesTable(myMPT2);

    world = new G4Box("world", world_size / 2, world_size / 2, world_size / 2);
    log_world = new G4LogicalVolume(world, air, "world_log");
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
    G4double a, z, density;
    G4int nelements;

    // Air
    G4Element* N = new G4Element("Nitrogen", "N", z=7 , a=14.01*g/mole);
    G4Element* O = new G4Element("Oxygen"  , "O", z=8 , a=16.00*g/mole);
    G4Material* air = new G4Material("Air", density=0.0000000000000000012*g/cm3, nelements=2);
    air->AddElement(N, 70.*perCent);
    air->AddElement(O, 30.*perCent);
    G4double photonEnergy_air[] = { 1.90744615*eV, 1.95409565*eV, 2.00308412*eV, 2.054592*eV,   2.10881877*eV, 2.16598554*eV,
                                 2.22633808*eV, 2.29015032*eV, 2.35772852*eV, 2.42941622*eV, 2.5056*eV,     2.58671655*eV,
                                 2.67326097*eV, 2.76579692*eV, 2.86496892*eV, 2.97151736*eV, 3.086297*eV,   3.2103*eV,
                                 3.34468465*eV, 3.49081165*eV, 3.65029036*eV, 3.8250383*eV,  4.01735866*eV, 4.23004235*eV,
                                 4.46650435*eV, 4.73096842*eV, 5.02872168*eV, 5.36647164*eV, 5.7528576*eV,  6.1992*eV    };

    const G4int nEntries_air = sizeof(photonEnergy_air)/sizeof(G4double);

    G4double refractiveIndex_air[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    G4MaterialPropertiesTable* myMPT2 = new G4MaterialPropertiesTable();
    myMPT2->AddProperty("RINDEX", photonEnergy_air, refractiveIndex_air, nEntries_air);

    air->SetMaterialPropertiesTable(myMPT2);
    distance1 = 13* cm;
    distance2 = 13 * cm;

    ConstructQuartzRadiator("QuartzRadiator1");
    ConstructQuartzRadiator("QuartzRadiator2");
    
    //ConstructSphere();
    
    ConstructWaterPhantom();

    // поворот для тонких объемов
    G4RotationMatrix* rotThin = new G4RotationMatrix();
    rotThin->rotateY(90.0 * deg);

    // размеры тонких объемов
    G4double HightVolume = 10.0 * cm;
    G4double ThinVolume = 0.1 * cm;
    G4double LenghtVolume = 5.0 * cm;

    // тонкий объем 1
    G4Box* thinVolumeSolid1 = new G4Box("ThinVolume1", HightVolume/2, ThinVolume / 2, LenghtVolume/2);
    thinVolumeLogical1 = new G4LogicalVolume(thinVolumeSolid1, air, "ThinVolume1_log");

    G4VisAttributes* visAttributes1 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    thinVolumeLogical1->SetVisAttributes(visAttributes1);

    new G4PVPlacement(rotThin, G4ThreeVector(0, distance1 + radiatorThin/2 + ThinVolume/2, 0), thinVolumeLogical1, "ThinVolume1_phys", log_world, false, 0);

    // тонкий объем 2
    G4Box* thinVolumeSolid2 = new G4Box("ThinVolume2", HightVolume/2, ThinVolume / 2, LenghtVolume/2);
    thinVolumeLogical2 = new G4LogicalVolume(thinVolumeSolid2, air, "ThinVolume2_log");

    G4VisAttributes* visAttributes2 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    thinVolumeLogical2->SetVisAttributes(visAttributes2);

    new G4PVPlacement(rotThin, G4ThreeVector(0, -distance2 - radiatorThin/2 -ThinVolume/2, 0), thinVolumeLogical2, "ThinVolume2_phys", log_world, false, 0);



    log_world->SetVisAttributes(G4VisAttributes::GetInvisible());

    return phys_world;
}

void DetectorConstruction::ConstructSphere()
{
    G4Sphere* Sphere = new G4Sphere("sphere",4.9*cm,5*cm,0.0,2.0*M_PI,0.0,M_PI);

    log_Sphere = new G4LogicalVolume(Sphere, world_mat , "log_Sphere");

    G4VisAttributes* SphereVisAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    SphereVisAttributes->SetVisibility(true);
    log_Sphere->SetVisAttributes(SphereVisAttributes);
    new G4PVPlacement(0, G4ThreeVector(0,0,0), log_Sphere,"sphere", log_world, false, 0);

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

//    sensitiveDetector3 = new SensitiveDetector("SensitiveDetector3", 3);
//    SDman -> AddNewDetector(sensitiveDetector3);
//   log_Sphere->SetSensitiveDetector(sensitiveDetector3);
}




void DetectorConstruction::ConstructWaterPhantom() 
{
   G4double phantomSize = 10*cm; 
   G4Box* phantomSolid = new G4Box("Phantom", phantomSize/2, phantomSize/2, phantomSize/2);

   G4LogicalVolume* phantomLogic = new G4LogicalVolume(phantomSolid, water_mat, "PhantomLogic");

   G4VisAttributes* visAttributes = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0)); 
   phantomLogic->SetVisAttributes(visAttributes);

   new G4PVPlacement(0, G4ThreeVector(), phantomLogic, "Phantom", log_world, false, 0, true);
}

void DetectorConstruction::ConstructQuartzRadiator(const G4String &name)
{

    G4double a2, z1, density1, fractionmass;
    G4int nelements1, natoms, nelements2;

    G4Element* Si = new G4Element("Silicium", "Si", z1 = 14, a2 = 28.086 * g / mole);
    G4Element* Pb = new G4Element("Plumbum", "Pb", z1 = 82, a2 = 207.19 * g / mole);
    G4Element* O = new G4Element("Oxygenium", "O", z1 = 8, a2 = 16 * g / mole);
    G4Element* K = new G4Element("Kalium", "K", z1 = 19, a2 = 39.098 * g / mole);
    G4Element* As = new G4Element("Arsenicum", "As", z1 = 33, a2 = 74.922 * g / mole);
    G4Element* Na = new G4Element("Natrium", "C", z1 = 11, a2 = 22.99 * g / mole);
    G4Element* Cs = new G4Element("Caesium","Cs", z1 =55, a2 = 133*g/mole);
    G4Element* I = new G4Element("Iodine","I", z1 =53, a2 = 127*g/mole);

    G4Material* PbO = new G4Material("PbO", density1 = 9.53 * g / cm3, nelements1 = 2);
    PbO->AddElement(Pb, natoms=1);
    PbO->AddElement(O, natoms=1);

    G4Material* K2O = new G4Material("K2O", density1 = 2.35 * g / cm3, nelements1 = 2);
    K2O->AddElement(K, natoms=2);
    K2O->AddElement(O, natoms=1);

    G4Material* As2O3 = new G4Material("As2O3", density1 = 3.74 * g / cm3, nelements1 = 2);
    As2O3->AddElement(As, natoms=2);
    As2O3->AddElement(O, natoms=3);

    G4Material* Na2O = new G4Material("Na2O", density1 = 2.27 * g / cm3, nelements1 = 2);
    Na2O->AddElement(Na, natoms=2);
    Na2O->AddElement(O, natoms=1);

    G4Material* SiO2 = new G4Material("SiO2", density1 = 2.64 * g / cm3, nelements1 = 2);
    SiO2->AddElement(Si, natoms=1);
    SiO2->AddElement(O, natoms=2);

    //Material CsI
    double density3 = 4.51*g/cm3;
    G4Material* CsI = new G4Material("CsI", density3, nelements2 = 2);
    CsI->AddElement(Cs, natoms=1);
    CsI->AddElement(I, natoms=1);


    //Material LeadGlass (LG)
    double density = 5.2*g/cm3;
    G4Material* LG = new G4Material("lead_glass", density, nelements1 = 5);
    LG->AddMaterial(SiO2, fractionmass = 57.33 * perCent);
    LG->AddMaterial(PbO, fractionmass = 40.12 * perCent);
    LG->AddMaterial(K2O, fractionmass = 1.34 * perCent);
    LG->AddMaterial(Na2O, fractionmass = 1.02 * perCent);
    LG->AddMaterial(As2O3, fractionmass = 0.19 * perCent);

    //LG scintillation properties
    const G4int LGNUMENTRIES = 30;

    G4double LG_PP[LGNUMENTRIES] = {1.9074461538461538*eV,  1.943532972972973*eV,  1.9810115702479338*eV,  
                                            2.01996404494382*eV,  2.060479083094556*eV,  2.1026526315789473*eV,  
                                            2.1465886567164176*eV,  2.1923999999999997*eV,  2.2402093457943923*eV,  
                                            2.2901503184713374*eV,  2.3423687296416937*eV,  2.3970239999999996*eV,  
                                            2.454290784982935*eV, 2.514360839160839*eV,  2.5774451612903224*eV, 
                                             2.643776470588235*eV,  2.713612075471698*eV,  2.7872372093023254*eV,  
                                             2.8649689243027887*eV,  2.947160655737705*eV,  3.0342075949367087*eV,  
                                             3.1265530434782605*eV,  3.2246959641255604*eV, 3.3292*eV,  
                                             3.4407043062200953*eV,  3.559936633663366*eV,  3.6877292307692304*eV,  
                                             3.8250382978723403*eV,  3.9729679558011046*eV,  4.1328*eV};//диапазон длин волн

    G4double LG_FAST[LGNUMENTRIES] = {0.00000000e+00, 0, 0, 0,
                                         1.07467447e-01, 1.45667801e-01, 1.86620023e-01, 2.29304120e-01,
                                         2.72742648e-01, 3.17343270e-01, 3.85404287e-01, 4.82743181e-01,
                                         6.07456021e-01, 7.19110047e-01, 8.25161380e-01, 9.25210762e-01,
                                         9.36216955e-01, 7.88027704e-01, 5.09066955e-01, 2.66182775e-01,
                                         1.31208635e-01, 4.19477247e-02, 5.57075520e-03, 4.16742738e-03,
                                         3.47285615e-03, 2.77828492e-03, 2.08371369e-03, 1.38914246e-03,
                                         6.94571230e-04, 0.00000000e+00};// нормированный спектр высвечивания в зависимости от длины волны

    G4double LG_ABS[LGNUMENTRIES] = {100*cm, 100*cm,  100*cm, 100*cm, 100*cm, 100*cm,
                                       100*cm, 100*cm, 100*cm, 100*cm, 100*cm, 100*cm,
                                       100*cm, 100*cm, 100*cm, 100*cm, 100*cm, 100*cm,
                                       100*cm,  100*cm, 100*cm, 100*cm, 100*cm, 100*cm,
                                       100*cm, 100*cm, 100*cm, 100*cm, 100*cm,  100*cm };;//средняя длина поглощенеия в зависимости от длины волны 

    G4double LG_RINDEX[LGNUMENTRIES] = {1.58, 1.5927809814338914, 1.5973731100158748, 
                                                    1.601545505321651, 1.6057098606688374, 1.6095490261169614, 
                                                    1.6133881915650852, 1.6172273570132092, 1.6210665224613332, 
                                                    1.624905687909457, 1.628744853357581, 1.632193886199313,
                                                     1.6355831886422627, 1.6389724910852121, 1.6423617935281618, 
                                                     1.6457510959711115, 1.6490628157963894, 1.6517797243675925, 
                                                     1.6544966329387956, 1.6572135415099987, 1.6599304500812018, 
                                                     1.6626658829870584, 1.6655827372106877, 1.668499591434317, 
                                                    1.6709960716286392, 1.6729102572128958, 1.6748244427971526,
                                                     1.6767386283814092, 1.6783915185563154, 1.68};//коэффициент преломленемия  в зависимости от длины волны

    G4MaterialPropertiesTable* LG_MPT = new G4MaterialPropertiesTable(); //новая таблица параметров сцинтиллятора

    LG_MPT -> AddProperty("RINDEX",LG_PP, LG_RINDEX, LGNUMENTRIES);//коэффициент преломления
    LG_MPT -> AddProperty("ABSLENGTH",LG_PP, LG_ABS, LGNUMENTRIES);//длина поглощения

    //ДЛЯ сцинтилляции :

    // LG_MPT->AddProperty("SCINTILLATIONCOMPONENT1", LG_PP, LG_FAST, LGNUMENTRIES);//спектр высвечивания
    // LG_MPT -> AddConstProperty("SCINTILLATIONYIELD", 10000./MeV);//светимость, колическово фотонов на потерю МэВ энерии
    // LG_MPT -> AddConstProperty("RESOLUTIONSCALE", 1.0);
    // LG_MPT -> AddConstProperty("SCINTILLATIONTIMECONSTANT1", 8*ns);//время высвечивания сцинтиллятора

    // LG_MPT -> AddConstProperty("SCINTILLATIONYIELD1",1.0);

 //  LG->SetMaterialPropertiesTable(LG_MPT);// применение свойст сцинтиллятора к материалу



    // размеры
    radiatorThin = 25 * cm;
    G4double radiatorLength = 5.0 * cm;
    G4double radiatorHeight = 10.0 * cm;

    // радиатор
    G4Box* radiatorSolid = new G4Box(name, radiatorHeight/2, radiatorThin / 2, radiatorLength/2 );

    G4LogicalVolume* radiatorLogical = new G4LogicalVolume(radiatorSolid, CsI , name + "_log");

    G4VisAttributes* radiatorVisAttributes = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8));
    radiatorVisAttributes->SetVisibility(true);
    radiatorLogical->SetVisAttributes(radiatorVisAttributes);

    // поворот радиатора
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateY(90.0 * deg);

    // расположение 1 и 2 радиатора
    if (name == "QuartzRadiator1") {
        new G4PVPlacement(rot, G4ThreeVector(0, distance1, 0), radiatorLogical, name, log_world, true, 0);
    } else if (name == "QuartzRadiator2") {
        new G4PVPlacement(rot, G4ThreeVector(0, -distance2, 0), radiatorLogical, name, log_world, true, 0);
    }





    G4OpticalSurface* opAirSurface = new G4OpticalSurface("AirSurface");//создание новой поверхности (для сцинтиллятора в данном случае)
    opAirSurface->SetType(dielectric_dielectric); // тип поверхности (диэлектрик-диэлектик (вохдух-сцинтиллятор))
    opAirSurface->SetFinish(ground); // шероховатость
    opAirSurface->SetModel(unified);
    // opAirSurface->SetSigmaAlpha(1);

    G4LogicalSkinSurface* airSurface1 =
     new G4LogicalSkinSurface("AirSurface1", radiatorLogical, opAirSurface); //создание самой поверхности.
 
}