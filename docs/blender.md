Custom Properties
=================

## Scene Objects

* **export_path** _(string)_
 If this is defined, any maps or models are exported to this folder pathname.
* **export_name** _(string)_
 If this is defined, any maps or models are exported to export_name.txt.

## Mesh Objects

entity_type (string enum)
 'CELL'
 'ROOM'
 'PORTAL'
 'DOOR_CONTROLLER'
 'COLLISION_MESH'
 'CAMERA_ANIMATION'
 'CAMERA_MARKER'

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



 
