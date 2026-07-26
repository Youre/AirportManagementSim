"""Create cooker-visible, internally authored Phase 2 definition assets."""

import unreal


DEFINITION_PATH = "/Game/Phase2/Definitions"
AIRCRAFT_PATH = "/Game/Phase2/Aircraft"


def identity(stable_id, display_name, kind):
    return {
        "stable_content_id": stable_id,
        "display_name": display_name,
        "localization_key": f"phase2.{stable_id.lower()}",
        "owning_phase": "Phase2",
        "provenance": "InternalApproved",
        "source": "ProjectAuthored",
        "definition_kind": kind,
    }


def create_data_asset(name, asset_class, properties, package_path):
    object_path = f"{package_path}/{name}.{name}"
    asset = unreal.EditorAssetLibrary.load_asset(object_path)
    if not asset:
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


unreal.EditorAssetLibrary.make_directory(DEFINITION_PATH)
unreal.EditorAssetLibrary.make_directory(AIRCRAFT_PATH)

for (
    asset_name,
    stable_id,
    display_name,
    role_id,
    length_cm,
    wingspan_cm,
    runway_m,
    grass_compatible,
) in [
    (
        "DA_P2Aircraft_BasicTrainer_Riverbend",
        "Aircraft.BasicTrainer.Riverbend",
        "Riverbend Basic Trainer",
        "AircraftRole.BasicTrainer",
        830,
        1100,
        600,
        True,
    ),
    (
        "DA_P2Aircraft_TouringPiston_Wayfarer",
        "Aircraft.TouringPiston.Wayfarer",
        "Wayfarer Touring Piston",
        "AircraftRole.TouringPiston",
        760,
        1070,
        620,
        True,
    ),
    (
        "DA_P2Aircraft_BusinessTurboprop_Northstar",
        "Aircraft.BusinessTurboprop.Northstar",
        "Northstar STOL Turboprop",
        "AircraftRole.BusinessTurboprop",
        1420,
        1580,
        750,
        True,
    ),
    (
        "DA_P2Aircraft_LightBusinessJet_Aster",
        "Aircraft.LightBusinessJet.Aster",
        "Aster Light Business Jet",
        "AircraftRole.LightBusinessJet",
        1480,
        1470,
        1050,
        False,
    ),
]:
    create_data_asset(
        asset_name,
        unreal.AMSimPhase2AircraftDefinition,
        {
            **identity(stable_id, display_name, "Aircraft"),
            "role_id": role_id,
            "length_centimeters": length_cm,
            "wingspan_centimeters": wingspan_cm,
            "minimum_runway_meters": runway_m,
            "grass_runway_compatible": grass_compatible,
            "fictional_livery_approved": True,
            "heading_direction_count": 16,
            "accuracy_review_id": "CT02.Phase2.InternalRoster.2026-07-26",
            "attributes": {
                "RuntimeDimension": "2D",
                "ArtSource": "ProjectAuthoredEditableSilhouette",
                "ExternalSourceFiles": "0",
            },
        },
        AIRCRAFT_PATH,
    )


definitions = [
    ("Operator_SkywardSchool", "Operator.SkywardSchool", "Skyward School", "Operator", {"Fictional": "true"}),
    ("Operator_MeadowAeroClub", "Operator.MeadowAeroClub", "Meadow Aero Club", "Operator", {"Fictional": "true"}),
    ("Operator_NorthstarCharter", "Operator.NorthstarCharter", "Northstar Charter", "Operator", {"Fictional": "true"}),
    ("Operator_AsterExecutive", "Operator.AsterExecutive", "Aster Executive", "Operator", {"Fictional": "true"}),
    ("Tenant_SkywardSchool", "Tenant.SkywardSchool", "Skyward Flight School", "Tenant", {"Specialization": "FlightSchool"}),
    ("Tenant_MeadowAeroClub", "Tenant.MeadowAeroClub", "Meadow Aero Club", "Tenant", {"Specialization": "GeneralAviation"}),
    ("Tenant_NorthstarCharter", "Tenant.NorthstarCharter", "Northstar Charter", "Tenant", {"Specialization": "Charter"}),
    ("Contract_GA", "Contract.GA.CircuitAndTouring", "Circuit and Touring Agreement", "Contract", {"FlightsPerDay": "2", "RewardCredits": "340"}),
    ("Contract_School", "Contract.FlightSchool.RecurringTraining", "Recurring Training Agreement", "Contract", {"FlightsPerDay": "3", "RewardCredits": "220"}),
    ("Contract_Charter", "Contract.Charter.ExecutiveShuttle", "Executive Shuttle Agreement", "Contract", {"FlightsPerDay": "1", "RewardCredits": "720"}),
    ("Vehicle_OpsVan", "Vehicle.OpsVan.Starter", "Operations Van", "ServiceVehicle", {"Capability": "Service.Inspection", "Depot": "Depot.Operations"}),
    ("Vehicle_FuelTruck", "Vehicle.FuelTruck.Light", "Light Fuel Truck", "ServiceVehicle", {"Capability": "Service.Fuel", "Depot": "Depot.Fuel"}),
    ("Vehicle_Deicer", "Vehicle.Deicer.Light", "Light Deicer", "ServiceVehicle", {"Capability": "Service.Deicing", "Depot": "Depot.Service"}),
    ("Vehicle_Tug", "Vehicle.Tug.Light", "Light Aircraft Tug", "ServiceVehicle", {"Capability": "Service.Tow", "Depot": "Depot.Service"}),
    ("Staff_FlightOps", "StaffRole.FlightOperations", "Flight Operations Team", "StaffRole", {"TeamSize": "2"}),
    ("Staff_GroundService", "StaffRole.GroundService", "Ground Service Team", "StaffRole", {"TeamSize": "3"}),
    ("Staff_Maintenance", "StaffRole.Maintenance", "Maintenance Team", "StaffRole", {"TeamSize": "2"}),
    ("Staff_Emergency", "StaffRole.Emergency", "Emergency Readiness Team", "StaffRole", {"TeamSize": "2"}),
    ("Parcel_EastMeadow", "Parcel.EastMeadow", "East Meadow", "Parcel", {"PurchaseCostCredits": "750"}),
    ("Facility_StandA2", "Facility.GAStand.A2", "General Aviation Stand A2", "Facility", {"Compatibility": "LightAircraft"}),
    ("Facility_StandA3", "Facility.GAStand.A3", "General Aviation Stand A3", "Facility", {"Compatibility": "LightAircraft"}),
    ("Facility_PAPI", "Facility.PAPI.Starter", "Starter PAPI", "Facility", {"RunwayDirections": "06,24"}),
    ("Service_Deicing", "Service.Deicing", "Aircraft Deicing", "ServiceTask", {"RequiredRole": "StaffRole.GroundService", "RequiredVehicle": "Vehicle.Deicer.Light"}),
    ("Service_Tow", "Service.Tow", "Aircraft Tow", "ServiceTask", {"RequiredRole": "StaffRole.GroundService", "RequiredVehicle": "Vehicle.Tug.Light"}),
    ("Weather_Cycle", "Weather.Phase2.TemperateCycle", "Temperate Operating Cycle", "WeatherProfile", {"NamedStream": "Phase2.LivingAirport", "ForecastDays": "3"}),
    ("Incident_Basic", "Incident.BasicOperations", "Basic Operational Incidents", "IncidentProfile", {"Types": "DisabledAircraft,FuelSpill,WeatherClosure", "Catastrophic": "false"}),
    ("Balance_LivingGA", "Balance.Phase2.LivingGA", "Living GA Balance", "BalanceProfile", {"OperatingDayMilliseconds": "600000", "DailyCostCredits": "180"}),
    ("Achievement_Fortnight", "Achievement.FortnightOperator", "Fortnight Operator", "Achievement", {"RequiredOperatingDays": "14", "EvidenceBased": "true"}),
    ("Presentation_LivingGA", "Presentation.LivingGA", "Living GA 2D Presentation", "PresentationDescriptor", {"Dimension": "2D", "Required3DAssets": "0", "AircraftProxyPool": "4", "VehicleProxyPool": "4"}),
]

for asset_name, stable_id, display_name, kind, attributes in definitions:
    create_data_asset(
        f"DA_P2_{asset_name}",
        unreal.AMSimPhase2Definition,
        {
            **identity(stable_id, display_name, kind),
            "attributes": attributes,
        },
        DEFINITION_PATH,
    )

unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
unreal.log("AMSim Phase 2 authored definition assets created successfully.")
