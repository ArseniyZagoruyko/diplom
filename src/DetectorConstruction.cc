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
#include "DetectorSphere.hh"
#include "SteppingAction.hh"
#include "G4RegionStore.hh"

G4LogicalVolume* radiatorLogical;

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



    distance1 = 30* cm;
    distance2 = -30 * cm;

    ConstructLGRadiator();
    
    //ConstructSphere();
    
    // ConstructWaterPhantom();

    // поворот для тонких объемов
    G4RotationMatrix* rotThin = new G4RotationMatrix();
    rotThin->rotateY(90.0 * deg);

    // размеры тонких объемов
    G4double HightVolume = 8.0 * cm;
    G4double ThinVolume = 0.1 * cm;
    G4double LenghtVolume = 8.0 * cm;

    G4double a2, z1, density1, fractionmass;
    G4int nelements1, natoms, nelements2;

    G4Element* Si = new G4Element("Silicium", "Si", z1 = 14, a2 = 28.086 * g / mole);
    G4Material* SiPM = new G4Material("SiPM", density1 = 2.33 * g / cm3, nelements1 = 1);
    SiPM->AddElement(Si, natoms=1);

    // тонкий объем 1
    G4Box* thinVolumeSolid1 = new G4Box("ThinVolume1", HightVolume/2, ThinVolume / 2, LenghtVolume/2);
    thinVolumeLogical1 = new G4LogicalVolume(thinVolumeSolid1, SiPM, "ThinVolume1_log");

    G4VisAttributes* visAttributes1 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    thinVolumeLogical1->SetVisAttributes(visAttributes1);

    new G4PVPlacement(rotThin, G4ThreeVector(0, distance1 + radiatorThin/2 + ThinVolume/2, 0), thinVolumeLogical1, "ThinVolume1_phys", log_world, false, 0);

    // тонкий объем 2
    G4Box* thinVolumeSolid2 = new G4Box("ThinVolume2", HightVolume/2, ThinVolume / 2, LenghtVolume/2);
    thinVolumeLogical2 = new G4LogicalVolume(thinVolumeSolid2, SiPM, "ThinVolume2_log");

    G4VisAttributes* visAttributes2 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    thinVolumeLogical2->SetVisAttributes(visAttributes2);

    new G4PVPlacement(rotThin, G4ThreeVector(0, distance2 - radiatorThin/2 -ThinVolume/2, 0), thinVolumeLogical2, "ThinVolume2_phys", log_world, false, 0);



    log_world->SetVisAttributes(G4VisAttributes::GetInvisible());

    return phys_world;
}

void DetectorConstruction::ConstructSphere()
{
    G4Sphere* Sphere = new G4Sphere("sphere",9.9*cm,10*cm,0.0,2.0*M_PI,0.0,M_PI);

    log_Sphere = new G4LogicalVolume(Sphere, world_mat , "log_Sphere");

    G4VisAttributes* SphereVisAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    SphereVisAttributes->SetVisibility(true);
    log_Sphere->SetVisAttributes(SphereVisAttributes);
    new G4PVPlacement(0, G4ThreeVector(0,0,0), log_Sphere,"sphere", log_world, false, 0);

    //ДЛЯ СПЕКТРА ФОТОНОВ потом удалить
    DetectorSphere* detector = new DetectorSphere("Detector");
    G4SDManager::GetSDMpointer()->AddNewDetector(detector);
    log_Sphere->SetSensitiveDetector(detector);
    



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

   new G4PVPlacement(0, G4ThreeVector(0,0*cm,0), phantomLogic, "Phantom", log_world, false, 0, true);
}

void DetectorConstruction::ConstructLGRadiator()
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
    G4Element* Ti = new G4Element("Titanium","Ti", z1 =22, a2 = 47.867*g/mole );
    G4Element* Alu = new G4Element("Aluminium", "Al", z1 = 13, a2 =26.98 *g/mole);

    G4Material* Al = new G4Material("Al", density1 = 2.70 * g / cm3, nelements1 = 1);
    Al->AddElement(Alu, natoms=1);

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

    //Material PbF2
    G4double density3 = 7.77 * g/cm3; // Плотность PbF2 
    G4NistManager* nist = G4NistManager::Instance(); 
    G4Material* PbF2 = new G4Material("LeadFluoride", density3, 2); 
    G4Element* F = nist->FindOrBuildElement("F"); 
    PbF2->AddElement(Pb, 1); 
    PbF2->AddElement(F, 2); 
    
    // Создание таблицы оптических свойств 
    double wavelength[] = {300*nm, 400*nm, 500*nm, 600*nm, 700*nm, 800*nm, 900*nm, 1000*nm, 3000*nm, 5000*nm, 7000*nm, 9000*nm}; 
    double refractiveIndex[] = {1.93665, 1.81804, 1.78220, 1.76489, 1.75502, 1.74897, 1.74455, 1.74150, 1.72363, 1.68544, 1.68544, 1.65504}; 
    double absorptionLength[] = {1.0*cm, 1.5*cm, 2.0*cm, 2.5*cm, 3.0*cm, 3.5*cm, 4.0*cm, 4.5*cm, 5.0*cm, 5.5*cm, 6.0*cm, 6.5*cm}; 
    
    G4MaterialPropertiesTable* mptPbF2 = new G4MaterialPropertiesTable(); 
    mptPbF2->AddProperty("RINDEX", wavelength, refractiveIndex, 12); 
    mptPbF2->AddProperty("ABSLENGTH", wavelength, absorptionLength, 12); 
    
    PbF2->SetMaterialPropertiesTable(mptPbF2);   


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

    LG->SetMaterialPropertiesTable(LG_MPT);// применение свойст сцинтиллятора к материалу

    //reflective prop for Al(c ti02 не получилось, поэтому покрытие - просто краска с коэф отр 0.9)
    const G4int Al_NUMENTRIES = 15;

    G4double Al_PP[Al_NUMENTRIES] = {0.07*eV, 0.08*eV, 0.09*eV, 0.1*eV, 0.12*eV, 0.14*eV, 0.16*eV, 0.18*eV, 0.2*eV, 0.26*eV , 0.45*eV,  0.5*eV, 0.7*eV, 0.9*eV , 1*eV};//диапазон длин волн

    G4double Al_RINDEX[Al_NUMENTRIES] = {50.952, 43.77, 38.46, 33.519, 26.216, 20.837, 16.755, 14.088, 12.195, 8.58, 3.938, 3.34, 1.92, 1.39, 1.31};//коэффициент преломленемия  в зависимости от длины волны

    G4double Al_Reflectivity[Al_NUMENTRIES] = {0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9};
    G4double Al_Efficiency[Al_NUMENTRIES] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; 
    
    G4MaterialPropertiesTable* Al_MPT = new G4MaterialPropertiesTable();
    Al_MPT->AddProperty ("RINDEX", Al_PP,Al_RINDEX, Al_NUMENTRIES);
    Al_MPT->AddProperty("REFLECTIVITY", Al_PP, Al_Reflectivity, Al_NUMENTRIES);
    Al->SetMaterialPropertiesTable(Al_MPT);
    



    // размеры
    radiatorThin = 1 * cm;
    radiatorLength = 1.9 * cm;
    radiatorHeight = 1.9 * cm;

    // радиатор
    G4Box* radiatorSolid = new G4Box("radiator", radiatorHeight/2, radiatorThin / 2, radiatorLength/2 );

    radiatorLogical = new G4LogicalVolume(radiatorSolid, PbF2 , "radiatorlog");

    G4VisAttributes* radiatorVisAttributes = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8));
    radiatorVisAttributes->SetVisibility(true);
    radiatorLogical->SetVisAttributes(radiatorVisAttributes);


        //Регион для радиатора
    G4Region* radiatorRegion= new G4Region("RadiatorRegion");
    radiatorLogical->SetRegion(radiatorRegion);
    radiatorRegion->AddRootLogicalVolume(radiatorLogical);

    // поворот радиатора
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateY(90.0 * deg);


    //физические объемы радиаторов
    const int numRadiatorColumns = 4; 
    const int numRadiatorRows = 4; 
    const double radiatorSpacing = 2.0 * cm; 
    


    std::vector<G4VPhysicalVolume*> radiatorPhysVector; // для хранения указателей на радиаторы

    for (int i = 0; i < numRadiatorColumns; ++i) 
    { 
        for (int j = 0; j < numRadiatorRows; ++j) 
        { 
        
            G4ThreeVector radiatorPosition(-3 * cm + i * radiatorSpacing, distance1, -3 * cm + j * radiatorSpacing); 
    
            G4VPhysicalVolume* currentRadiator = new G4PVPlacement(rot, radiatorPosition, radiatorLogical, "Radiator" + std::to_string(i * numRadiatorRows + j + 1), log_world, true, 0);

            radiatorPhysVector.push_back(currentRadiator);
        } 
    }

    for (int l = 0; l < numRadiatorColumns; ++l) 
    { 
        for (int k = 0; k < numRadiatorRows; ++k) 
        { 
        
            G4ThreeVector radiatorPosition(-3 * cm + l * radiatorSpacing, distance2, -3 * cm + k * radiatorSpacing); 
    
            G4VPhysicalVolume* currentRadiator = new G4PVPlacement(rot, radiatorPosition, radiatorLogical, "Radiator" + std::to_string(l * numRadiatorRows + k + 1), log_world, true, 0);

            radiatorPhysVector.push_back(currentRadiator);
        } 
    }




    //Рефлекторы
    G4double ReflectorThin = 0.1*cm;
    G4double ReflectorLenght = 1.0*cm;
    G4double ReflectorHeight = 1.9*cm;

    G4Box* ReflectorSolid = new G4Box("Reflector", ReflectorThin/2, ReflectorLenght / 2, ReflectorHeight/2);
    G4LogicalVolume* ReflectorLogical = new G4LogicalVolume(ReflectorSolid, Al, "Reflector_log");

    G4VisAttributes* ReflectorVisAttributes = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)); 
    ReflectorVisAttributes->SetVisibility(true);
    ReflectorLogical->SetVisAttributes(ReflectorVisAttributes);

    G4RotationMatrix* rot1 = new G4RotationMatrix();
    rot1->rotateY(90.0 * deg);

    G4VPhysicalVolume* ReflectorPhys1 = new G4PVPlacement(0, G4ThreeVector(radiatorHeight/2 + ReflectorThin/2, 0, 0), ReflectorLogical, "Reflector1", radiatorLogical, false, 0);
    G4VPhysicalVolume* ReflectorPhys2 = new G4PVPlacement(0, G4ThreeVector(-radiatorHeight/2 - ReflectorThin/2, 0, 0), ReflectorLogical, "Reflector2", radiatorLogical, false, 0);
    G4VPhysicalVolume* ReflectorPhys3 = new G4PVPlacement(rot1, G4ThreeVector(0, 0, radiatorLength/2 +ReflectorThin/2), ReflectorLogical, "Reflector3", radiatorLogical, false, 0);
    G4VPhysicalVolume* ReflectorPhys4 = new G4PVPlacement(rot1, G4ThreeVector(0, 0, -radiatorLength/2 - ReflectorThin/2), ReflectorLogical, "Reflector4", radiatorLogical, false, 0);


    // отражающая поверхность между радиаторами и рефлекторами

    G4VPhysicalVolume* reflectorPhys[4] = {ReflectorPhys1, ReflectorPhys2, ReflectorPhys3, ReflectorPhys4};

    G4OpticalSurface* OpSurface = new G4OpticalSurface("name");

    OpSurface->SetType(dielectric_metal);
    OpSurface->SetModel(unified);
    OpSurface->SetFinish(polished);
    OpSurface->SetSigmaAlpha(0.1);

    std::vector<G4double> pp = {2.038*eV, 4.144*eV};
    std::vector<G4double> specularlobe = {0.3, 0.3};
    std::vector<G4double> specularspike = {0.2, 0.2};
    std::vector<G4double> backscatter = {0.1, 0.1};
    std::vector<G4double> rindex = {1.35, 1.40};
    std::vector<G4double> reflectivity = {0.9, 0.9};
    std::vector<G4double> efficiency = {0.8, 0.1};
    G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
    SMPT->AddProperty("RINDEX", pp, rindex);
    SMPT->AddProperty("SPECULARLOBECONSTANT", pp, specularlobe);
    SMPT->AddProperty("SPECULARSPIKECONSTANT", pp, specularspike);
    SMPT->AddProperty("BACKSCATTERCONSTANT", pp, backscatter);
    SMPT->AddProperty("REFLECTIVITY", pp, reflectivity);
    SMPT->AddProperty("EFFICIENCY", pp, efficiency);
    OpSurface->SetMaterialPropertiesTable(SMPT);

    for (int i = 0; i <radiatorPhysVector.size(); i++)
    {
        G4VPhysicalVolume* currentRadiator = radiatorPhysVector[i];

        for (int j = 0; j < 4; j++)
        {
            G4VPhysicalVolume* currentReflector = reflectorPhys[j];

            G4LogicalBorderSurface* Surface = new G4LogicalBorderSurface("name",currentRadiator, currentReflector ,OpSurface);
            
        }

    }



    G4OpticalSurface* opAirSurface = new G4OpticalSurface("AirSurface");//создание новой поверхности 
    opAirSurface->SetType(dielectric_dielectric); // тип поверхности диэлектрик-диэлектик (вохдух-сцинтиллятор)
    opAirSurface->SetFinish(ground); // шероховатость
    opAirSurface->SetModel(unified);
    // opAirSurface->SetSigmaAlpha(1);

    G4LogicalSkinSurface* airSurface1 =
    new G4LogicalSkinSurface("AirSurface1", radiatorLogical, opAirSurface); 
 
}

