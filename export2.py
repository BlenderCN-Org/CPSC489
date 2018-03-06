import bpy
import os
import re

#
# FILENAME AND PATHNAME FUNCTIONS
#
def GetPythonPath(): return bpy.app.binary_path_python
def GetFileFullPath(): return bpy.data.filepath
def GetFilePathName(): return os.path.splitext(bpy.data.filepath)[0]
def GetFilePathSplit(): return os.path.split(bpy.data.filepath) 
def GetFilePathSplitExt(): return os.path.splitext(bpy.data.filepath) 

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
def GetObjects() : return bpy.data.objects
def GetArmatureObjects(): return bpy.data.armatures
def GetMeshObjects(): return bpy.data.meshes
def GetSelectedArmatureObjects():
	r = []
	for obj in GetObjects():
		if ((IsArmature(obj) == True) and (obj.select == True)): r.append(obj)
	return r
def GetSelectedMeshObjects():
	r = []
	for obj in GetObjects():
		if ((IsMesh(obj) == True) and (obj.select == True)): r.append(obj)
	return r
def IsArmature(obj): return (True if obj.type == 'ARMATURE' else False)
def IsMesh(obj): return (True if obj.type == 'MESH' else False)

#
# ARMATURE FUNCTIONS
#
def HasBones(armature): return ((armature != None) and (len(armature.data.bones) > 0))
def CreateIndexBoneMap(armature):
	if HasBones(armature) == False: return {}
	bonemap = {}
	for index, bone in enumerate(armature.data.bones): bonemap[bone.name] = index
	return bonemap

#
# MESH FUNCTIONS
#
def GetUVMapChannel(mesh, name):
	for index, uv_layer in enumerate(mesh.uv_layers):
		if uv_layer.name == name: return index
	return -1
def GetIndexBufferDictionary(mesh):
	if IsPortalMesh(mesh.name): return None
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
# MATERIAL FUNCTIONS
#
class Material:
	# n_materials
	pass
class MaterialTexture:
	# name
	# uvmap
	# filename
	pass

def GetMeshMaterials(mesh):
	if((mesh == None) or (len(mesh.materials) == 0)): return None
	for material in mesh.materials:
		print(material.name)
	return None
def GetMeshMaterialTextures(material):
	rv = []
	for slot in material.texture_slots:
		if ((slot != None) and (slot.use == True)):
			if slot.texture_coords != 'UV': raise Exception('Materials must use UV texture coordinates.')
			if slot.texture.type != 'IMAGE': raise Exception('Materials must use image textures.')
			mt = MaterialTexture()
			mt.name = slot.name
			mt.uvmap = slot.uv_layer
			if slot.texture.image != None: mt.filename = slot.texture.image.filepath
			else: mt.filename = 'default.bmp'
			rv.append(mt)
	return rv
	
#
# PORTAL FUNCTIONS
#
def IsPortalMesh(objname):
	if(objname == None): return False
	list = objname.split('_')
	if(len(list) == 0): return False
	return list[0] == 'portal'
def IsRoomMesh(objname):
	if(objname == None): return False
	list = objname.split('_')
	if(len(list) == 0): return False
	return list[0] == 'room'

# create file
splitpath = GetFilePathSplitExt()
filename = splitpath[0] + "_test.txt"
file = open(filename, 'w')

meshlist = GetMeshObjects()
for mesh in meshlist:
	print(mesh.name)
	facedict = GetIndexBufferDictionary(mesh)
	if facedict != None: print('{} #number of facedict'.format(len(facedict)))
	else: print('No face dict.')
	if facedict != None and len(facedict) > 0:
		for i, facelist in facedict.items():
			file.write('{} # number of faces'.format(len(facelist)))