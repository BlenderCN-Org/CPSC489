#######################################################################
#
# CPSC489 Game Development Project
# Copyright (c) 2018  Steven Emory <mobysteve@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#######################################################################

import bpy
import mathutils
import os
import re

#
# CLASSES + STRUCTURES
#
class Bone:
    # self.name
    # self.parent
    # self.position[3]
    # self.matrix[3][3]
    pass
class ArmatureData:
    # self.n_bones    - int
    # self.bonelist[] - Bone[]
    pass
class AnimationKeyframe:
    # self.position - vector3
    # self.rotation - vector4
    # self.scale    - vector3
    pass
class AnimationData:
    # self.name      - string
    # self.n_keyable - uint
    # self.bonemap   - dictionary<String, AnimationKeyFrame>}
    pass
class Material:
    # self.name     string
    # self.textures MaterialTexture[]
    pass
class MaterialTexture:
    # self.name
    # self.type
    # self.channel
    # self.filename
    pass
class BoundingBox:
    # self.position vector4
    # self.rotation vector4
    # self.halfdims vector4
    pass
    
#
# FILENAME AND PATHNAME FUNCTIONS
#
def GetFileFullPath(): return bpy.data.filepath
def GetFilePathName(): return os.path.splitext(bpy.data.filepath)[0]
def GetFilePathSplit(): return os.path.split(bpy.data.filepath) 
def GetFilePathSplitExt(): return os.path.splitext(bpy.data.filepath) 
def GetPythonPath(): return bpy.app.binary_path_python

#
# FILE FUNCTIONS
#
def WriteVector3(file, v): file.write('{} {} {}\n'.format(v[0], v[1], v[2]))
def WriteOffsetVector3(file, v, dv): file.write('{} {} {}\n'.format(v[0]+dv[0], v[1]+dv[1], v[2]+dv[2]))
def WriteVector4(file, v): file.write('{} {} {} {}\n'.format(v[0], v[1], v[2], v[3]))
def WriteOffsetVector4(file, v, dv): file.write('{} {} {} {}\n'.format(v[0]+dv[0], v[1]+dv[1], v[2]+dv[2], v[3]+dv[3]))

#
# MODE FUNCTIONS
#
def GetMode(self): return bpy.context.mode
def InObjectMode(self): return (bpy.context.mode == 'OBJECT')
def InEditArmatureMode(self): return (bpy.context.mode == 'EDIT_ARMATURE')
def InEditMeshMode(self): return (bpy.context.mode == 'EDIT_MESH')
def InPoseMode(self): return (bpy.context.mode == 'POSE')

#
# OBJECT FUNCTIONS
#
def GetObjects(): return bpy.data.objects
def GetSelectedObjects():
    rv = []
    for obj in bpy.data.objects:
        if obj.select: rv.append(obj)
    return rv
def IsArmatureObject(obj): return (True if obj.type == 'ARMATURE' else False)
def IsMeshObject(obj): return (True if obj.type == 'MESH' else False)
def GetArmatureObjects():
    r = []
    for obj in GetObjects():
        if IsArmatureObject(obj): r.append(obj)
    return r
def GetSelectedArmatureObjects():
    r = []
    for obj in GetObjects():
        if (IsArmatureObject(obj) and obj.select): r.append(obj)
    return r
def GetMeshObjects():
    rv = []
    for obj in GetObjects():
        if IsMeshObject(obj) and IsMeshDataValid(obj.data): rv.append(obj)
    return rv
def GetSelectedMeshObjects():
    rv = []
    for obj in GetObjects():
        if IsMeshObject(obj) and IsMeshDataValid(obj.data) and obj.select: rv.append(obj)
    return rv

#
# DATA FUNCTIONS
#
def GetActions(): return bpy.data.actions
def GetArmatures(): return bpy.data.armatures
def GetMaterials(): return bpy.data.materials
def GetMeshes(): return bpy.data.meshes
def IsArmatureDataValid(data):
    if (data is None) or (data.bones is None): return False
    return len(data.bones) > 0
def IsMeshDataValid(data):
    if (data is None) or (data.vertices is None): return False
    return len(data.vertices) > 0
def GetObjectFromArmatureData(data):
	if data is None: return None
	for obj in GetObjects():
		if obj.data.name == data.name: return obj
	return None
def GetObjectFromMeshData(data):
	if data is None: return None
	for obj in GetObjects():
		if obj.data.name == data.name: return obj
	return None
	
#
# ARMATURE FUNCTIONS
#
def HasBones(armature):
	return ((armature is not None) and (armature.data is not None) and (len(armature.data.bones) > 0))
def CreateIndexBoneMap(armature):
	if HasBones(armature) == False: return {}
	bonemap = {}
	for index, bone in enumerate(armature.data.bones): bonemap[bone.name] = index
	return bonemap
def GetArmatureObjectByName(name):
	if name is None or len(name) == 0: return None
	list = GetArmatureObjects()
	for obj in list:
		if obj.name == name: return obj
	return None
def GetArmatureDataByName(name):
	if name is None or len(name) == 0: return None
	list = GetArmatureObjects()
	for obj in list:
		if obj.data.name == name: return obj.data
	return None
def GetArmatureObjectFromMesh(mesh):
	if mesh is None: return None
	# 1st - parent armature
	obj = GetObjectFromMeshData(mesh)
	if (obj.parent is not None):
		if (obj.parent.type == 'ARMATURE'): return obj.parent
	# 2nd - modifier armature
	for modifier in obj.modifiers:
		if modifier.type == 'ARMATURE':
			return GetArmatureObjectByName(modifier.object.name)
	# 3rd - first armature you find
	list = GetArmatureObjects()
	if (list is not None) and len(list): return list[0]
	return None
def ConstructArmatureData(armature):
	if armature is None or armature.data is None: raise Exception('Invalid argument.')
	# save number of bones    
	data = ArmatureData()
	data.n_bones = len(armature.data.bones)
	data.bonelist = []
	if data.n_bones == 0: return data
	# save bones
	bmap = CreateIndexBoneMap(armature)
	for index, bone in enumerate(armature.data.bones):
		data.bonelist.append(Bone())
		data.bonelist[index].name = bone.name
		data.bonelist[index].parent = -1
		if bone.parent != None: data.bonelist[index].parent = bmap[bone.parent.name]
		data.bonelist[index].position = [
			bone.head_local[0],
			bone.head_local[1],
			bone.head_local[2]]
		data.bonelist[index].matrix = [
			[bone.matrix_local[0][0], bone.matrix_local[0][1], bone.matrix_local[0][2]],
			[bone.matrix_local[1][0], bone.matrix_local[1][1], bone.matrix_local[1][2]],
			[bone.matrix_local[2][0], bone.matrix_local[2][1], bone.matrix_local[2][2]]]
	return data
def ConstructArmatureDataFromPose(armature):
    if armature is None: raise Exception('Invalid argument.')
    if armature.data is None: raise Exception('Invalid argument.')
    if armature.pose is None: raise Exception('Invalid argument.')
    # save number of bones    
    data = ArmatureData()
    data.n_bones = len(armature.pose.bones)
    data.bonelist = []
    if data.n_bones == 0: return data
    # save bones
    bmap = CreateIndexBoneMap(armature)
    for index, bone in enumerate(armature.pose.bones):
        data.bonelist.append(Bone())
        data.bonelist[index].name = bone.name
        data.bonelist[index].parent = -1
        if bone.parent != None: data.bonelist[index].parent = bmap[bone.parent.name]
        m = armature.matrix_world * bone.matrix
        data.bonelist[index].position = [ m[0][3], m[1][3], m[2][3] ]
        data.bonelist[index].matrix = [
            [m[0][0], m[0][1], m[0][2], m[0][3]],
            [m[1][0], m[1][1], m[1][2], m[1][3]],
            [m[2][0], m[2][1], m[2][2], m[2][3]],
            [m[3][0], m[3][1], m[3][2], m[3][3]]]
    return data

#
# ANIMATION FUNCTIONS
#
def ConstructAnimationData(armature):
	if (armature is None) or (armature.data is None): raise Exception('Invalid argument.')
	rv = []
	for action in GetActions(): rv.append(GetAnimationData(action, armature))
	return rv
def GetAnimationData(action, armature):
    # valid arguments
    if action is None: raise Exception('Invalid argument.')
    if (armature is None) or (armature.data is None): raise Exception('Invalid argument.')
    # map to associate bone with key
    bonemap = {}
    for bone in armature.data.bones: bonemap[bone.name] = {}
    # for each curve
    for fcu in action.fcurves:
        # extract bone name and key type
        pattern = r'pose\.bones\[\"(.*)\"\]\.(.*)'
        str = fcu.data_path
        m = re.match(pattern, str)
        if m is None: continue
        g1 = m.group(1)
        g2 = m.group(2)
        # extract keyframes
        for keyframe in fcu.keyframe_points:
            # frame/value pair
            frame, value = keyframe.co
            # lookup key dictionary for bone
            keydict = bonemap[g1]
            if keydict == None: raise Exception('Bone name lookup failed.')
            # create default values
            if frame not in keydict:
                item = AnimationKeyframe()
                item.position = [0.0, 0.0, 0.0]
                item.rotation = [1.0, 0.0, 0.0, 0.0]
                item.scale    = [1.0, 1.0, 1.0]
                keydict[frame] = item
            # assign values
            if g2 == 'location':
                if (fcu.array_index >= 0 and fcu.array_index <= 2):
                    keydict[frame].position[fcu.array_index] = value
            if g2 == 'rotation_euler':
                pass
            if g2 == 'rotation_quaternion':
                if (fcu.array_index >= 0 and fcu.array_index <= 4):
                    keydict[frame].rotation[fcu.array_index] = value
            if g2 == 'scale':
                if (fcu.array_index >= 0 and fcu.array_index <= 2):
                    keydict[frame].scale[fcu.array_index] = value
    # pre-iterate through <bone, keys> dictionary
    n_keyable = 0
    for name, keydict in bonemap.items():
        n_keys = len(keydict)
        if n_keys > 0: n_keyable = n_keyable + 1
    # return animation data
    rv = AnimationData()
    rv.name = action.name
    rv.n_keyable = n_keyable
    rv.bonemap = bonemap
    return rv

#
# MESH FUNCTIONS
#
def GetUVMapChannel(mesh, name):
	for index, uv_layer in enumerate(mesh.uv_layers):
		if uv_layer.name == name: return index
	return -1
def GetVertexBufferPositions(mesh):
	if len(mesh.vertices) == 0: return None
	buffer = []
	for i in range(len(mesh.vertices)):
		# multiply vertices by matrix_world to get them in world coordinates
		v = mesh.vertices[i]
		temp = GetObjectFromMeshData(mesh).matrix_world * v.co
		temp[0] = temp[0]
		temp[1] = temp[1]
		temp[2] = temp[2]
		buffer.append(temp)
	return buffer
def GetVertexBufferNormals(mesh):
	if len(mesh.vertices) == 0: return None
	buffer = []
	for i in range(len(mesh.vertices)):
		v = mesh.vertices[i]
		temp = v.normal
		temp[0] = temp[0]
		temp[1] = temp[1]
		temp[2] = temp[2]
		buffer.append(temp)
	return buffer
def GetVertexBufferTexcoord(meshobj):
    if (meshobj is None) or (meshobj.data is None): return None
    if IsPortalMesh(meshobj): raise Exception('You cannot use this function on portal meshes.')
    mesh = meshobj.data
    if mesh.vertices is None or mesh.uv_layers is None: return None
    if len(mesh.vertices) == 0: return None
    if len(mesh.uv_layers) == 0: return None
    buffer = [[]] * len(mesh.uv_layers)
    for i in range(len(mesh.uv_layers)):
        for j in range(len(mesh.vertices)):
            buffer[i].append([0.0, 0.0])
    for poly in mesh.polygons:
        if poly.loop_total != 3: raise Exception('Mesh geometry contains non-triangles.')
        for loop_index in poly.loop_indices:
            vindex = mesh.loops[loop_index].vertex_index
            for channel in range(len(mesh.uv_layers)):
                data = mesh.uv_layers[channel].data[loop_index]
                buffer[channel][vindex] = data.uv
    return buffer
def GetVertexBufferColors(meshobj):
    if (meshobj is None) or (meshobj.data is None): return None
    if IsPortalMesh(meshobj): raise Exception('You cannot use this function on portal meshes.')
    mesh = meshobj.data
    if mesh.vertices is None or mesh.vertex_colors is None: return None
    n_verts = len(mesh.vertices)
    n_channels = len(mesh.vertex_colors)
    if len(mesh.vertices) == 0 or len(mesh.vertex_colors) == 0: return None
    buffer = [[]] * n_channels
    for i in range(n_channels):
        for j in range(len(mesh.vertices)):
            buffer[i].append([0.0, 0.0, 0.0, 0.0])
    for poly in mesh.polygons:
        if poly.loop_total != 3: raise Exception('Mesh geometry contains non-triangles.')
        for loop_index in poly.loop_indices:
            vindex = mesh.loops[loop_index].vertex_index
            for channel in range(n_channels):
                data = mesh.vertex_colors[channel].data[loop_index]
                buffer[channel][vindex][0] = data.color[0]
                buffer[channel][vindex][1] = data.color[1]
                buffer[channel][vindex][2] = data.color[2]
    return buffer
def GetVertexBufferBlendData(meshobj):
    if (meshobj is None) or (meshobj.data is None): return None
    if IsPortalMesh(meshobj): raise Exception('You cannot use this function on portal meshes.')
    mesh = meshobj.data
    # no vertices 
    if (mesh.vertices is None): return None
    n_verts = len(mesh.vertices)
    if n_verts == 0: return None
    # get armature object
    armature = GetArmatureObjectFromMesh(mesh)
    if not HasBones(armature): return None
    # get bone map <key = bone name, value = bone index>
    bonemap = CreateIndexBoneMap(armature)
    if bonemap is None: return None
    # get vertex group dictionary <key = vertex group index, value = vertex group name>
    vg_dict = CreateVertexGroupDictionary(meshobj)
    if vg_dict is None: return None
    # get blend data
    buffer = [[], []]
    for i in range(n_verts):
        # get list of (vertex group index, weight)
        vertex = mesh.vertices[i]
        if len(vertex.groups) > 4: raise Exception('Cannot have more than four weights per vertex.')
        # for each (vertex group index, weight)
        bi = [0, 0, 0, 0]
        bw = [0.0, 0.0, 0.0, 0.0]
        for i, vg in enumerate(vertex.groups):
            if vg.group not in vg_dict:
                raise Exception('The vertex group ({}) was not found in vertex group dictionary.'.format(vg.group))
            vg_name = vg_dict[vg.group]
            if vg_name not in bonemap:
                raise Exception('The vertex group name ({}) was not found in bone dictionary'.format(vg_name))
            bone_index = bonemap[vg_name]
            bi[i] = bone_index
            bw[i] = vg.weight
        # insert values
        buffer[0].append(bi)
        buffer[1].append(bw)
    return buffer;
def GetIndexBuffer(meshobj):
    # Returns the index buffer regardless of material. Use this method for col-
    # lision detection meshes.
    if (meshobj is None) or (meshobj.data is None): return None
    mesh = meshobj.data
    ib = []
    for poly in mesh.polygons:
        if poly.loop_total != 3: raise Exception('Mesh geometry contains non-triangles.')
        face = [
            mesh.loops[poly.loop_indices[0]].vertex_index,
            mesh.loops[poly.loop_indices[1]].vertex_index,
            mesh.loops[poly.loop_indices[2]].vertex_index]
        ib.append(face)
    return ib
def GetIndexBufferDictionary(meshobj):
    # In Blender, each face can be assigned a different material. We need to be
    # able to keep the faces that use the same materials together. Given a mat-
    # erial index, we look it up in a dictionary to obtain a list of faces that
    # use that material.
    if (meshobj is None) or (meshobj.data is None): return None
    if IsPortalMesh(meshobj): raise Exception('You cannot use this function on portal meshes.')
    mesh = meshobj.data
    facedict = {}
    for index in range(len(mesh.materials)): facedict[index] = []
    for poly in mesh.polygons:
        if poly.loop_total != 3: raise Exception('Mesh geometry contains non-triangles.')
        face = [
            mesh.loops[poly.loop_indices[0]].vertex_index,
            mesh.loops[poly.loop_indices[1]].vertex_index,
            mesh.loops[poly.loop_indices[2]].vertex_index]
        facedict[poly.material_index].append(face)
    return facedict

#
# VERTEX GROUP FUNCTIONS
#
def CreateVertexGroupDictionary(meshobj):
    # dictionary<vertex group index, vertex group name>
    if (meshobj is None) or (meshobj.data is None): return None
    if IsPortalMesh(meshobj): raise Exception('You cannot use this function on portal meshes.')
    vgdict = {}
    for vg in meshobj.vertex_groups: vgdict[vg.index] = vg.name
    return vgdict

#
# MATERIAL FUNCTIONS
#
#region MATERIAL_FUNCTIONS

## GetTexturedMaterials
#  Returns a list of material objects that reference one or more textures since
#  empty material declarations are useless. Because there are always (16) slots
#  reserved for textures in Blender, slots should be queried to see if they are
#  being used.
#  @return A list of material objects that reference textures.
def GetTexturedMaterials():
    rv = []
    for material in bpy.data.materials:
        if material.texture_slots is not None:
            used_slots = 0
            for slot in material.texture_slots:
                if (slot is not None) and (slot.use == True): used_slots = used_slots + 1
            if used_slots > 0: rv.append(material)
    return rv

## GetTexturedMeshMaterials(meshlist)
#  Same function as above, but only collects materials associated with meshes
#  provided in the meshlish parameter.
#  @param meshlist An array of Blender mesh objects (not mesh data).
#  @return A list of material objects that reference textures.
def GetTexturedMeshMaterials(meshlist):
    if (meshlist is None) or (len(meshlist) == 0): return None
    rv = set()
    for meshobj in meshlist:
        mesh = meshobj.data
        for material in mesh.materials:
            for slot in material.texture_slots:
                if(slot is not None) and (slot.use == True):
                    rv.add(material)
                    break
    return rv

## GetMaterialDictionary(meshlist)
#  Given a list of mesh objects, iterate through the list extracting any materials
#  that they reference. A dictionary is used to avoid storing duplicate materials.
#  Before using this function, make sure to filter meshlist to remove any special
#  meshes such as portal meshes, door controller meshes, etc.
#  @param meshlist An array of Blender mesh objects (not mesh data).
#  @return If any materials are referenced by the meshes in the list, a dictionary
#  of used material names is returned. dict[name] = Material(). Otherwise, None is
#  returned.
def GetMaterialDictionary(meshlist):
    if (meshlist is None) or (len(meshlist) == 0): return None
    rv = {}
    for meshobj in meshlist:
        mesh = meshobj.data
        n_mats = len(mesh.materials)
        used = set()
        for poly in mesh.polygons:
            if (poly.material_index in used) == False:
                used.add(poly.material_index)
                if len(used) == n_mats: break
        for material_index in used:
            name = mesh.materials[material_index].name
            if (name in rv) == False:
                mobj = Material()
                mobj.name = name
                mobj.textures = GetMeshMaterialTextures(mesh, mesh.materials[material_index])
                rv[name] = mobj
    return rv;
def GetMeshMaterials(mesh):
	if((mesh == None) or (len(mesh.materials) == 0)): return None
	rv = []
	for material in mesh.materials:
		mobj = Material()
		mobj.name = material.name
		mobj.textures = GetMeshMaterialTextures(mesh, material)
		rv.append(mobj)
	return rv
def GetMeshMaterialTextures(mesh, material):
    rv = []
    for slot in material.texture_slots:
        if ((slot is not None) and (slot.use == True)):
            if slot.texture_coords != 'UV': raise Exception('Materials must use UV texture coordinates.')
            if slot.texture.type != 'IMAGE': raise Exception('Materials must use image textures.')
            # assign texture name
            mt = MaterialTexture()
            mt.name = slot.name
            # assign texture type
            if 'type' in slot.texture: mt.type = slot.texture['type']
            else: mt.type = 'diffuse'
            # assign texture channel
            mt.channel = GetUVMapChannel(mesh, slot.uv_layer)
            if mt.channel == -1: raise Exception('This material refers to a UV map that does not exist.')
            # assign texture filename
            if slot.texture.image != None:
                mt.filename = slot.texture.image.filepath
                if 'path' in slot.texture: mt.filename = slot.texture['path'] + '\\' + slot.texture.image.filepath.split('\\')[-1:][0]
            else: mt.filename = 'default.bmp'
            rv.append(mt)
    return rv
#endregion MATERIAL_FUNCTIONS

#
# PORTAL FUNCTIONS
#
def IsPortalMesh(obj):
    if IsMeshObject(obj):
        # 1st - check custom property
        if obj is None: return False
        if 'entity_type' in obj:
            type = obj['entity_type']
            if type == 'PORTAL': return True
        # 2nd - check mesh name
        list = obj.name.split('_')
        if (len(list) and list[0] == 'portal'): return True
        # 3rd - check mesh data name
        mesh = obj.data
        if mesh is not None:
            list = mesh.name.split('_')
            if len(list): return list[0] == 'portal'
        return False
def SetPortalMesh():
	meshlist = GetSelectedMeshObjects()
	for mesh in meshlist: mesh['entity_type'] = 'PORTAL'
def IsCellMesh(mesh):
	# 1st - check custom property
	if mesh is None: return False
	if 'is_cell' in mesh:
		is_cell = mesh['is_cell']
		if 'is_portal' in mesh and is_cell and mesh['is_portal']:
			raise Exception('Mesh {} has is_cell and is_portal set to True.'.format(mesh.name))
		return is_cell
	# 2nd - check mesh name
	list = mesh.name.split('_')
	if (len(list) and (list[0] == 'room' or list[0] == 'cell')): return True
	# 3rd - check mesh object name
	obj = GetObjectFromMesh(mesh)
	if obj:
		list = obj.name.split('_')
		if len(list): return (list[0] == 'room' or list[0] == 'cell')
	return False
def SetCellMeshState(state):
	meshlist = GetSelectedMeshObjects()
	if meshlist is None: return
	for mesh in meshlist: mesh['is_cell'] = state

###
### DOOR CONTROLLER FUNCTIONS
###
def IsDoorController(obj):
    if IsMeshObject(obj):
        # 1st - check custom property
        if obj is None: return False
        if 'entity_type' in obj:
            entity_type = obj['entity_type']
            return entity_type == 'DOOR_CONTROLLER'
        # 2nd - check object name
        list = obj.name.split('_')
        if (len(list) and list[0] == 'DC'): return True
    return False
def GetDoorControllerDims(obj):
    if IsMeshObject(obj) == False: return []
    # compute location
    location = obj.location
    # compute orientation
    rotation = obj.rotation_quaternion
    #compute bounding box
    min_v = [ obj.bound_box[0][0], obj.bound_box[0][1], obj.bound_box[0][2] ]
    max_v = [ obj.bound_box[0][0], obj.bound_box[0][1], obj.bound_box[0][2] ]
    for p in obj.bound_box:
        v = obj.matrix_world*mathutils.Vector(p)
        if v[0] < min_v[0]: min_v[0] = v[0]
        if v[1] < min_v[1]: min_v[1] = v[1]
        if v[2] < min_v[2]: min_v[2] = v[2]
        if max_v[0] < v[0]: max_v[0] = v[0]
        if max_v[1] < v[1]: max_v[1] = v[1]
        if max_v[2] < v[2]: max_v[2] = v[2]
    rv = BoundingBox()
    rv.position = location
    rv.rotation = rotation
    rv.halfdims = [(max_v[0] - min_v[0])/2.0, (max_v[1] - min_v[1])/2.0, (max_v[2] - min_v[2])/2.0]
    return rv

###
### COLLISION MESH FUNCTIONS
###
def IsCollisionMesh(obj):
    if IsMeshObject(obj):
        # 1st - check custom property
        if obj is None: return False
        if 'entity_type' in obj:
            entity_type = obj['entity_type']
            return entity_type == 'COLLISION_MESH'
        # 2nd - check object name
        list = obj.name.split('_')
        if (len(list) and list[0] == 'collision'): return True
    return False
def GetCollisionMeshObjects():
    rv = []
    list = GetMeshObjects()
    for meshobj in list:
        if IsCollisionMesh(meshobj) is True: rv.append(meshobj)
    return rv;

##
## EXPORT FUNCTIONS
##
#region EXPORT_FUNCTIONS

## ExportMeshUTF
#  
def ExportMeshUTF():

    ###
    ### PHASE #1
    ### CREATE FILE
    ###

    # create file
    splitpath = GetFilePathSplitExt()
    filename = splitpath[0] + ".txt"
    file = open(filename, 'w')

    ###
    ### PHASE #2
    ### SEPARATE MESHES BY TYPE
    ###

    # mesh types
    meshlist1 = [] # portal meshes
    meshlist2 = [] # normal meshes
    meshlist3 = [] # collision meshes
    meshlist4 = [] # door controllers

    # count number of portal and non-portal meshes
    n_mesh = 0
    n_portal_mesh = 0
    n_door_controllers = 0
    n_collision = 0
    for mesh in GetMeshObjects():
        # portal mesh
        if IsPortalMesh(mesh):
            meshlist1.append(mesh)
            n_portal_mesh = n_portal_mesh + 1
        # door controller mesh
        elif IsDoorController(mesh):
            meshlist2.append(mesh)
            n_door_controllers = n_door_controllers + 1
        # collision mesh
        elif IsCollisionMesh(mesh):
            meshlist3.append(mesh)
            n_collision = n_collision + 1
        # normal mesh
        else:
            meshlist4.append(mesh)
            n_mesh = n_mesh + 1

    ###
    ### PHASE #3
    ### SAVE ARMATURE
    ###

    # track skeletons used by meshes
    skellist = []
    for mesh in meshlist4:
    	armature = GetArmatureObjectFromMesh(mesh)
    	if(not armature is None): skellist.append(armature)
    n_skeleton = len(skellist)
    if n_skeleton > 1: raise Exception('Meshes cannot be controlled by more than one skeleton object.')

    # construct and save armature
    if n_skeleton:
    	data = ConstructArmatureDataFromPose(skellist[0])
    	file.write('{} # number of bones\n'.format(data.n_bones))
    	for bone in data.bonelist:
    		file.write(bone.name + "\n")
    		file.write("{}\n".format(bone.parent))
    		WriteVector3(file, bone.position)
    		file.write("{} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}\n".format(
    			bone.matrix[0][0], bone.matrix[0][1], bone.matrix[0][2], bone.matrix[0][3],
    			bone.matrix[1][0], bone.matrix[1][1], bone.matrix[1][2], bone.matrix[1][3],
    			bone.matrix[2][0], bone.matrix[2][1], bone.matrix[2][2], bone.matrix[2][3],
    			bone.matrix[3][0], bone.matrix[3][1], bone.matrix[3][2], bone.matrix[3][3]))
    else:
    	file.write('0 # number of bones\n')

    ###
    ### PHASE #4
    ### SAVE ANIMATIONS
    ###

    # construct and save animations
    animdata = None
    n_anim = 0
    if n_skeleton:
    	# get animation data
    	animdata = ConstructAnimationData(skellist[0])
    	if animdata is None: n_anim = 0
    	else: n_anim = len(animdata)
    	# print animations
    	file.write('{} # number of animations\n'.format(n_anim))
    	for anim in animdata:
    		file.write(anim.name + '\n')
    		file.write('{}'.format(anim.n_keyable) + ' # number of keyframed bones\n')
    		# offsets (just in case)
    		offset = [0.0, 0.0, 0.0] # armature.location[0]
    		# iterate through <bone, keys> dictionary
    		for name, keydict in anim.bonemap.items():
    			n_keys = len(keydict)
    			if n_keys > 0:
    				file.write(name + '\n')
    				file.write('{}'.format(len(keydict)) + ' # number of keys\n')
    				for frame, keyval in sorted(keydict.items()):
    					file.write('{}\n'.format(int(frame)))
    					WriteOffsetVector3(file, keyval.position, offset)
    					WriteVector4(file, keyval.rotation)
    					WriteVector3(file, keyval.scale)
    else:
    	file.write('0 # number of animations\n')

    ###
    ### PHASE #5
    ### SAVE COLLISION MESHES
    ###

    # save collision meshes
    file.write('{} # number of collision meshes\n'.format(n_collision))
    for meshobj in meshlist3:

        # mesh data
        mesh = meshobj.data
        if IsMeshDataValid(mesh) == False: raise Exception('Collision mesh does not contain valid mesh data.')

        # mesh properties
        n_verts = len(mesh.vertices)
        file.write('{} # number of collision vertices\n'.format(n_verts))
    
        # build vertex buffer positions
        vb = GetVertexBufferPositions(mesh)
        if vb is None or len(vb) == 0:
            raise Exception('The collision mesh {} has no vertex buffer.'.format(mesh.name))
        if len(vb) != len(mesh.vertices):
            raise Exception('The number of mesh vertices for collision mesh {} do not match.'.format(mesh.name))

        # write vertex buffer
        for i in range(len(vb)): 
            file.write("{:.4f} {:.4f} {:.4f}\n".format(vb[i][0], vb[i][1], vb[i][2]))

        # index buffer data and properties
        ib = GetIndexBuffer(meshobj)
        if ib is None or len(ib) == 0:
            raise Exception('The collision mesh {} has no index buffer.'.format(mesh.name))
        n_faces = len(ib)

        # write index buffer
        file.write('{} # number of collision faces\n'.format(n_faces))
        for face in ib: file.write('{} {} {}\n'.format(face[0], face[1], face[2]))

    ###
    ### PHASE #6
    ### SAVE MATERIALS
    ###

    # build material dictionary
    matdict = GetMaterialDictionary(meshlist4)

    # write number of materials
    n_materials = 0
    if matdict is not None: n_materials = len(matdict)
    file.write('{} # of materials\n'.format(n_materials))

    # write material data
    if matdict is not None:
        for name, mat in matdict.items():
            file.write('{}\n'.format(name))
            if mat.textures != None:
                file.write('{} # number of textures\n'.format(len(mat.textures)))
                for texture in mat.textures:
                    file.write('{}\n'.format(texture.name))
                    file.write('{}\n'.format(texture.type))
                    file.write('{}\n'.format(texture.channel))
                    file.write('{}\n'.format(texture.filename))
            else:
                file.write('0 # of textures\n')
                        
    ###
    ### PHASE #7
    ### SAVE MESHES
    ###

    # save meshes
    file.write('{} # number of meshes\n'.format(n_mesh))
    for meshobj in meshlist4:

        # mesh data
        mesh = meshobj.data
        if IsMeshDataValid(mesh) == False: continue
        
        # mesh properties
        n_verts = len(mesh.vertices)
        n_channels = len(mesh.uv_layers)
        n_colors = len(mesh.vertex_colors)

        # build mesh materials
        matlist = GetMeshMaterials(mesh)
        if (matlist is None) or (len(matlist) == 0):
            raise Exception('The mesh {} does not have a material.'.format(mesh.name))

        # build vertex buffer positions
        vbuffer1 = GetVertexBufferPositions(mesh)
        if vbuffer1 is None or len(vbuffer1) == 0:
            raise Exception('The mesh {} has no vertex buffer.'.format(mesh.name))
        if len(vbuffer1) != len(mesh.vertices):
            raise Exception('The number of mesh vertices for mesh {} do not match.'.format(mesh.name))

        # build vertex buffer normals
        vbuffer2 = GetVertexBufferNormals(mesh)
        if vbuffer2 is None or len(vbuffer2) == 0:
            raise Exception('The mesh {} has no vertex buffer.'.format(mesh.name))
        if len(vbuffer2) != len(mesh.vertices):
            raise Exception('The number of mesh vertices for mesh {} do not match.'.format(mesh.name))

        # build vertex buffer texture coordinates
        vbuffer3 = []
        vbuffer4 = []
        uvdata = GetVertexBufferTexcoord(meshobj)
        if uvdata:
            if n_channels > 0: vbuffer3 = uvdata[0]
            if n_channels > 1: vbuffer4 = uvdata[1]

        # build vertex buffer blend data
        vbuffer5 = None
        vbuffer6 = None
        blenddata = GetVertexBufferBlendData(meshobj)
        if blenddata:
            vbuffer5 = blenddata[0]
            vbuffer6 = blenddata[1]

        # build vertex buffer color coordinates
        vbuffer7 = []
        vbuffer8 = []
        cdata = GetVertexBufferColors(meshobj)
        if cdata:
            if n_colors > 0: vbuffer7 = cdata[0]
            if n_colors > 1: vbuffer8 = cdata[1]

        # build index buffers
        # facedict[mesh data material index] = face
        facedict = GetIndexBufferDictionary(meshobj)

        # count submeshes and total faces
        n_submesh = 0
        n_faces = 0
        if facedict != None:
            for i, facelist in facedict.items():
                if len(facelist) > 0:
                    n_submesh = n_submesh + 1
                    n_faces = n_faces + len(facelist)

        # output mesh if valid
        if facedict != None:

            # write mesh name
            file.write('{}\n'.format(mesh.name))

            # write vertex buffers header
            file.write('{} # number of vertices\n'.format(n_verts))
            file.write('{} # number of UV channels\n'.format(n_channels))
            file.write('{} # number of color channels\n'.format(n_colors))

            # write vertex buffers
            for i in range(len(vbuffer1)): 
                file.write("{:.4f} {:.4f} {:.4f}\n".format(vbuffer1[i][0], vbuffer1[i][1], vbuffer1[i][2]))
                file.write("{:.4f} {:.4f} {:.4f}\n".format(vbuffer2[i][0], vbuffer2[i][1], vbuffer2[i][2]))
                if n_channels > 0: file.write("{:.4f} {:.4f}\n".format(vbuffer3[i][0], vbuffer3[i][1]))  
                if n_channels > 1: file.write("{:.4f} {:.4f}\n".format(vbuffer4[i][0], vbuffer4[i][1]))
                if vbuffer5 is not None and vbuffer6 is not None:
                    file.write("{} {} {} {}\n".format(vbuffer5[i][0], vbuffer5[i][1], vbuffer5[i][2], vbuffer5[i][3]))  
                    file.write("{:.4f} {:.4f} {:.4f} {:.4f}\n".format(vbuffer6[i][0], vbuffer6[i][1], vbuffer6[i][2], vbuffer6[i][3]))
                if n_colors > 0: file.write("{:.4f} {:.4f} {:.4f} {:.4f}\n".format(vbuffer7[i][0], vbuffer7[i][1], vbuffer7[i][2], vbuffer7[i][3]))  
                if n_colors > 1: file.write("{:.4f} {:.4f} {:.4f} {:.4f}\n".format(vbuffer8[i][0], vbuffer8[i][1], vbuffer8[i][2], vbuffer8[i][3]))

            # write index buffers
            # for each material index (i), output faces (facelist)
            file.write('{} # number of total faces\n'.format(n_faces))
            file.write('{} # number of submeshes\n'.format(n_submesh))
            for i, facelist in facedict.items():
                if len(facelist) > 0:
                    file.write('{} # number of faces\n'.format(len(facelist)))
                    file.write('{}\n'.format(mesh.materials[i].name))
                    for face in facelist:
                        file.write('{} {} {}\n'.format(face[0], face[1], face[2]))

    # close file
    file.close()

#endregion EXPORT_FUNCTIONS

ExportMeshUTF()