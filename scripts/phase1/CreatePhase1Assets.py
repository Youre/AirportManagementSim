"""Create the project-authored Phase 1 data assets and empty 2D presentation map."""

import unreal


DEFINITION_PATH = "/Game/Phase1/Definitions"
CORE_DEFINITION_PATH = "/Game/Phase1/CoreDefinitions"
MAP_PATH = "/Game/Maps/L_TemperateStarter"


def create_data_asset(name, asset_class, properties, package_path=DEFINITION_PATH):
    object_path = f"{package_path}/{name}.{name}"
    existing = unreal.EditorAssetLibrary.load_asset(object_path)
    if existing:
        asset = existing
    else:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property("data_asset_class", asset_class)
        asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            name, package_path, asset_class, factory
        )
    if not asset:
        raise RuntimeError(f"Could not create {object_path}")
    for property_name, value in properties.items():
        asset.set_editor_property(property_name, value)
    unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
    return asset


def identity(stable_id, display_name, localization_key):
    return {
        "stable_content_id": stable_id,
        "display_name": display_name,
        "localization_key": localization_key,
        "owning_phase": "Phase1",
        "provenance": "InternalPrototype",
        "source": "ProjectAuthored",
    }


unreal.EditorAssetLibrary.make_directory(DEFINITION_PATH)
unreal.EditorAssetLibrary.make_directory(CORE_DEFINITION_PATH)
unreal.EditorAssetLibrary.make_directory("/Game/Maps")

create_data_asset(
    "DA_Map_TemperateStarter",
    unreal.AMSimMapDefinition,
    {
        **identity(
            "Map.TemperateStarter",
            "Temperate Starter Parcel",
            "phase1.map.temperate_starter",
        ),
        "parcel_size_meters": unreal.IntPoint(1000, 1000),
        "climate_id": "Climate.Temperate.Clear",
        "supports_starter_airfield": True,
    },
)

for asset_name, stable_id, display_name, facility_type, cost, minimum_length in [
    (
        "DA_Facility_GrassRunway_Starter",
        "Facility.GrassRunway.Starter",
        "Starter Grass Runway",
        "GrassRunway",
        1600,
        400,
    ),
    (
        "DA_Facility_GrassTaxiway_Starter",
        "Facility.GrassTaxiway.Starter",
        "Starter Grass Taxiway",
        "GrassTaxiway",
        500,
        0,
    ),
    (
        "DA_Facility_GAStand_Starter",
        "Facility.GAStand.Starter",
        "Starter General Aviation Stand",
        "GAStand",
        450,
        0,
    ),
    (
        "DA_Facility_OperationsHut_Starter",
        "Facility.OperationsHut.Starter",
        "Starter Operations Hut",
        "OperationsHut",
        400,
        0,
    ),
    (
        "DA_Facility_AccessConnection_Starter",
        "Facility.AccessConnection.Starter",
        "Starter Construction Access",
        "AccessConnection",
        300,
        0,
    ),
    (
        "DA_Facility_MarkingsWindsock_Starter",
        "Facility.MarkingsWindsock.Starter",
        "Starter Runway Markings and Windsock",
        "MarkingsAndWindsock",
        150,
        0,
    ),
]:
    create_data_asset(
        asset_name,
        unreal.AMSimFacilityDefinition,
        {
            **identity(stable_id, display_name, f"phase1.{stable_id.lower()}"),
            "facility_type": facility_type,
            "base_cost_credits": cost,
            "minimum_length_meters": minimum_length,
            "required_for_starter_loop": True,
        },
    )

create_data_asset(
    "DA_Aircraft_LightPiston_Starter",
    unreal.AMSimAircraftDefinition,
    {
        **identity(
            "Aircraft.LightPiston.Starter",
            "Riverbend Light Piston",
            "phase1.aircraft.light_piston_starter",
        ),
        "length_centimeters": 720,
        "wingspan_centimeters": 1000,
        "minimum_runway_meters": 400,
        "grass_runway_compatible": True,
        "requires_inspection": True,
        "requires_fuel": True,
    },
)

create_data_asset(
    "DA_Operator_RiverbendFlyingClub",
    unreal.AMSimOperatorDefinition,
    {
        **identity(
            "Operator.RiverbendFlyingClub",
            "Riverbend Flying Club",
            "phase1.operator.riverbend_flying_club",
        ),
        "call_sign_prefix": "Riverbend",
        "fictional_brand_approved": True,
    },
)

for asset_name, stable_id, display_name, definition_kind, attributes in [
    (
        "DA_Balance_Phase1_Starter",
        "Balance.Phase1.Starter",
        "Phase 1 Starter Balance",
        "BalanceProfile",
        {"StartingCredits": "5000", "StarterPlanCost": "3400"},
    ),
    (
        "DA_Recipe_StarterAirfield",
        "Recipe.StarterAirfield",
        "Starter Grass Airfield Recipe",
        "ConstructionRecipe",
        {"ReadyToOpenMilliseconds": "120000", "ConstructionTeamSize": "3"},
    ),
    (
        "DA_StaffRole_Construction",
        "StaffRole.Construction",
        "Construction Team",
        "StaffRole",
        {"TeamSize": "3"},
    ),
    (
        "DA_StaffRole_Ramp",
        "StaffRole.Ramp",
        "Ramp Team",
        "StaffRole",
        {"TeamSize": "2"},
    ),
    (
        "DA_StaffRole_Fueling",
        "StaffRole.Fueling",
        "Fueling Team",
        "StaffRole",
        {"TeamSize": "1"},
    ),
    (
        "DA_Offer_FirstFlight",
        "Offer.FirstFlight",
        "First Compatible Visit",
        "Offer",
        {"RewardCredits": "600", "RewardAirportPoints": "5"},
    ),
    (
        "DA_Service_Inspection",
        "Service.Inspection",
        "Light Aircraft Inspection",
        "ServiceTask",
        {"RequiredRole": "StaffRole.Ramp"},
    ),
    (
        "DA_Service_Fueling",
        "Service.Fueling",
        "Light Aircraft Fueling",
        "ServiceTask",
        {"RequiredRole": "StaffRole.Fueling"},
    ),
    (
        "DA_Objective_FirstFlight",
        "Objective.FirstFlight",
        "Complete the First Flight",
        "Objective",
        {"Optional": "true"},
    ),
    (
        "DA_Weather_TemperateClear",
        "Weather.TemperateClear",
        "Clear Temperate Day",
        "WeatherProfile",
        {"VisualState": "ClearDay", "OperationalModifier": "None"},
    ),
    (
        "DA_Presentation_GrassAirfield",
        "Presentation.GrassAirfield",
        "Grass Airfield 2D Presentation",
        "PresentationDescriptor",
        {"Dimension": "2D", "AircraftArt": "InternalVectorSilhouette"},
    ),
]:
    create_data_asset(
        asset_name,
        unreal.AMSimPhase1Definition,
        {
            **identity(
                stable_id,
                display_name,
                f"phase1.{stable_id.lower()}",
            ),
            "definition_kind": definition_kind,
            "attributes": attributes,
        },
        CORE_DEFINITION_PATH,
    )

if not unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
    world_factory = unreal.WorldFactory()
    world = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        "L_TemperateStarter", "/Game/Maps", unreal.World, world_factory
    )
    if not world:
        raise RuntimeError(f"Could not create {MAP_PATH}")
    if not unreal.EditorAssetLibrary.save_loaded_asset(world, only_if_is_dirty=False):
        raise RuntimeError(f"Could not save {MAP_PATH}")
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
unreal.log("AMSim Phase 1 authored assets created successfully.")
