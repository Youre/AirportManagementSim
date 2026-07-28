#include "AMSimPresentationAssetCommandlet.h"

#include "AMSimComponentGallery.h"
#include "AMSimRootScreen.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "AutomatedAssetImportData.h"
#include "Engine/Texture2D.h"
#include "FileHelpers.h"
#include "IAssetTools.h"
#include "Misc/Paths.h"
#include "PaperSprite.h"
#include "SpriteEditorOnlyTypes.h"
#include "UObject/Package.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"

namespace
{
	struct FPresentationSpriteSource
	{
		FString DiskPath;
		FString TextureFolder;
		FString TexturePath;
		FString SpritePackage;
	};

	bool ImportTexture(
		const FPresentationSpriteSource& Source,
		TArray<UPackage*>& PackagesToSave)
	{
		UAutomatedAssetImportData* ImportData = NewObject<UAutomatedAssetImportData>();
		ImportData->Filenames = {Source.DiskPath};
		ImportData->DestinationPath = Source.TextureFolder;
		ImportData->bReplaceExisting = true;
		ImportData->bSkipReadOnly = true;
		const TArray<UObject*> Imported =
			FAssetToolsModule::GetModule().Get().ImportAssetsAutomated(ImportData);
		if (Imported.IsEmpty() && !LoadObject<UTexture2D>(nullptr, *Source.TexturePath))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to import presentation texture: %s"), *Source.DiskPath);
			return false;
		}
		UTexture2D* Texture = Imported.IsEmpty()
			? LoadObject<UTexture2D>(nullptr, *Source.TexturePath)
			: Cast<UTexture2D>(Imported[0]);
		if (!Texture)
		{
			UE_LOG(LogTemp, Error, TEXT("Imported asset is not a texture: %s"), *Source.DiskPath);
			return false;
		}
		Texture->MarkPackageDirty();
		PackagesToSave.AddUnique(Texture->GetPackage());
		return true;
	}

	bool CreateOrUpdateSprite(
		const FPresentationSpriteSource& Source,
		TArray<UPackage*>& PackagesToSave)
	{
		UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *Source.TexturePath);
		if (!Texture)
		{
			UE_LOG(LogTemp, Error, TEXT("Presentation sprite source is missing: %s"), *Source.TexturePath);
			return false;
		}

		UPackage* Package = CreatePackage(*Source.SpritePackage);
		const FString AssetName = FPackageName::GetLongPackageAssetName(Source.SpritePackage);
		UPaperSprite* Sprite = FindObject<UPaperSprite>(Package, *AssetName);
		const bool bCreated = Sprite == nullptr;
		if (bCreated)
		{
			Sprite = NewObject<UPaperSprite>(
				Package,
				*AssetName,
				RF_Public | RF_Standalone | RF_Transactional);
		}

		FSpriteAssetInitParameters Init;
		Init.SetTextureAndFill(Texture);
		Init.SetPixelsPerUnrealUnit(0.32f);
		Sprite->InitializeSprite(Init);
		Sprite->PostEditChange();
		Package->MarkPackageDirty();
		if (bCreated)
		{
			FAssetRegistryModule::AssetCreated(Sprite);
		}
		PackagesToSave.AddUnique(Package);
		UE_LOG(LogTemp, Display, TEXT("Prepared presentation sprite: %s"), *Source.SpritePackage);
		return true;
	}

	bool CreateProductionRootBlueprint(TArray<UPackage*>& PackagesToSave)
	{
		const FString PackagePath = TEXT("/Game/UI/Screens/WBP_AMSimRootScreen");
		UPackage* Package = FindPackage(nullptr, *PackagePath);
		if (!Package)
		{
			Package = CreatePackage(*PackagePath);
		}
		if (FindObject<UWidgetBlueprint>(Package, TEXT("WBP_AMSimRootScreen")))
		{
			return true;
		}

		UWidgetBlueprintFactory* Factory = NewObject<UWidgetBlueprintFactory>();
		Factory->ParentClass = UAMSimRootScreen::StaticClass();
		UObject* Created = Factory->FactoryCreateNew(
			UWidgetBlueprint::StaticClass(),
			Package,
			TEXT("WBP_AMSimRootScreen"),
			RF_Public | RF_Standalone | RF_Transactional,
			nullptr,
			GWarn);
		UWidgetBlueprint* Blueprint = Cast<UWidgetBlueprint>(Created);
		if (!Blueprint)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create production root Widget Blueprint."));
			return false;
		}
		FAssetRegistryModule::AssetCreated(Blueprint);
		Package->MarkPackageDirty();
		PackagesToSave.AddUnique(Package);
		UE_LOG(LogTemp, Display, TEXT("Created /Game/UI/Screens/WBP_AMSimRootScreen."));
		return true;
	}

	bool CreateComponentGalleryBlueprint(TArray<UPackage*>& PackagesToSave)
	{
		const FString PackagePath = TEXT("/Game/Developer/Phase15/WBP_AMSimComponentGallery");
		UPackage* Package = FindPackage(nullptr, *PackagePath);
		if (!Package)
		{
			Package = CreatePackage(*PackagePath);
		}
		if (FindObject<UWidgetBlueprint>(Package, TEXT("WBP_AMSimComponentGallery")))
		{
			return true;
		}

		UWidgetBlueprintFactory* Factory = NewObject<UWidgetBlueprintFactory>();
		Factory->ParentClass = UAMSimComponentGallery::StaticClass();
		UObject* Created = Factory->FactoryCreateNew(
			UWidgetBlueprint::StaticClass(),
			Package,
			TEXT("WBP_AMSimComponentGallery"),
			RF_Public | RF_Standalone | RF_Transactional,
			nullptr,
			GWarn);
		UWidgetBlueprint* Blueprint = Cast<UWidgetBlueprint>(Created);
		if (!Blueprint)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create development component gallery."));
			return false;
		}
		FAssetRegistryModule::AssetCreated(Blueprint);
		Package->MarkPackageDirty();
		PackagesToSave.AddUnique(Package);
		UE_LOG(LogTemp, Display, TEXT("Created development component gallery."));
		return true;
	}
}

UAMSimPresentationAssetCommandlet::UAMSimPresentationAssetCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UAMSimPresentationAssetCommandlet::Main(const FString& Params)
{
	const FString SourceRoot = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(FPaths::ProjectDir(), TEXT("../SourceAssets/Phase1.5")));
	const FString Phase45SourceRoot = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(
			FPaths::ProjectDir(),
			TEXT("../SourceAssets/Phase4.5/Sprites")));
	const FString Phase5SourceRoot = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(
			FPaths::ProjectDir(),
			TEXT("../SourceAssets/Phase5/Aircraft")));
	const auto MakeSource = [&SourceRoot](
		const TCHAR* Folder,
		const TCHAR* TextureName,
		const TCHAR* SpriteName)
	{
		return FPresentationSpriteSource{
			FPaths::Combine(SourceRoot, Folder, FString(TextureName) + TEXT(".png")),
			FString::Printf(TEXT("/Game/Phase1/Presentation/Textures/%s"), Folder),
			FString::Printf(
				TEXT("/Game/Phase1/Presentation/Textures/%s/%s.%s"),
				Folder,
				TextureName,
				TextureName),
			FString::Printf(
				TEXT("/Game/Phase1/Presentation/Sprites/%s/%s"),
				Folder,
				SpriteName)};
	};
	TArray<FPresentationSpriteSource> Sources = {
		MakeSource(TEXT("Surfaces"), TEXT("T_TemperateGrass"), TEXT("S_TemperateGrass")),
		MakeSource(TEXT("Surfaces"), TEXT("T_GrassRunway"), TEXT("S_GrassRunway")),
		MakeSource(TEXT("Surfaces"), TEXT("T_TaxiWear"), TEXT("S_TaxiWear")),
		MakeSource(TEXT("Surfaces"), TEXT("T_WarmStand"), TEXT("S_WarmStand")),
		MakeSource(TEXT("Props"), TEXT("T_WhiteSquare"), TEXT("S_WhiteSquare")),
		MakeSource(TEXT("Props"), TEXT("T_OperationsHut"), TEXT("S_OperationsHut")),
		MakeSource(TEXT("Props"), TEXT("T_Windsock"), TEXT("S_Windsock")),
		MakeSource(TEXT("Props"), TEXT("T_SelectionRing"), TEXT("S_SelectionRing"))};
	for (int32 Index = 0; Index < 16; ++Index)
	{
		const FString TextureName = FString::Printf(TEXT("T_Cessna152_Heading_%02d"), Index);
		const FString SpriteName = FString::Printf(TEXT("S_Cessna152_Heading_%02d"), Index);
		Sources.Add(MakeSource(TEXT("Aircraft"), *TextureName, *SpriteName));
	}
	const auto MakePhase45Source = [&Phase45SourceRoot](
		const TCHAR* Folder,
		const TCHAR* AssetName)
	{
		const FString TextureName =
			FString::Printf(TEXT("T_%s"), AssetName);
		const FString SpriteName =
			FString::Printf(TEXT("S_%s"), AssetName);
		return FPresentationSpriteSource{
			FPaths::Combine(
				Phase45SourceRoot,
				Folder,
				TextureName + TEXT(".png")),
			FString::Printf(
				TEXT("/Game/Phase45/Presentation/Textures/%s"),
				Folder),
			FString::Printf(
				TEXT("/Game/Phase45/Presentation/Textures/%s/%s.%s"),
				Folder,
				*TextureName,
				*TextureName),
			FString::Printf(
				TEXT("/Game/Phase45/Presentation/Sprites/%s/%s"),
				Folder,
				*SpriteName)};
	};
	static const TCHAR* OperationsSprites[] = {
		TEXT("FuelTruck"),
		TEXT("BaggageTug"),
		TEXT("BaggageCartTrain"),
		TEXT("OperationsVan"),
		TEXT("FireRescueTruck"),
		TEXT("ConstructionTruck"),
		TEXT("ShuttleBus"),
		TEXT("Taxi"),
		TEXT("RentalCar"),
		TEXT("Deicer"),
		TEXT("RampWorker"),
		TEXT("ConstructionWorker"),
		TEXT("SecurityOfficer"),
		TEXT("TerminalAgent"),
		TEXT("PassengerFamily"),
		TEXT("Passenger"),
		TEXT("Suitcase"),
		TEXT("SafetyCones"),
		TEXT("PortableStairs"),
		TEXT("PushbackTug")};
	for (const TCHAR* AssetName : OperationsSprites)
	{
		Sources.Add(MakePhase45Source(TEXT("Operations"), AssetName));
	}
	static const TCHAR* TerminalSprites[] = {
		TEXT("CheckInDesk"),
		TEXT("BagDropDesk"),
		TEXT("SecurityScanner"),
		TEXT("QueueBarriers"),
		TEXT("GatePodium"),
		TEXT("SeatingCluster"),
		TEXT("BaggageConveyor"),
		TEXT("SortingTable"),
		TEXT("BaggageCart"),
		TEXT("ReclaimCarousel"),
		TEXT("InformationDesk"),
		TEXT("RestroomBlock"),
		TEXT("EntranceDoors"),
		TEXT("SecureDoor"),
		TEXT("PartitionWall"),
		TEXT("CurbsideShelter"),
		TEXT("BusStopShelter"),
		TEXT("ParkingKiosk"),
		TEXT("TerminalFloor"),
		TEXT("SecureFloor"),
		TEXT("CautionHatch"),
		TEXT("DirectionArrow"),
		TEXT("AccessibleRoute"),
		TEXT("ProtectionZone")};
	for (const TCHAR* AssetName : TerminalSprites)
	{
		Sources.Add(MakePhase45Source(TEXT("Terminal"), AssetName));
	}
	static const TCHAR* UISprites[] = {
		TEXT("Build"),
		TEXT("Routes"),
		TEXT("Timetable"),
		TEXT("Staff"),
		TEXT("Overlays"),
		TEXT("Alerts"),
		TEXT("Flights"),
		TEXT("Projects"),
		TEXT("Weather"),
		TEXT("Services"),
		TEXT("Passengers"),
		TEXT("Baggage"),
		TEXT("GeneralAviation"),
		TEXT("FlightSchool"),
		TEXT("Charter"),
		TEXT("MixedAirport")};
	for (const TCHAR* AssetName : UISprites)
	{
		Sources.Add(MakePhase45Source(TEXT("UI"), AssetName));
	}
	static const TCHAR* SiteSprites[] = {
		TEXT("RunwayAsphalt"),
		TEXT("TaxiwayAsphalt"),
		TEXT("ApronStand"),
		TEXT("AccessRoad"),
		TEXT("RegionalTerminal"),
		TEXT("GAHangars"),
		TEXT("OperationsStation"),
		TEXT("FuelFarm"),
		TEXT("ParkingLot"),
		TEXT("BusTaxiBay"),
		TEXT("RailPlatform"),
		TEXT("DropoffIsland"),
		TEXT("TreeCluster"),
		TEXT("LandscapeCluster"),
		TEXT("PerimeterGate"),
		TEXT("ApronFixtures")};
	for (const TCHAR* AssetName : SiteSprites)
	{
		Sources.Add(MakePhase45Source(TEXT("Site"), AssetName));
	}
	const auto MakePhase5AircraftSource = [&Phase5SourceRoot](
		const TCHAR* AssetName)
	{
		const FString TextureName =
			FString::Printf(TEXT("T_%s"), AssetName);
		const FString SpriteName =
			FString::Printf(TEXT("S_%s"), AssetName);
		return FPresentationSpriteSource{
			FPaths::Combine(
				Phase5SourceRoot,
				TextureName + TEXT(".png")),
			TEXT("/Game/Phase5/Presentation/Textures/Aircraft"),
			FString::Printf(
				TEXT("/Game/Phase5/Presentation/Textures/Aircraft/%s.%s"),
				*TextureName,
				*TextureName),
			FString::Printf(
				TEXT("/Game/Phase5/Presentation/Sprites/Aircraft/%s"),
				*SpriteName)};
	};
	Sources.Add(MakePhase5AircraftSource(TEXT("Riverlark_F28")));
	Sources.Add(MakePhase5AircraftSource(TEXT("Hearthwing_F62")));

	bool bPassed = true;
	TArray<UPackage*> PackagesToSave;
	for (const FPresentationSpriteSource& Source : Sources)
	{
		bPassed &= ImportTexture(Source, PackagesToSave);
	}
	for (const FPresentationSpriteSource& Source : Sources)
	{
		bPassed &= CreateOrUpdateSprite(Source, PackagesToSave);
	}
	bPassed &= CreateProductionRootBlueprint(PackagesToSave);
	bPassed &= CreateComponentGalleryBlueprint(PackagesToSave);
	if (!bPassed || !UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true))
	{
		UE_LOG(LogTemp, Error, TEXT("Presentation asset generation failed."));
		return 1;
	}
	UE_LOG(
		LogTemp,
		Display,
		TEXT("Presentation asset generation complete: %d sprites."),
		Sources.Num());
	return 0;
}
