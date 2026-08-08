#include "AMSimPresentationAssetCommandlet.h"

#include "AMSimComponentGallery.h"
#include "AMSimRootScreen.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "AutomatedAssetImportData.h"
#include "Engine/Texture2D.h"
#include "FileHelpers.h"
#include "IAssetTools.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "PaperSprite.h"
#include "Sound/SoundGroups.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundWaveLoadingBehavior.h"
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

	struct FUISoundSource
	{
		const TCHAR* StableId;
		float RuntimeVolume;
	};

	bool ImportUISounds(
		const FString& SourceRoot,
		TArray<UPackage*>& PackagesToSave)
	{
		static const FUISoundSource Sources[] = {
			{TEXT("ui_hover_soft"), 0.70f},
			{TEXT("ui_focus_tick"), 0.90f},
			{TEXT("ui_click_primary"), 8.00f},
			{TEXT("ui_click_secondary"), 1.60f},
			{TEXT("ui_tab_switch"), 0.40f},
			{TEXT("ui_panel_open"), 0.80f},
			{TEXT("ui_panel_close"), 0.95f},
			{TEXT("ui_back"), 1.15f},
			{TEXT("ui_build_start"), 0.60f},
			{TEXT("ui_build_place"), 0.60f},
			{TEXT("ui_build_snap"), 0.65f},
			{TEXT("ui_build_confirm"), 0.50f},
			{TEXT("ui_build_cancel"), 1.90f},
			{TEXT("ui_invalid_geometry"), 0.65f},
			{TEXT("ui_offer_available"), 0.65f},
			{TEXT("ui_offer_accept"), 0.70f},
			{TEXT("ui_offer_decline"), 3.30f},
			{TEXT("ui_schedule_confirm"), 1.10f},
			{TEXT("ui_schedule_rejected"), 0.80f},
			{TEXT("ui_notification"), 3.80f},
			{TEXT("ui_save_success"), 1.85f},
			{TEXT("ui_load_success"), 0.50f},
			{TEXT("ui_load_failure"), 0.75f},
			{TEXT("ui_warning_attention"), 1.05f},
			{TEXT("ui_objective_complete"), 1.10f},
			{TEXT("ui_capability_unlock"), 1.30f},
			{TEXT("ui_incident_alert"), 0.75f},
			{TEXT("ui_recovery_success"), 0.55f}};

		bool bPassed = true;
		for (const FUISoundSource& Source : Sources)
		{
			const FString DiskPath = FPaths::Combine(
				SourceRoot,
				FString(Source.StableId) + TEXT(".mp3"));
			const FString ObjectPath = FString::Printf(
				TEXT("/Game/Audio/UI/%s.%s"),
				Source.StableId,
				Source.StableId);
			UAutomatedAssetImportData* ImportData =
				NewObject<UAutomatedAssetImportData>();
			ImportData->Filenames = {DiskPath};
			ImportData->DestinationPath = TEXT("/Game/Audio/UI");
			ImportData->bReplaceExisting = true;
			ImportData->bSkipReadOnly = true;
			const TArray<UObject*> Imported =
				FAssetToolsModule::GetModule().Get().ImportAssetsAutomated(ImportData);
			USoundWave* Sound = Imported.IsEmpty()
				? LoadObject<USoundWave>(nullptr, *ObjectPath)
				: Cast<USoundWave>(Imported[0]);
			if (!Sound || Sound->GetDuration() <= 0.0f ||
				Sound->GetDuration() > 2.0f || Sound->NumChannels != 2)
			{
				UE_LOG(
					LogTemp,
					Error,
					TEXT("Failed UI sound import validation: %s"),
					*DiskPath);
				bPassed = false;
				continue;
			}
			Sound->bLooping = false;
			Sound->SoundGroup = SOUNDGROUP_UI;
			Sound->Volume = Source.RuntimeVolume;
			Sound->OverrideLoadingBehavior(
				ESoundWaveLoadingBehavior::RetainOnLoad);
			Sound->MarkPackageDirty();
			PackagesToSave.AddUnique(Sound->GetPackage());
			UE_LOG(
				LogTemp,
				Display,
				TEXT("Prepared UI sound: %s (%.2fs, volume %.2f)"),
				Source.StableId,
				Sound->GetDuration(),
				Source.RuntimeVolume);
		}
		return bPassed;
	}

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
	const FString UIAudioSourceRoot = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(FPaths::ProjectDir(), TEXT("../SourceAssets/Audio/UI")));
	if (FParse::Param(*Params, TEXT("UISoundsOnly")))
	{
		TArray<UPackage*> UIAudioPackages;
		if (!ImportUISounds(UIAudioSourceRoot, UIAudioPackages) ||
			!UEditorLoadingAndSavingUtils::SavePackages(UIAudioPackages, true))
		{
			UE_LOG(LogTemp, Error, TEXT("UI sound asset generation failed."));
			return 1;
		}
		UE_LOG(
			LogTemp,
			Display,
			TEXT("UI sound asset generation complete: 28 sounds."));
		return 0;
	}
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
	const FString Phase6SourceRoot = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(
			FPaths::ProjectDir(),
			TEXT("../SourceAssets/Phase6/Aircraft")));
	const FString TerminalGrowthSourceRoot = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(
			FPaths::ProjectDir(),
			TEXT("../SourceAssets/TerminalGrowth/Runtime/Textures")));
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
		MakeSource(TEXT("Surfaces"), TEXT("T_RunwayMarked"), TEXT("S_RunwayMarked")),
		MakeSource(TEXT("Surfaces"), TEXT("T_TaxiwayMarked"), TEXT("S_TaxiwayMarked")),
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
	const auto MakePhase6AircraftSource = [&Phase6SourceRoot](
		const int32 HeadingIndex)
	{
		const FString TextureName = FString::Printf(
			TEXT("T_RiverbendLongreach787_9_H%02d"),
			HeadingIndex);
		const FString SpriteName = FString::Printf(
			TEXT("S_RiverbendLongreach787_9_H%02d"),
			HeadingIndex);
		return FPresentationSpriteSource{
			FPaths::Combine(
				Phase6SourceRoot,
				TextureName + TEXT(".png")),
			TEXT("/Game/Phase6/Presentation/Textures/Aircraft"),
			FString::Printf(
				TEXT("/Game/Phase6/Presentation/Textures/Aircraft/%s.%s"),
				*TextureName,
				*TextureName),
			FString::Printf(
				TEXT("/Game/Phase6/Presentation/Sprites/Aircraft/%s"),
				*SpriteName)};
	};
	for (int32 HeadingIndex = 0; HeadingIndex < 16; ++HeadingIndex)
	{
		Sources.Add(MakePhase6AircraftSource(HeadingIndex));
	}
	const auto MakeTerminalGrowthSource = [&TerminalGrowthSourceRoot](
		const TCHAR* Category,
		const TCHAR* AssetName)
	{
		const FString TextureName = FString::Printf(TEXT("T_%s"), AssetName);
		const FString SpriteName = FString::Printf(TEXT("S_%s"), AssetName);
		return FPresentationSpriteSource{
			FPaths::Combine(TerminalGrowthSourceRoot, TextureName + TEXT(".png")),
			FString::Printf(TEXT("/Game/TerminalGrowth/Presentation/Textures/%s"), Category),
			FString::Printf(
				TEXT("/Game/TerminalGrowth/Presentation/Textures/%s/%s.%s"),
				Category,
				*TextureName,
				*TextureName),
			FString::Printf(
				TEXT("/Game/TerminalGrowth/Presentation/Sprites/%s/%s"),
				Category,
				*SpriteName)};
	};
	struct FTerminalGrowthCategory
	{
		const TCHAR* Name;
		TArray<const TCHAR*> Assets;
	};
	const FTerminalGrowthCategory TerminalGrowthCategories[] = {
		{TEXT("Surface"), {TEXT("PublicFloor"), TEXT("ServiceFloor"), TEXT("RoofSurface")}},
		{TEXT("Structure"), {TEXT("ExteriorWall"), TEXT("InteriorWall"), TEXT("GlassWall")}},
		{TEXT("Door"), {TEXT("EntranceDoor"), TEXT("StandardDoor"), TEXT("ServiceDoor"), TEXT("AirsideGateDoor")}},
		{TEXT("Roof"), {TEXT("RoofEdge"), TEXT("RoofCorner"), TEXT("EntranceCanopy"), TEXT("RoofHVAC")}},
		{TEXT("Furniture"), {TEXT("StaffDesk"), TEXT("Storage"), TEXT("Vending"), TEXT("Water"), TEXT("WasteRecycling"), TEXT("Noticeboard"), TEXT("Signage"), TEXT("Plant")}},
		{TEXT("Construction"), {TEXT("ConstructionFoundation"), TEXT("ConstructionDelivery"), TEXT("ConstructionFraming"), TEXT("ConstructionActive"), TEXT("ConstructionInspection"), TEXT("ConstructionClosure")}},
		{TEXT("Overlay"), {TEXT("PlacementValid"), TEXT("PlacementBlocked"), TEXT("PlacementUnaffordable"), TEXT("PlacementRouteLoss"), TEXT("PlacementSnap"), TEXT("PlacementConnection")}},
		{TEXT("Icon"), {TEXT("IconStructure"), TEXT("IconDoors"), TEXT("IconFurniture"), TEXT("IconAmenities"), TEXT("IconOperations"), TEXT("IconZones"), TEXT("IconDemolish"), TEXT("IconRotate"), TEXT("IconCopy"), TEXT("IconUndo")}}
	};
	for (const FTerminalGrowthCategory& Category : TerminalGrowthCategories)
	{
		for (const TCHAR* AssetName : Category.Assets)
		{
			Sources.Add(MakeTerminalGrowthSource(Category.Name, AssetName));
		}
	}
	const bool bTerminalGrowthOnly =
		FParse::Param(*Params, TEXT("TerminalGrowthOnly"));
	if (bTerminalGrowthOnly)
	{
		Sources = Sources.FilterByPredicate(
			[](const FPresentationSpriteSource& Source)
			{
				return Source.TexturePath.Contains(TEXT("/Game/TerminalGrowth/"));
			});
	}
	const bool bPhase1MovementSurfacesOnly =
		FParse::Param(*Params, TEXT("Phase1MovementSurfacesOnly"));
	if (bPhase1MovementSurfacesOnly)
	{
		Sources = Sources.FilterByPredicate(
			[](const FPresentationSpriteSource& Source)
			{
				return Source.TexturePath.Contains(TEXT("/T_RunwayMarked.")) ||
					Source.TexturePath.Contains(TEXT("/T_TaxiwayMarked."));
			});
	}

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
	if (!bPhase1MovementSurfacesOnly && !bTerminalGrowthOnly)
	{
		bPassed &= ImportUISounds(UIAudioSourceRoot, PackagesToSave);
		bPassed &= CreateProductionRootBlueprint(PackagesToSave);
		bPassed &= CreateComponentGalleryBlueprint(PackagesToSave);
	}
	if (!bPassed || !UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true))
	{
		UE_LOG(LogTemp, Error, TEXT("Presentation asset generation failed."));
		return 1;
	}
	if (bPhase1MovementSurfacesOnly)
	{
		UE_LOG(LogTemp, Display, TEXT("Phase 1 movement-surface generation complete: %d sprites."), Sources.Num());
	}
	else if (bTerminalGrowthOnly)
	{
		UE_LOG(LogTemp, Display, TEXT("Terminal-growth asset generation complete: %d sprites."), Sources.Num());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Presentation asset generation complete: %d sprites and 28 UI sounds."), Sources.Num());
	}
	return 0;
}
