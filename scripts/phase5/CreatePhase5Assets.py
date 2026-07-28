"""Create cooker-visible, internally authored Phase 5 definition assets."""

import unreal


DEFINITION_PATH = "/Game/Phase5/Definitions"


def create_data_asset(name, stable_id, display_name, kind, attributes):
    object_path = f"{DEFINITION_PATH}/{name}.{name}"
    asset = None
    if unreal.EditorAssetLibrary.does_asset_exist(object_path):
        asset = unreal.EditorAssetLibrary.load_asset(object_path)
    if not asset:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property(
            "data_asset_class", unreal.AMSimPhase5Definition
        )
        asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            name,
            DEFINITION_PATH,
            unreal.AMSimPhase5Definition,
            factory,
        )
    if not asset:
        raise RuntimeError(f"Could not create {object_path}")
    properties = {
        "stable_content_id": stable_id,
        "display_name": display_name,
        "localization_key": f"phase5.{stable_id.lower()}",
        "owning_phase": "Phase5",
        "provenance": "InternalApproved",
        "source": "ProjectAuthored",
        "definition_kind": kind,
        "attributes": attributes,
    }
    for property_name, value in properties.items():
        asset.set_editor_property(property_name, value)
    unreal.EditorAssetLibrary.save_loaded_asset(
        asset, only_if_is_dirty=False
    )


unreal.EditorAssetLibrary.make_directory(DEFINITION_PATH)

aircraft = {
    "Fictional": "true",
    "ExternalSourceFiles": "0",
    "RuntimeDimension": "2D",
    "HeadingVariants": "16",
    "PixelsPerMeter": "32",
}
event = {
    "DeclinePenalty": "0",
    "OrdinarySystems": "true",
    "MidEventSaveLoad": "true",
    "PartialSuccess": "true",
}
presentation = {
    "Dimension": "2D",
    "Required3DAssets": "0",
    "RuntimeStringLoading": "false",
    "WorldAreaPercent": "60",
    "CompactAtScale": "1.75",
}

definitions = [
    ("DA_P5_Aircraft_FeederFreighter", "Aircraft.FeederFreighter.Phase5", "Riverlark F-28", "Aircraft", {**aircraft, "SizeClass": "FeederFreighter"}),
    ("DA_P5_Aircraft_RegionalFreighter", "Aircraft.RegionalFreighter.Phase5", "Hearthwing F-62", "Aircraft", {**aircraft, "SizeClass": "RegionalFreighter"}),
    ("DA_P5_Operator_MeadowFreight", "Operator.MeadowFreight", "Meadow Freight", "CargoOperator", {"Fictional": "true", "Recurring": "true"}),
    ("DA_P5_Contract_GeneralOutbound", "Contract.Cargo.GeneralOutbound", "Farm Machinery Parts", "CargoContract", {"Class": "General", "Flow": "Outbound", "DangerousGoods": "false"}),
    ("DA_P5_Contract_ExpressInbound", "Contract.Cargo.ExpressInbound", "Valley Express Mail", "CargoContract", {"Class": "MailExpress", "Flow": "Inbound", "DangerousGoods": "false"}),
    ("DA_P5_Contract_ColdTransfer", "Contract.Cargo.ColdTransfer", "Fresh Produce Connection", "CargoContract", {"Class": "TemperatureSensitive", "Flow": "Transfer", "DangerousGoods": "false"}),
    ("DA_P5_Contract_OversizedOutbound", "Contract.Cargo.OversizedOutbound", "Oversized Pump Assembly", "CargoContract", {"Class": "Oversized", "Flow": "Outbound", "DangerousGoods": "false"}),
    ("DA_P5_Contract_BellyGeneral", "Contract.Cargo.BellyGeneral", "Regional Belly Freight", "CargoContract", {"Class": "General", "Flow": "Outbound", "BellyFreight": "true"}),
    ("DA_P5_Facility_Warehouse", "Facility.Cargo.Warehouse", "Riverbend Cargo House", "Facility", {"RoadConnected": "true", "AirsidePorts": "2"}),
    ("DA_P5_Warehouse_General", "Warehouse.General", "General Storage", "WarehouseZone", {"Compatibility": "General", "VolumeLitres": "60000"}),
    ("DA_P5_Warehouse_Express", "Warehouse.Express", "Express Staging", "WarehouseZone", {"Compatibility": "MailExpress", "VolumeLitres": "18000"}),
    ("DA_P5_Warehouse_Cold", "Warehouse.Cold", "Cold Store", "WarehouseZone", {"Compatibility": "TemperatureSensitive", "VolumeLitres": "12000"}),
    ("DA_P5_Warehouse_Oversized", "Warehouse.Oversized", "Oversized Pad", "WarehouseZone", {"Compatibility": "Oversized", "VolumeLitres": "90000"}),
    ("DA_P5_Facility_RoadDock", "Facility.Cargo.RoadDock", "Receiving and Shipping Docks", "Facility", {"RoadConnected": "true", "Directions": "Inbound,Outbound"}),
    ("DA_P5_Facility_BuildUp", "Facility.Cargo.BuildUp", "Build-up and Breakdown", "Facility", {"Directions": "Inbound,Outbound,Transfer"}),
    ("DA_P5_Facility_AirsidePort", "Facility.Cargo.AirsidePort", "Cargo Airside Ports", "Facility", {"Controlled": "true", "Ports": "2"}),
    ("DA_P5_Vehicle_Forklift", "Vehicle.Cargo.Forklift", "Cargo Forklift", "Vehicle", {"RuntimeDimension": "2D", "Capability": "CargoHandling"}),
    ("DA_P5_Vehicle_DollyTug", "Vehicle.Cargo.DollyTug", "Dolly Tug", "Vehicle", {"RuntimeDimension": "2D", "Capability": "CargoHandling"}),
    ("DA_P5_Vehicle_DeliveryTruck", "Vehicle.Cargo.DeliveryTruck", "Delivery Truck", "Vehicle", {"RuntimeDimension": "2D", "Capability": "CargoHandling"}),
    ("DA_P5_Vehicle_HighLoader", "Vehicle.Cargo.HighLoader", "Cargo High Loader", "Vehicle", {"RuntimeDimension": "2D", "Capability": "CargoHandling"}),
    ("DA_P5_Vehicle_SupportUnit", "Vehicle.Cargo.SupportUnit", "Cargo Support Unit", "Vehicle", {"RuntimeDimension": "2D", "Capability": "CargoHandling"}),
    ("DA_P5_Role_CargoHandling", "StaffRole.CargoHandling", "Cargo Handling Team", "StaffRole", {"Qualified": "true", "ShiftCoverage": "required"}),
    ("DA_P5_Tenant_FBO", "Tenant.FBO.Riverbend", "Riverbend Air Services", "ProviderTenant", {"Provider": "FBO", "GraceAndRecovery": "true"}),
    ("DA_P5_Tenant_Maintenance", "Tenant.Maintenance.Hearthside", "Hearthside Maintenance", "ProviderTenant", {"Provider": "Maintenance", "GraceAndRecovery": "true"}),
    ("DA_P5_Tenant_Fuel", "Tenant.Fuel.Meadow", "Meadow Fuel", "ProviderTenant", {"Provider": "Fuel", "GraceAndRecovery": "true"}),
    ("DA_P5_Tenant_Cargo", "Tenant.Cargo.MeadowFreight", "Meadow Freight", "ProviderTenant", {"Provider": "CargoOperator", "GraceAndRecovery": "true"}),
    ("DA_P5_Tenant_Food", "Tenant.Concession.SkyCafe", "Sky Café", "ConcessionTenant", {"Provider": "Food", "RevenueShare": "true", "GraceAndRecovery": "true"}),
    ("DA_P5_Tenant_Retail", "Tenant.Concession.RunwayGoods", "Runway Goods", "ConcessionTenant", {"Provider": "Retail", "RevenueShare": "true", "GraceAndRecovery": "true"}),
    ("DA_P5_Event_Airshow", "Event.Phase5.Airshow", "Riverbend Airshow", "SpecialEvent", {**event, "Family": "Airshow"}),
    ("DA_P5_Event_GAFlyIn", "Event.Phase5.GAFlyIn", "Valley GA Fly-in", "SpecialEvent", {**event, "Family": "GAFlyIn"}),
    ("DA_P5_Event_SchoolOpenDay", "Event.Phase5.FlightSchoolOpenDay", "Flight-school Open Day", "SpecialEvent", {**event, "Family": "FlightSchoolOpenDay"}),
    ("DA_P5_Event_CharterVIP", "Event.Phase5.CharterVip", "Charter VIP Movement", "SpecialEvent", {**event, "Family": "CharterVip"}),
    ("DA_P5_Event_CargoSurge", "Event.Phase5.CargoSurge", "Harvest Cargo Surge", "SpecialEvent", {**event, "Family": "CargoSurge"}),
    ("DA_P5_Event_HolidaySurge", "Event.Phase5.HolidayPassengerSurge", "Holiday Passenger Surge", "SpecialEvent", {**event, "Family": "HolidayPassengerSurge"}),
    ("DA_P5_Event_EmergencyExercise", "Event.Phase5.EmergencyExercise", "Emergency Exercise", "SpecialEvent", {**event, "Family": "EmergencyExercise"}),
    ("DA_P5_Event_NotableAircraft", "Event.Phase5.NotableAircraft", "Notable Visiting Aircraft", "SpecialEvent", {**event, "Family": "NotableVisitingAircraft"}),
    ("DA_P5_Balance", "Balance.Phase5.AdvancedBreadth", "Phase 5 Advanced Breadth", "BalanceProfile", {"AirportPoints": "50", "OperatingDays": "7", "Safety": "70", "Reliability": "70", "MajorLocked": "true"}),
    ("DA_P5_Scenario_Cargo", "Scenario.Phase5.CargoContinuity", "Cargo Continuity Fixture", "Scenario", {"Classes": "4", "Flows": "3", "DangerousGoods": "false"}),
    ("DA_P5_Scenario_Belly", "Scenario.Phase5.BellyFreight", "Belly Freight Interoperability", "Scenario", {"PassengerPrerequisiteForCargoAdvanced": "false"}),
    ("DA_P5_Scenario_Advanced", "Scenario.Phase5.SixAdvancedPaths", "Six Independent Advanced Paths", "Scenario", {"PathCount": "6", "UnrelatedFacilitiesAbsent": "true"}),
    ("DA_P5_Presentation_Cargo", "Presentation.Phase5.Cargo", "Cargo Operations Presentation", "PresentationDescriptor", {**presentation, "VisualReference": "VA-03,VA-04,VA-05"}),
    ("DA_P5_Presentation_Providers", "Presentation.Phase5.Providers", "Provider and Concession Presentation", "PresentationDescriptor", {**presentation, "VisualReference": "VA-01,VA-06"}),
    ("DA_P5_Presentation_Events", "Presentation.Phase5.Events", "Special Event Presentation", "PresentationDescriptor", {**presentation, "VisualReference": "VA-01,VA-06"}),
    ("DA_P5_Presentation_Progression", "Presentation.Phase5.Progression", "Advanced Capability Presentation", "PresentationDescriptor", {**presentation, "VisualReference": "VA-07"}),
]

for definition in definitions:
    create_data_asset(*definition)

unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
unreal.log(
    f"AMSim Phase 5 authored definition assets created: {len(definitions)}"
)
