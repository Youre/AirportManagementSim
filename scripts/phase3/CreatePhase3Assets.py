"""Create cooker-visible, internally authored Phase 3 definition assets."""

import unreal


DEFINITION_PATH = "/Game/Phase3/Definitions"


def create_data_asset(name, stable_id, display_name, kind, attributes):
    object_path = f"{DEFINITION_PATH}/{name}.{name}"
    asset = unreal.EditorAssetLibrary.load_asset(object_path)
    if not asset:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property(
            "data_asset_class", unreal.AMSimPhase3Definition
        )
        asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            name,
            DEFINITION_PATH,
            unreal.AMSimPhase3Definition,
            factory,
        )
    if not asset:
        raise RuntimeError(f"Could not create {object_path}")
    properties = {
        "stable_content_id": stable_id,
        "display_name": display_name,
        "localization_key": f"phase3.{stable_id.lower()}",
        "owning_phase": "Phase3",
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
    ("DA_P3_TerminalShell", "Facility.Terminal.Shell", "Domestic Terminal Shell", "Facility", {"GridMeters": "1", "Floors": "1"}),
    ("DA_P3_Entrance", "Facility.Terminal.Entrance", "Terminal Entrance", "Facility", {"Zone": "Landside", "Accessible": "true"}),
    ("DA_P3_CheckIn", "Facility.Terminal.CheckIn", "Check-in & Bag Drop", "Facility", {"Zone": "Landside", "Capacity": "48"}),
    ("DA_P3_Security", "Facility.Terminal.Security", "Domestic Security", "Facility", {"ZoneTransition": "Landside,SterileDepartures", "ChildAppropriate": "true", "AccessibleLane": "true"}),
    ("DA_P3_DepartureLounge", "Facility.Terminal.DepartureLounge", "Departure Lounge", "Facility", {"Zone": "SterileDepartures", "Capacity": "90"}),
    ("DA_P3_GateA1", "Facility.Terminal.GateA1", "Gate A1", "Facility", {"Zone": "SterileDepartures", "Capacity": "60"}),
    ("DA_P3_Arrivals", "Facility.Terminal.Arrivals", "Arrivals Corridor", "Facility", {"Zone": "SterileArrivals", "OneWay": "true"}),
    ("DA_P3_BagMakeUp", "Facility.Terminal.BagMakeUp", "Baggage Make-up", "Facility", {"Zone": "RestrictedBaggage", "Capacity": "36"}),
    ("DA_P3_Reclaim", "Facility.Terminal.Reclaim", "Baggage Reclaim", "Facility", {"Zone": "Landside", "Capacity": "60"}),
    ("DA_P3_Curb", "Facility.Landside.Curb", "Taxi & Pick-up Curb", "Facility", {"Mode": "Taxi", "Capacity": "36"}),
    ("DA_P3_Parking", "Facility.Landside.Parking", "Passenger Parking", "Facility", {"Mode": "PrivateCar", "Capacity": "70"}),
    ("DA_P3_BusStop", "Facility.Landside.BusStop", "Riverbend Bus Stop", "Facility", {"Mode": "PublicBus", "Capacity": "48"}),
    ("DA_P3_RoadNetwork", "Network.Landside.RoadCurbParkingBus", "Landside Road Network", "Network", {"Direction": "ExternalToCurb", "Accessible": "true"}),
    ("DA_P3_EntranceRoute", "Network.Pedestrian.EntranceCheckIn", "Entrance to Check-in", "Network", {"Direction": "EntranceToCheckIn", "Accessible": "true"}),
    ("DA_P3_SecurityRoute", "Network.Controlled.DomesticSecurity", "Domestic Security Transition", "Network", {"Direction": "LandsideToSterile", "Controlled": "true", "Accessible": "true"}),
    ("DA_P3_GateRoute", "Network.Pedestrian.SterileGateA1", "Sterile Route to Gate A1", "Network", {"Direction": "SecurityToGate", "Accessible": "true"}),
    ("DA_P3_ArrivalsRoute", "Network.Controlled.ArrivalsReclaimCurb", "Arrivals to Curb", "Network", {"Direction": "AircraftToCurb", "Controlled": "true", "Accessible": "true"}),
    ("DA_P3_BaggageOutbound", "Network.Baggage.Outbound", "Outbound Baggage System", "Network", {"Direction": "BagDropToAircraft", "ScreeningRequired": "true"}),
    ("DA_P3_BaggageArrival", "Network.Baggage.Arrival", "Arrival Baggage System", "Network", {"Direction": "AircraftToReclaim", "Controlled": "true"}),
    ("DA_P3_PassengerService", "StaffRole.PassengerService", "Passenger Service Team", "StaffRole", {"TeamSize": "3", "Zone": "Landside"}),
    ("DA_P3_SecurityTeam", "StaffRole.Security", "Security Team", "StaffRole", {"TeamSize": "4", "Zone": "Checkpoint"}),
    ("DA_P3_BaggageTeam", "StaffRole.Baggage", "Baggage Team", "StaffRole", {"TeamSize": "4", "Zone": "Baggage"}),
    ("DA_P3_TerminalOps", "StaffRole.TerminalOperations", "Terminal Operations Team", "StaffRole", {"TeamSize": "2", "Zone": "Terminal"}),
    ("DA_P3_Tenant", "Tenant.RiverbendConnect", "Riverbend Connect", "Tenant", {"Fictional": "true", "Specialization": "Passenger"}),
    ("DA_P3_Operator", "Operator.RiverbendConnect", "Riverbend Connect", "Operator", {"Fictional": "true", "RealBranding": "false"}),
    ("DA_P3_Contract", "Contract.Passenger.DomesticStarter", "Riverbend Domestic Starter", "Contract", {"Gate": "A1", "DeparturePassengers": "28", "ArrivalPassengers": "24"}),
    ("DA_P3_NameSet", "PassengerNameSet.Riverbend", "Riverbend Passenger Names", "PassengerNames", {"GeneratedLocally": "true", "ProtectedTraitEffects": "false"}),
    ("DA_P3_Aircraft", "Aircraft.RegionalPassenger.Phase3", "Riverbend Regional Passenger Aircraft", "Aircraft", {"Fictional": "true", "ExternalSourceFiles": "0", "RuntimeDimension": "2D", "PresentationReuse": "ReviewedFictionalSilhouette"}),
    ("DA_P3_PrivateCar", "Vehicle.Landside.PrivateCar", "Passenger Car", "LandsideVehicle", {"RuntimeDimension": "2D", "Mode": "PrivateCar"}),
    ("DA_P3_Taxi", "Vehicle.Landside.Taxi", "Riverbend Taxi", "LandsideVehicle", {"RuntimeDimension": "2D", "Mode": "Taxi"}),
    ("DA_P3_Bus", "Vehicle.Landside.PublicBus", "Riverbend Public Bus", "LandsideVehicle", {"RuntimeDimension": "2D", "Mode": "PublicBus"}),
    ("DA_P3_Balance", "Balance.Phase3.Terminal", "Phase 3 Terminal Balance", "BalanceProfile", {"TerminalCostCredits": "1200", "JourneyBucketMilliseconds": "5000"}),
    ("DA_P3_Scenario", "Scenario.S05.PassengerDepartureArrival", "S05 Passenger Departure and Arrival", "Scenario", {"ExactReconciliation": "true", "SecurityBypassAllowed": "false", "AccessibleRouteRequired": "true"}),
    ("DA_P3_Presentation", "Presentation.TerminalFlow", "Terminal Flow 2D Presentation", "PresentationDescriptor", {"Dimension": "2D", "Required3DAssets": "0", "VisiblePassengerProxyTarget": "2000", "LogicalPassengerTarget": "10000", "VisualReference": "VA-03", "RuntimeStringLoading": "false"}),
]

for definition in definitions:
    create_data_asset(*definition)

unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
unreal.log("AMSim Phase 3 authored definition assets created successfully.")
