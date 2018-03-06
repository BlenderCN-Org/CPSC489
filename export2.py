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
	# name
	# MaterialTexture[]
	pass
class MaterialTexture:
	# name
	# type
	# channel
	# filename
	pass

def GetMeshMaterials(mesh):
	if((mesh == None) or (len(mesh.materials) == 0)): return None
	rv = []
	for material in mesh.materials:
		mt = GetMeshMaterialTextures(mesh, material)
		if mt != None: rv.append(mt)
	return rv
def GetMeshMaterialTextures(mesh, material):
	rv = []
	for slot in material.texture_slots:
		if ((slot != None) and (slot.use == True)):
			if slot.texture_coords != 'UV': raise Exception('Materials must use UV texture coordinates.')
			if slot.texture.type != 'IMAGE': raise Exception('Materials must use image textures.')
			# assign texture name
			mt = MaterialTexture()
			mt.name = slot.name
			# assign texture type
			if 'type' in slot.texture: mt.type = slot.texture['type']
			else: mt.type = 'diffuse'
			# assign texture channel
			mt.channel = -1
			for index, uv_layer in enumerate(mesh.uv_layers):
				if uv_layer.name == slot.uv_layer: mt.channel = index;
			if mt.channel == -1: raise Exception('This material refers to a UV map that doesn\'t exist.')
			# assign texture filename
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

	# build mesh materials
	matlist = GetMeshMaterials(mesh)
	if (matlist == None) or (len(matlist) == 0):
		raise Exception('The mesh {} does not have a material.'.format(mesh.name))
	
	# build index buffers
	facedict = GetIndexBufferDictionary(mesh)

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

		# number of verts, texture channels, materials, and color channels
		n_verts = len(mesh.vertices)
		n_channels = len(mesh.uv_layers)
		n_colors = len(mesh.vertex_colors)
		
		# write mesh header
		file.write('{}\n'.format(mesh.name))
		file.write('{} # number of vertices\n'.format(n_verts))
		file.write('{} # number of UV channels\n'.format(n_channels))
		file.write('{} # number of color channels\n'.format(n_colors))

		# write material data
		file.write('{} # of materials\n'.format(len(matlist)))
		for mat in matlist:
			file.write('{} # of textures\n'.format(len(mat)))
			for texture in mat:
				file.write('{}\n'.format(texture.name))
				file.write('{}\n'.format(texture.type))
				file.write('{}\n'.format(texture.channel))
				file.write('{}\n'.format(texture.filename))
		
		# write submesh data
		file.write('{} # number of total faces\n'.format(n_faces))
		file.write('{} # number of submeshes\n'.format(n_submesh))
		for i, facelist in facedict.items():
			if len(facelist) > 0:
				file.write('{} # number of faces\n'.format(len(facelist)))
				file.write('{} # material index\n'.format(i))
				for face in facelist:
					file.write('{} {} {}\n'.format(face[0], face[1], face[2]))
			
# close file
file.close()