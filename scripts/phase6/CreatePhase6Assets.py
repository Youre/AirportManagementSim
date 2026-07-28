"""Create cooker-visible Phase 6 definition assets.

The Boeing 787-9 definition records the approved external reference and the
project-owned Riverbend Longreach River & Sun runtime treatment.
"""

import unreal


DEFINITION_PATH = "/Game/Phase6/Definitions"
PRESENTATION = {
    "RuntimeDimension": "2D",
    "Required3DAssets": "0",
    "RuntimeStringLoading": "false",
    "WorldAreaPercent": "60",
    "CompactAtScale": "1.75",
}


def create_data_asset(
    name,
    stable_id,
    display_name,
    kind,
    attributes,
    provenance="InternalApproved",
    source="ProjectAuthored",
):
    object_path = f"{DEFINITION_PATH}/{name}.{name}"
    asset = None
    if unreal.EditorAssetLibrary.does_asset_exist(object_path):
        asset = unreal.EditorAssetLibrary.load_asset(object_path)
    if not asset:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property(
            "data_asset_class", unreal.AMSimPhase6Definition
        )
        asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            name,
            DEFINITION_PATH,
            unreal.AMSimPhase6Definition,
            factory,
        )
    if not asset:
        raise RuntimeError(f"Could not create {object_path}")
    properties = {
        "stable_content_id": stable_id,
        "display_name": display_name,
        "localization_key": f"phase6.{stable_id.lower()}",
        "owning_phase": "Phase6",
        "provenance": provenance,
        "source": source,
        "definition_kind": kind,
        "attributes": {**PRESENTATION, **attributes},
    }
    for property_name, value in properties.items():
        asset.set_editor_property(property_name, value)
    unreal.EditorAssetLibrary.save_loaded_asset(
        asset, only_if_is_dirty=False
    )


unreal.EditorAssetLibrary.make_directory(DEFINITION_PATH)

facility_definitions = [
    ("Runway_Parallel", "Facility.Major.Runway.Parallel", "Parallel Runway 09R/27L", "ParallelRunway", "12000", "1"),
    ("Stand_Large", "Facility.Major.Stand.Large", "Stand H1", "LargeStand", "6500", "1"),
    ("GA_HangarCampus", "Facility.Major.GA.HangarCampus", "Riverbend Hangar Campus", "HangarCampus", "4800", "8"),
    ("School_InstructionCenter", "Facility.Major.FlightSchool.InstructionCenter", "Riverbend Training Center", "InstructionCenter", "4200", "4"),
    ("Charter_Executive", "Facility.Major.Charter.Executive", "Riverbend Executive", "ExecutiveFacility", "5200", "2"),
    ("Cargo_Hub", "Facility.Major.Cargo.Hub", "Riverbend Cargo Hub", "CargoHub", "8000", "48"),
    ("Passenger_Concourse", "Facility.Major.Passenger.Concourse", "Riverbend Concourse B", "TerminalConcourse", "9500", "600"),
    ("Baggage_Hall", "Facility.Major.Baggage.Hall", "Baggage Sort Hall B", "BaggageHall", "7000", "600"),
    ("Service_Depot", "Facility.Major.Service.Depot", "Heavy Service Depot", "ServiceDepot", "6000", "12"),
    ("Emergency_Station", "Facility.Major.Emergency.Station", "Airport Rescue Station", "EmergencyStation", "6800", "4"),
    ("Access_Hub", "Facility.Major.Access.Hub", "Riverbend Transit Hub", "GroundAccessHub", "7200", "800"),
    ("Operations_Center", "Facility.Major.Operations.Center", "Airport Operations Center", "OperationsCenter", "5500", "6"),
]

definitions = [
    (
        "DA_P6_Aircraft_Boeing787_9",
        "Aircraft.Boeing787-9.Phase6",
        "Boeing 787-9 — Riverbend Longreach",
        "Aircraft",
        {
            "FictionalLivery": "true",
            "RealOperator": "false",
            "CandidateApproval": "OwnerApprovedRiverAndSun",
            "CandidateSourcePath": r"C:\Users\dave\Documents\Joes_Game\dist\assets\Boeing_787-9-B24vQXZX.png",
            "CandidateChecksum": "6AA56CD2CDD7035E3EE3526CFF5A603751E03441DA11ECE6C7E6E550B9B04449",
            "CandidateDimensions": "719x776 RGBA",
            "RuntimeTextureImported": "true",
            "RuntimeSpriteReferences": "16",
            "EditableVectorMaster": "Riverbend_Longreach_787-9.svg",
            "HeadingVariants": "16",
            "PixelsPerMeter": "32",
            "LiveryTreatment": "RiverAndSun",
            "Operator": "Riverbend Longreach",
        },
        "OwnerApprovedDerivedAsset",
        "ApprovedExternalReferenceAndProjectAuthoredVector",
    ),
    (
        "DA_P6_Operator_Longreach",
        "Operator.RiverbendLongreach",
        "Riverbend Longreach",
        "PassengerOperator",
        {"Fictional": "true", "RealOperator": "false"},
    ),
    (
        "DA_P6_Runway_ParallelConfiguration",
        "Runway.Major.ParallelConfiguration",
        "Parallel Runway Configuration",
        "RunwayConfiguration",
        {
            "Primary": "09L/27R",
            "Parallel": "09R/27L",
            "ReciprocalSideSwap": "true",
            "IndependentUse": "true",
        },
    ),
    (
        "DA_P6_Operation_Boeing787_9",
        "Operation.Major.Boeing787-9",
        "RL 602 Wide-body Turnaround",
        "LargeAircraftOperation",
        {
            "Aircraft": "Aircraft.Boeing787-9.Phase6",
            "PassengerCapacity": "290",
            "RepresentativePassengers": "278",
            "RepresentativeBags": "334",
            "OrdinaryDispatch": "true",
        },
    ),
    (
        "DA_P6_Incident_AircraftLoss",
        "Incident.Phase6.AircraftLoss",
        "Warned Aircraft-loss Incident",
        "SeriousIncident",
        {
            "WarningRequired": "true",
            "PlayerConfirmationRequired": "true",
            "AbstractHumanOutcomes": "true",
            "LocalizedClosure": "true",
            "BankruptcyForbidden": "true",
        },
    ),
    (
        "DA_P6_Project_IncidentRepair",
        "Project.Phase6.IncidentRepair",
        "Bounded Runway Repair",
        "RepairProject",
        {
            "CostCredits": "5000",
            "RecoverySupportCredits": "7000",
            "RepairMilliseconds": "60000",
        },
    ),
    (
        "DA_P6_Balance",
        "Balance.Phase6.MajorCapability",
        "Phase 6 Major Capability",
        "BalanceProfile",
        {
            "AirportPoints": "100",
            "OperatingDays": "14",
            "Safety": "80",
            "Reliability": "80",
            "TenantRelationships": "75",
            "EarnedBandPermanent": "true",
            "OperationalBandRecoverable": "true",
        },
    ),
    (
        "DA_P6_Scenario_MajorPaths",
        "Scenario.Phase6.SixMajorPaths",
        "Six Independent Major Paths",
        "Scenario",
        {"PathCount": "6", "AcceleratedPreparedJourneys": "true"},
    ),
    (
        "DA_P6_Scenario_LargeAircraft",
        "Scenario.Phase6.LargeAircraftTurnaround",
        "Large-aircraft Turnaround",
        "Scenario",
        {"OrdinaryDispatch": "true", "ReconciliationRequired": "true"},
    ),
    (
        "DA_P6_Scenario_Incident",
        "Scenario.Phase6.SeriousIncidentRecovery",
        "Serious Incident and Recovery",
        "Scenario",
        {"MidIncidentSaveLoad": "true", "OtherOperationsContinue": "true"},
    ),
    (
        "DA_P6_Scenario_MaxScale",
        "Scenario.Phase6.MaximumScale",
        "Maximum-scale Airport",
        "Scenario",
        {
            "LogicalAgents": "10000",
            "VisibleAgents": "2000",
            "Aircraft": "150",
            "Vehicles": "500",
        },
    ),
    (
        "DA_P6_Presentation_Runways",
        "Presentation.Phase6.Runways",
        "Parallel Runway Presentation",
        "PresentationDescriptor",
        {**PRESENTATION, "VisualReference": "VA-01,VA-06"},
    ),
    (
        "DA_P6_Presentation_Capacity",
        "Presentation.Phase6.MajorCapacity",
        "Major Capacity Presentation",
        "PresentationDescriptor",
        {**PRESENTATION, "VisualReference": "VA-01,VA-03,VA-04"},
    ),
    (
        "DA_P6_Presentation_LargeAircraft",
        "Presentation.Phase6.LargeAircraft",
        "Large-aircraft Turnaround Presentation",
        "PresentationDescriptor",
        {**PRESENTATION, "VisualReference": "VA-05"},
    ),
    (
        "DA_P6_Presentation_Incident",
        "Presentation.Phase6.Incident",
        "Serious Incident Presentation",
        "PresentationDescriptor",
        {**PRESENTATION, "VisualReference": "VA-06"},
    ),
    (
        "DA_P6_Presentation_Progression",
        "Presentation.Phase6.Progression",
        "Major Capability Presentation",
        "PresentationDescriptor",
        {**PRESENTATION, "VisualReference": "VA-07"},
    ),
]

for (
    suffix,
    stable_id,
    display_name,
    facility_type,
    cost,
    capacity,
) in facility_definitions:
    definitions.append(
        (
            f"DA_P6_Facility_{suffix}",
            stable_id,
            display_name,
            "Facility",
            {
                "FacilityType": facility_type,
                "CostCredits": cost,
                "CapacityUnits": capacity,
                "CookRule": "AlwaysCook",
            },
        )
    )

for definition in definitions:
    create_data_asset(*definition)

unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
unreal.log(
    f"AMSim Phase 6 authored definition assets created: {len(definitions)}"
)
