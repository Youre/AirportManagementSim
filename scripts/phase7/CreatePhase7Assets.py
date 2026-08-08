"""Create cooker-visible Phase 7 release definition assets."""

import unreal


DEFINITION_PATH = "/Game/Phase7/Definitions"
COMMON = {
    "RuntimeDimension": "2D",
    "Required3DAssets": "0",
    "RuntimeStringLoading": "false",
    "RightsStatus": "Reviewed",
    "OfflineRequired": "true",
}


def create_definition(name, stable_id, display_name, kind, attributes):
    object_path = f"{DEFINITION_PATH}/{name}.{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(object_path):
        asset = unreal.EditorAssetLibrary.load_asset(object_path)
    else:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property(
            "data_asset_class", unreal.AMSimPhase7Definition
        )
        asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            name,
            DEFINITION_PATH,
            unreal.AMSimPhase7Definition,
            factory,
        )
    if not asset:
        raise RuntimeError(f"Could not create {object_path}")
    values = {
        "stable_content_id": stable_id,
        "display_name": display_name,
        "localization_key": f"phase7.{stable_id.lower()}",
        "owning_phase": "Phase7",
        "provenance": "InternalApproved",
        "definition_kind": kind,
        "attributes": {**COMMON, **attributes},
    }
    for property_name, value in values.items():
        asset.set_editor_property(property_name, value)
    unreal.EditorAssetLibrary.save_loaded_asset(
        asset, only_if_is_dirty=False
    )


unreal.EditorAssetLibrary.make_directory(DEFINITION_PATH)

definitions = [
    (
        "DA_P7_ReleaseMap",
        "Release.Map.TemperateStarter",
        "Riverbend Plains Release Region",
        "ReleaseMap",
        {
            "MapId": "Map.TemperateStarter",
            "Level": "/Game/Maps/L_TemperateStarter",
            "SupportedSpecializations": "6",
            "StarterLoopValidated": "true",
        },
    ),
    (
        "DA_P7_AircraftOperatorCatalog",
        "Release.Catalog.AircraftOperators",
        "Approved Aircraft and Operator Catalog",
        "ReleaseCatalog",
        {
            "CatalogPhases": "1-6",
            "RealOperatorBranding": "false",
            "MilitaryCoreAircraft": "0",
            "UnapprovedAircraft": "0",
        },
    ),
    (
        "DA_P7_Guidance_Starter",
        "Guidance.Release.StarterJourney",
        "Starter Airfield Guidance",
        "Guidance",
        {"ContextCards": "6", "MaximumWordsPerCard": "45"},
    ),
    (
        "DA_P7_Guidance_Controls",
        "Guidance.Release.Controls",
        "Mouse, Keyboard, Camera, and Time",
        "Guidance",
        {"MouseAlwaysVisible": "true", "SemanticActions": "true"},
    ),
    (
        "DA_P7_Guidance_AirportTerms",
        "Guidance.Release.AirportTerms",
        "Airport Terms and Glossary",
        "Guidance",
        {"AuthenticTerms": "true", "PlainExplanations": "true"},
    ),
    (
        "DA_P7_Guidance_Operations",
        "Guidance.Release.Operations",
        "Airport Operations Handbook",
        "Guidance",
        {"AutomaticDispatch": "true", "RecoveryExplained": "true"},
    ),
    (
        "DA_P7_Accessibility",
        "Accessibility.Release.Profile",
        "Release Accessibility Profile",
        "Accessibility",
        {
            "SupportedScales": "100,125,150,175,200",
            "CaptionsDefault": "true",
            "ReducedMotionDefault": "true",
            "CameraShakeDefault": "false",
            "ColorOnlyStatus": "false",
        },
    ),
    (
        "DA_P7_Audio_RadioCoverage",
        "Audio.Release.RadioCoverage",
        "Offline Radio Phrase Coverage",
        "Audio",
        {
            "PhraseFamilies": "15",
            "Provider": "LocalFliteOrRadioCue",
            "NetworkSpeech": "false",
        },
    ),
    (
        "DA_P7_Audio_CaptionPolicy",
        "Audio.Release.CaptionPolicy",
        "Captions-first Operational Audio",
        "Audio",
        {
            "CaptionsDefault": "true",
            "CaptionsBeforeSpeech": "true",
            "SilentFallback": "true",
        },
    ),
    (
        "DA_P7_RightsInventory",
        "Rights.Release.ApprovedInventory",
        "Approved Release Rights Inventory",
        "RightsManifest",
        {
            "ReviewedPhases": "1-7",
            "UnreviewedRuntimeAssets": "0",
            "UnreviewedMusicIncluded": "false",
        },
    ),
    (
        "DA_P7_Scenario_Migration",
        "Scenario.Release.MigrationMatrix",
        "Schemas 1 through 9 Migration Matrix",
        "ReleaseScenario",
        {"MinimumSchema": "1", "CurrentSchema": "9"},
    ),
    (
        "DA_P7_Scenario_Offline",
        "Scenario.Release.NetworkDenied",
        "Network-denied Offline Journey",
        "ReleaseScenario",
        {"ScenarioId": "S15", "UnexpectedTcpAllowed": "0"},
    ),
    (
        "DA_P7_Protocol_Soak",
        "Protocol.Release.ReferenceSoak",
        "Four-hour Reference-tier Soak",
        "HumanProtocol",
        {"DurationHours": "4", "Status": "PendingOwnerTest"},
    ),
    (
        "DA_P7_Protocol_Unassisted",
        "Protocol.Release.UnassistedTester",
        "Unassisted Starter Journey",
        "HumanProtocol",
        {"GameplayHintsAllowed": "0", "Status": "PendingOwnerTest"},
    ),
    (
        "DA_P7_ReleaseCandidate",
        "Manifest.Release.Candidate",
        "Riverbend Release Candidate",
        "ReleaseManifest",
        {
            "EngineVersion": "5.8.0",
            "Platform": "Win64",
            "SaveSchema": "10",
            "MapCount": "1",
            "HumanGateStatus": "PendingOwnerTest",
            "ConceptReferences": "VA-01,VA-02,VA-03,VA-04,VA-05,VA-06,VA-07",
        },
    ),
]

for definition in definitions:
    create_definition(*definition)

unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
unreal.log(
    f"AMSim Phase 7 release definitions created: {len(definitions)}"
)
