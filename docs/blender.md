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

## Camera Objects

### Camera Animation (Axis Object)

* **start** _(uint16)_  
The camera marker index to start from. If not defined, this property is assumed to be zero.

### Camera Marker (Mesh Group)

Camera marker objects can either reference a Blender Mesh Group or a Blender Plain Axes object. You
can use any type of model for your mesh group, but it should be small model since cameras markers
do not need to be that noticable in your seen.

After all camera markers for a camera animation have been placed, parent the camera markers to the
camera animation object. Doing this not only allows you to fold the markers under the camera
animation object in the Outliner panel, but also specifies that these camera markers belong to this
particular animation object.

* **entity_type** _(string enum)_  
This property must be set to 'CAMERA_MARKER' and is a requirement. If not set, this entity will be
ignored.
* **index** _(uint16)_  
This property must be defined for every camera marker object.
* **speed** _(real32)_  
If defined, this property indicates 
* **interpolate_speed** _(bool)_  
If defined and set to False, camera speed is not interpolated between markers and the current speed
is always the speed of the last marker that the camera passed through. If defined and set to True,
or if the property is not defined, camera speed is interpolated between markers.
* **fovy** _(uint16)_  
The field of view of the viewing frustum along the y-axis at the time the camera hits the marker.
If not defined, **fovy** is assumed to be 60 degrees. If defined, **fovy** must be a value between
30 and 120.
* **interpolate_fovy** _(bool)_  
Boolean flag to indicate whether or not the **fovy** property is interpolated between camera
markers. The default value is True.
 
## Texture Objects

* **path** _(string)_  
Use this to replace Blender's default pathname conventions, which are quite ugly.
For example, a texture named "image.dds", defined with path "models" will result
in the script outputting "models/image.dds" as the name rather than the ugly
output Blender puts out like "..//..//image.dds" for example.

* **type** _(string enum)_  
If this is defined, **type** must be one of the following values.
  - 'diffuse'  - diffuse map
  - 'normal'   - normal map
  - 'light'    - light map (ambient occlusion map)
  - 'specular' - specular map



 
