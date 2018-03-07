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
		if (IsArmature(obj) and obj.select): r.append(obj)
	return r
def GetSelectedMeshObjects():
	r = []
	for obj in GetObjects():
		if (IsMesh(obj) and obj.select): r.append(obj)
	return r
def IsArmature(obj): return (True if obj.type == 'ARMATURE' else False)
def IsMesh(obj): return (True if obj.type == 'MESH' else False)
def GetObjectFromMesh(mesh):
	if mesh is None: return None
	for obj in GetObjects():
		if obj.data.name == mesh.name: return obj
	return None
	
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
def GetVertexBufferPositions(mesh):
	if len(mesh.vertices) == 0: return None
	buffer = []
	for i in range(len(mesh.vertices)):
		# multiply vertices by matrix_world to get them in world coordinates
		v = mesh.vertices[i]
		temp = GetObjectFromMesh(mesh).matrix_world * v.co
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
def GetVertexBufferTexcoord(mesh):
	if mesh is None: raise Exception('You cannot use this function on a null mesh.')
	if IsPortalMesh(mesh): raise Exception('You cannot use this function on portal meshes.')
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
def GetVertexBufferColors(mesh):
	if mesh is None: raise Exception('You cannot use this function on a null mesh.')
	if IsPortalMesh(mesh): raise Exception('You cannot use this function on portal meshes.')
	if mesh.vertices is None or mesh.uv_layers is None: return None
	if len(mesh.vertices) == 0: return None
	if len(mesh.uv_layers) == 0: return None
	
	buffer = [[]] * len(mesh.vertex_colors)
	return buffer
def GetIndexBufferDictionary(mesh):
	if mesh is None: raise Exception('You cannot use this function on a null mesh.')
	if IsPortalMesh(mesh): raise Exception('You cannot use this function on portal meshes.')
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
	# name     String
	# textures MaterialTexture[]
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
		mobj = Material()
		mobj.name = material.name
		mobj.textures = GetMeshMaterialTextures(mesh, material)
		rv.append(mobj)
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
			mt.channel = GetUVMapChannel(mesh, slot.uv_layer)
			if mt.channel == -1: raise Exception('This material refers to a UV map that doesn\'t exist.')
			# assign texture filename
			if slot.texture.image != None: mt.filename = slot.texture.image.filepath
			else: mt.filename = 'default.bmp'
			rv.append(mt)
	return rv

#
# PORTAL FUNCTIONS
#
def IsPortalMesh(mesh):
	# 1st - check custom property
	if mesh is None: return False
	if 'is_portal' in mesh:
		is_portal = mesh['is_portal']
		if 'is_cell' in mesh and is_portal and mesh['is_cell']:
			raise Exception('Mesh {} has is_cell and is_portal set to True.'.format(mesh.name))
		return is_portal
	# 2nd - check mesh name
	list = mesh.name.split('_')
	if (len(list) and list[0] == 'portal'): return True
	# 3rd - check mesh object name
	obj = GetObjectFromMesh(mesh)
	if obj:
		list = obj.name.split('_')
		if len(list): return list[0] == 'portal'
	return False
def SetPortalMeshState(state):
	meshlist = GetSelectedMeshObjects()
	if meshlist is None: return
	for mesh in meshlist: mesh['is_portal'] = state
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
	
# create file
splitpath = GetFilePathSplitExt()
filename = splitpath[0] + "_test.txt"
file = open(filename, 'w')

meshlist = GetMeshObjects()
for mesh in meshlist:

	# mesh properties
	n_verts = len(mesh.vertices)
	n_channels = len(mesh.uv_layers)
	n_colors = len(mesh.vertex_colors)
		
	# if portal
	if IsPortalMesh(mesh):
	
		pass
		
	# if cell
	elif IsCellMesh(mesh):

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
		uvdata = GetVertexBufferTexcoord(mesh)
		if uvdata:
			if n_channels > 0: vbuffer3 = uvdata[0]
			if n_channels > 1: vbuffer4 = uvdata[1]
	
		vbuffer5 = []
		vbuffer6 = []
		vbuffer7 = []
		vbuffer8 = []
	
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

			# write mesh name
			file.write('{}\n'.format(mesh.name))
	
			# write material data
			file.write('{} # of materials\n'.format(len(matlist)))
			for mat in matlist:
				file.write('{}\n'.format(mat.name))
				if mat.textures != None:
					file.write('{} # of textures\n'.format(len(mat.textures)))
					for texture in mat.textures:
						file.write('{}\n'.format(texture.name))
						file.write('{}\n'.format(texture.type))
						file.write('{}\n'.format(texture.channel))
						file.write('{}\n'.format(texture.filename))
				else:
					file.write('0 # of textures\n')

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