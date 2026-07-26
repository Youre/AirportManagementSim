"""Create cooker-visible, internally authored Phase 4 definition assets."""

import unreal


DEFINITION_PATH = "/Game/Phase4/Definitions"


def create_data_asset(name, stable_id, display_name, kind, attributes):
    object_path = f"{DEFINITION_PATH}/{name}.{name}"
    asset = unreal.EditorAssetLibrary.load_asset(object_path)
    if not asset:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property(
            "data_asset_class", unreal.AMSimPhase4Definition
        )
        asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            name,
            DEFINITION_PATH,
            unreal.AMSimPhase4Definition,
            factory,
        )
    if not asset:
        raise RuntimeError(f"Could not create {object_path}")
    properties = {
        "stable_content_id": stable_id,
        "display_name": display_name,
        "localization_key": f"phase4.{stable_id.lower()}",
        "owning_phase": "Phase4",
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

definitions = [
    ("DA_P4_Operator_Riverbend", "Operator.RiverbendConnect.Phase4", "Riverbend Connect", "Operator", {"Fictional": "true", "Recurring": "true"}),
    ("DA_P4_Operator_Northstar", "Operator.NorthstarRegional", "Northstar Regional", "Operator", {"Fictional": "true", "Recurring": "true"}),
    ("DA_P4_Operator_Coastal", "Operator.CoastalAirways", "Coastal Airways", "Operator", {"Fictional": "true", "Recurring": "true", "International": "true"}),
    ("DA_P4_Aircraft_Turboprop", "Aircraft.RegionalTurboprop.Phase4", "Trailwing 42", "Aircraft", {"Fictional": "true", "ExternalSourceFiles": "0", "RuntimeDimension": "2D", "ApproachClass": "Regional"}),
    ("DA_P4_Aircraft_RegionalJet", "Aircraft.RegionalJet.Phase4", "Skylark 72", "Aircraft", {"Fictional": "true", "ExternalSourceFiles": "0", "RuntimeDimension": "2D", "ApproachClass": "RegionalJet"}),
    ("DA_P4_Aircraft_NarrowBody", "Aircraft.NarrowBody.Phase4", "Harborliner 126", "Aircraft", {"Fictional": "true", "ExternalSourceFiles": "0", "RuntimeDimension": "2D", "ApproachClass": "NarrowBody"}),
    ("DA_P4_Contract_Riverbend", "Contract.Phase4.RiverbendConnect", "Riverbend Connect Daily", "RecurringContract", {"FrequencyPerWeek": "7", "Operator": "RiverbendConnect"}),
    ("DA_P4_Contract_Northstar", "Contract.Phase4.NorthstarRegional", "Northstar Regional Daily", "RecurringContract", {"FrequencyPerWeek": "7", "Operator": "NorthstarRegional"}),
    ("DA_P4_Contract_Coastal", "Contract.Phase4.CoastalAirways", "Coastal Airways Daily", "RecurringContract", {"FrequencyPerWeek": "7", "Operator": "CoastalAirways"}),
    ("DA_P4_Gate_A1", "Facility.Gate.A1.Contact", "Gate A1", "Gate", {"BoardingMode": "Contact", "SizeClass": "Regional"}),
    ("DA_P4_Gate_A2", "Facility.Gate.A2.ContactInternational", "Gate A2 International", "Gate", {"BoardingMode": "Contact", "SizeClass": "NarrowBody", "BorderRoute": "true"}),
    ("DA_P4_Stand_R1", "Facility.Stand.R1.Remote", "Remote Stand R1", "Stand", {"BoardingMode": "RemoteBus", "SizeClass": "Regional"}),
    ("DA_P4_RemoteBus", "Vehicle.Airside.RemoteBus", "Remote Gate Bus", "Vehicle", {"RuntimeDimension": "2D", "Capacity": "48"}),
    ("DA_P4_RemoteBusRoute", "Network.Passenger.RemoteBus", "Remote Bus Route", "Network", {"Controlled": "true", "Accessible": "true"}),
    ("DA_P4_TransferNetwork", "Network.Baggage.Transfer", "Transfer Baggage Network", "Network", {"ScreeningStatePreserved": "true", "Backpressure": "true"}),
    ("DA_P4_TransferSort", "Facility.Baggage.TransferSort", "Transfer Sort", "Facility", {"Capacity": "48", "Controlled": "true"}),
    ("DA_P4_Immigration", "Facility.Border.Immigration", "Passport Control", "Facility", {"FictionalDocuments": "true", "Accessible": "true"}),
    ("DA_P4_Customs", "Facility.Border.Customs", "Customs", "Facility", {"InspectionAbstract": "true", "Accessible": "true"}),
    ("DA_P4_InternationalRoute", "Network.Controlled.International", "International Controlled Route", "Network", {"Controlled": "true", "ProtectedTraitEffects": "false", "Accessible": "true"}),
    ("DA_P4_RentalCar", "Facility.Landside.RentalCar", "Rental Car Centre", "Facility", {"Capacity": "80", "RouteRequired": "true"}),
    ("DA_P4_RentalTenant", "Tenant.RiverbendRentals", "Riverbend Rentals", "Tenant", {"Fictional": "true", "RecoveryOffer": "true"}),
    ("DA_P4_Rail", "Facility.Landside.RailStation", "Valley Rail Station", "Facility", {"Capacity": "160", "FrequencyMinutes": "15"}),
    ("DA_P4_RailTenant", "Tenant.ValleyRail", "Valley Rail", "Tenant", {"Fictional": "true", "RecoveryOffer": "true"}),
    ("DA_P4_Weather", "Weather.Phase4.SevenDayRegional", "Seven-day Regional Forecast", "WeatherProfile", {"Seeded": "true", "Season": "Autumn", "NearTermHours": "6"}),
    ("DA_P4_Approach", "Approach.Phase4.Runway27", "Runway 27 Approach Limits", "ApproachProfile", {"AircraftSpecific": "true", "DisclosedThresholds": "true"}),
    ("DA_P4_Incident", "Incident.Phase4.DisabledAircraft", "Disabled Aircraft Response", "Incident", {"ChildAppropriate": "true", "HumanOutcome": "NoInjuries", "WarningProvenance": "required", "AffectedOnlyClosure": "true"}),
    ("DA_P4_TowTeam", "Vehicle.Emergency.TowTeam", "Airport Tow Team", "EmergencyVehicle", {"RuntimeDimension": "2D", "Qualified": "true"}),
    ("DA_P4_Report", "Report.Phase4.Incident", "Incident Cause Report", "Report", {"Cause": "required", "Warnings": "required", "Prevention": "required"}),
    ("DA_P4_Renewal", "Renewal.Phase4.RegionalOperators", "Regional Operator Renewals", "RenewalProfile", {"RecoveryOffer": "true", "NoFailureSpiral": "true"}),
    ("DA_P4_Balance", "Balance.Phase4.RegionalWeek", "Phase 4 Regional Week Balance", "BalanceProfile", {"Days": "7", "FlightsPerDay": "3", "LockedHorizonMinutes": "30", "TimetableIncrementMinutes": "5"}),
    ("DA_P4_Scenario_S06", "Scenario.S06.PassengerConnectionTransfer", "S06 Passenger Connection and Baggage Transfer", "Scenario", {"Passengers": "24", "Missed": "6", "Bags": "32", "ExactReconciliation": "true"}),
    ("DA_P4_Scenario_S10", "Scenario.S10.WarnedIncidentRecovery", "S10 Warned Incident and Recovery", "Scenario", {"WarningProvenance": "required", "Outcome": "NoInjuries", "Recoverable": "true"}),
    ("DA_P4_Presentation_Timetable", "Presentation.Phase4.Timetable", "Regional Timetable Presentation", "PresentationDescriptor", {"Dimension": "2D", "Required3DAssets": "0", "RuntimeStringLoading": "false", "VisualReference": "VA-04"}),
    ("DA_P4_Presentation_Incident", "Presentation.Phase4.Incident", "Weather and Incident Presentation", "PresentationDescriptor", {"Dimension": "2D", "Required3DAssets": "0", "RuntimeStringLoading": "false", "VisualReference": "VA-06"}),
]

for definition in definitions:
    create_data_asset(*definition)

unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
unreal.log("AMSim Phase 4 authored definition assets created successfully.")
