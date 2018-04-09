Custom Properties
=================

## Scene Objects

* **export_path** _(string)_
  If this is defined, any maps or models are exported to export_path\export_name.export_fext.
  If not defined, **export_path** is the path of Blend file.
* **export_name** _(string)_
  If this is defined, any maps or models are exported to export_path\export_name.export_fext.
  If not defined, **export_name** is the name of the Blend file.
* **export_fext** _(string)_
  If this is defined, any maps or models are exported to export_path\export_name.export_fext.
  If not defined, **export_fext** is txt.

## Mesh Objects

* **entity_type** _(string enum)_
  If this is defined, **entity_type** must be one of the following values.
  - 'ROOM'
  - 'CELL'
  - 'PORTAL'
  - 'DOOR_CONTROLLER'
  - 'COLLISION_MESH'
  - 'CAMERA_ANIMATION'
  - 'CAMERA_MARKER'

## Axis Objects

 
## Texture Objects

path (string)
 Use this to replace Blender's default pathname conventions, which are quite ugly.
 For example, a texture named "image.dds", defined with path "models" will result
 in the script outputting "models/image.dds" as the name rather than the ugly
 output Blender puts out like "..//..//image.dds" for example.

type (string enum)
 'diffuse'  - diffuse map
 'normal'   - normal map
 'light'    - light map (ambient occlusion map)
 'specular' - specular map



 
