import unreal


PACKAGE_PATH = "/Game/Phase1/Presentation/Materials"
ASSET_NAME = "M_ProceduralInfrastructure"


def get_or_create_material():
    asset_path = f"{PACKAGE_PATH}/{ASSET_NAME}"
    existing = (
        unreal.EditorAssetLibrary.load_asset(asset_path)
        if unreal.EditorAssetLibrary.does_asset_exist(asset_path)
        else None
    )
    if existing:
        return existing

    tools = unreal.AssetToolsHelpers.get_asset_tools()
    material = tools.create_asset(
        ASSET_NAME,
        PACKAGE_PATH,
        unreal.Material,
        unreal.MaterialFactoryNew(),
    )
    if not material:
        raise RuntimeError(f"Failed to create {asset_path}")
    return material


def configure_material(material):
    material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    material.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    material.set_editor_property("two_sided", True)
    try:
        material.set_editor_property("used_with_procedural_mesh", True)
    except Exception:
        unreal.log_warning("Procedural mesh usage flag is unavailable; usage will be checked at runtime.")

    library = unreal.MaterialEditingLibrary
    library.delete_all_material_expressions(material)
    vertex_color = library.create_material_expression(
        material,
        unreal.MaterialExpressionVertexColor,
        -320,
        0,
    )
    if not vertex_color:
        raise RuntimeError("Failed to create the vertex-color material expression")
    emissive_connected = any(
        library.connect_material_property(
            vertex_color,
            output_name,
            unreal.MaterialProperty.MP_EMISSIVE_COLOR,
        )
        for output_name in ("RGB", "Color", "")
    )
    if not emissive_connected:
        raise RuntimeError("Failed to connect vertex RGB to emissive color")
    opacity_connected = any(
        library.connect_material_property(
            vertex_color,
            output_name,
            unreal.MaterialProperty.MP_OPACITY,
        )
        for output_name in ("A", "Alpha")
    )
    if not opacity_connected:
        raise RuntimeError("Failed to connect vertex alpha to opacity")

    library.recompile_material(material)
    unreal.EditorAssetLibrary.save_loaded_asset(material, False)


material_asset = get_or_create_material()
configure_material(material_asset)
unreal.log(f"Configured procedural infrastructure material: {material_asset.get_path_name()}")
