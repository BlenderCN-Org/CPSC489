#
# PYTHON IMPORTS
#
import array
import collections
import math
import os
import struct
import sys

#
# BLENDER IMPORTS
#
import bpy
import bmesh
import mathutils
import os
import re

## Python Doxygen Example
#  @brief
#  @details
#  @param <name> <description>
#  @param[in,out] <n1, n2, n3, ...> <description>
#  @return <description>

##
#  @brief Required for all plugins.
bl_info = {
'name': 'Import MeshUTF',
'author': 'Steven Emory',
'version': (1, 0),
'blender': (2, 79, 0),
'location': 'File > Import > Mesh UTF (.txt)',
'description': 'Imports a MeshUTF file.',
'warning': '',
'category': 'Import-Export',
}

class MeshUTFJoint:
    # name
    # parent
    # position
    # m_abs
    pass
class MeshUTFSkeleton:
    # n_joints
    # jntlist[]
    pass
class MeshUTFAnimationKeyframe:
    # self.position - vector3
    # self.rotation - vector4
    # self.scale    - vector3
    pass
class MeshUTFAnimation:
    # self.name      - string
    # self.jointmap  - dictionary<String, dictionary<uint, MeshUTFAnimationKeyFrame>>}
    pass
class MeshUTFCollisionMesh:
    # self.verts     - vector3<float>[]
    # self.faces     - vector3<uint>[]
    pass
class MeshUTFTexture:
    # self.name      - string
    # self.semantic  - string
    # self.uv_layer  - uint
    # self.filename  - string
    pass
class MeshUTFMaterial:
    # self.name      - string
    # self.index     - uint
    # self.textures  - MeshUTFTexture[]
    pass
class MeshUTFSurface:
    # self.n_faces   - uint
    # self.material  - uint
    # self.facelist  - vector3<uint>[]
    pass
class MeshUTFMesh:
    # self.name      - string
    # self.n_verts   - uint
    # self.n_uvs     - uint
    # self.n_colors  - uint
    # self.verts     - vector3<float>[]
    # self.normals   - vector3<float>[]
    # self.uvs       - vector2[][]
    # self.bi        - vector8<uint>[]
    # self.bw        - vector8<float>[]
    # self.colors    - vector2[][]
    # self.n_faces   - uint
    # self.surfaces  - MeshUTFSurface[]
    pass

class MeshUTFImporter:

    ##
    #
    def __init__(self):

        self.linelist = []
        self.linecurr = -1
        
        self.skeleton = MeshUTFSkeleton()
        self.animlist = []
        self.colllist = []
        self.mat_list = []
        self.meshlist = []

    ##
    #
    def read_string(self):
    
        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        return line
        
    ##
    #
    def read_int(self):
    
        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        return int(line)

    ##
    #
    def read_float(self):
    
        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        return float(line)

    ##
    #   @brief   Parses a line from a text file into a 2D float vector.
    #   @return  Returns [float, float].
    def read_vector2(self):
    
        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        parts = line.split(' ', 1)
        if len(parts) != 2: raise Exception('Expecting vector2.')        
        return [float(parts[0]), float(parts[1])]
        
    ##
    #   @brief   Parses a line from a text file into a 3D float vector.
    #   @return  Returns [float, float, float].
    def read_vector3(self):
    
        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        parts = line.split(' ', 2)
        if len(parts) != 3: raise Exception('Expecting vector3.')        
        return [float(parts[0]), float(parts[1]), float(parts[2])]

    ##
    #   @brief   Parses a line from a text file into a 4D float vector.
    #   @return  Returns [float, float, float, float].
    def read_vector4(self):
    
        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        parts = line.split(' ', 3)
        if len(parts) != 4: raise Exception('Expecting vector4.')
        return [float(parts[0]), float(parts[1]), float(parts[2]), float(parts[3])]

    ##
    #
    def read_matrix4(self):
    
        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        parts = line.split(' ', 15)
        if len(parts) != 16: raise Exception('Expecting matrix4x4.')
        return [
            [float(parts[ 0]), float(parts[ 1]), float(parts[ 2]), float(parts[ 3])],
            [float(parts[ 4]), float(parts[ 5]), float(parts[ 6]), float(parts[ 7])],
            [float(parts[ 8]), float(parts[ 9]), float(parts[10]), float(parts[11])],
            [float(parts[12]), float(parts[13]), float(parts[14]), float(parts[15])]]

    ##
    #
    def read_face3(self):
    
        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        parts = line.split(' ', 2)
        if len(parts) != 3: raise Exception('Expecting face3.')
        return [int(parts[0]), int(parts[1]), int(parts[2])]

    ##
    #
    def read_blendindices(self):

        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        parts = line.split(' ', 7)
        if len(parts) == 0: raise Exception('Expecting blendindices.')
        rv = [0, 0, 0, 0, 0, 0, 0, 0]
        for i in range(len(parts)): rv[i] = int(parts[i])
        return rv

    ##
    #
    def read_blendweights(self):

        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        parts = line.split(' ', 7)
        if len(parts) == 0: raise Exception('Expecting blendweights.')
        rv = [0, 0, 0, 0, 0, 0, 0, 0]
        for i in range(len(parts)): rv[i] = float(parts[i])
        return rv

    ##
    #
    def execute(self, filename):

        # reset data
        self.skeleton = MeshUTFSkeleton()
        self.animlist = []
        self.colllist = []
        self.mat_list = []
        self.meshlist = []

        # open file
        try: file = open(filename, 'r')
        except: raise Exception('Mesh UTF Import Error: Failed to open file.')
        
        # build a linelist, stripping blank lines
        self.linelist = []
        while True:
            line = file.readline()
            if line.endswith('\n') == False:
                break
            else:
                line = line.rstrip('\n\r\t ')
                line = line.split('#', 1)[0]
                if(len(line)): self.linelist.append(line)
                
        # process sections
        self.linecurr = 0
        if self.process_bones() == False: return
        if self.process_animations() == False: return
        if self.process_collision_meshes() == False: return
        if self.read_materials() == False: return
        if self.make_materials() == False: return
        if self.read_meshes() == False: return
        if self.make_meshes() == False: return

    ##
    #
    def process_bones(self):

        # temporary object
        self.skeleton = MeshUTFSkeleton()
        
        # read number of joints
        self.skeleton.n_jnts = self.read_int()
        if self.skeleton.n_jnts < 0: raise Exception('Invalid number of joints.')

        # read joints
        self.skeleton.jntlist = []
        for i in range(self.skeleton.n_jnts):

            # read joint name
            joint = MeshUTFJoint()
            joint.name = self.read_string()
            if len(joint.name) == 0: raise Exception('Invalid joint name.')
        
            # read parent ID
            joint.parent = self.read_int()
            if joint.parent >= self.skeleton.n_jnts: raise Exception('Invalid joint parent reference.')
        
            # read position and joint matrix
            joint.position = self.read_vector3()
            joint.m_abs = self.read_matrix4()

            # insert joint
            self.skeleton.jntlist.append(joint)

        # populate armature object with bones and restore mode
        prev_mode = bpy.context.mode
        self.build_skeleton()
        if bpy.context.mode != prev_mode: bpy.ops.object.mode_set(mode=prev_mode)

        return True

    ##
    #   @brief Generates armature object in Blender from joint data.
    #   @param None
    def build_skeleton(self):

        # nothing to do
        if self.skeleton.n_jnts == 0: return True

        # create armature object (must be done in OBJECT mode)
        if bpy.context.mode != 'OBJECT': bpy.ops.object.mode_set(mode='OBJECT')
        bpy.ops.object.add(type='ARMATURE')

        # set armature properties
        armature = bpy.context.active_object
        armature.name = 'skeleton'
        armature.data.name = 'skeleton'
        armature.location = [0, 0, 0]

        # go into edit mode to edit skeleton object
        if bpy.context.mode != 'EDIT': bpy.ops.object.mode_set(mode='EDIT')

        # create bone for each joint in skeleton
        bl_jntlist = []
        for i, joint in enumerate(self.skeleton.jntlist):

            # create bone and save it so we can attach children to parents
            bl_jnt = armature.data.edit_bones.new(joint.name)
            bl_jntlist.append(bl_jnt)

            # set the matrix now, before setting head and tail positions
            bl_jnt.matrix = [
                [joint.m_abs[0][0], joint.m_abs[0][1], joint.m_abs[0][2], joint.m_abs[0][3]],
                [joint.m_abs[1][0], joint.m_abs[1][1], joint.m_abs[1][2], joint.m_abs[1][3]],
                [joint.m_abs[2][0], joint.m_abs[2][1], joint.m_abs[2][2], joint.m_abs[2][3]],
                [joint.m_abs[3][0], joint.m_abs[3][1], joint.m_abs[3][2], joint.m_abs[3][3]]]
        
            # set a small bone along the joint's y-axis 
            dy = [
                0.01*joint.m_abs[1][0],
                0.01*joint.m_abs[1][1],
                0.01*joint.m_abs[1][2]]
        
            bl_jnt.head = [joint.position[0], joint.position[1], joint.position[2]]
            bl_jnt.tail = [
                joint.position[0] + dy[0],
                joint.position[1] + dy[1],
                joint.position[2] + dy[2]]
        
            # assign parent
            if joint.parent != -1:
                bl_jnt.parent = bl_jntlist[joint.parent]

        # update scene
        bpy.context.scene.update()
        return True

    ##
    #
    def process_animations(self):

        # read number of animations
        n_anim = self.read_int()
        if n_anim < 0: raise Exception('Invalid number of animations.')

        # read animations
        self.animlist = []
        for i in range(n_anim):

            # read animation name
            anim = MeshUTFAnimation()
            anim.name = self.read_string()
            if len(anim.name) == 0: raise Exception('Invalid animation name.')

            # read number of keyframed joints
            n_jnts = self.read_int()
            if n_jnts < 0: raise Exception('Invalid number of keyframed joints.')

            # read keyframed joints
            anim.jointmap = {}
            for j in range(n_jnts):

                # read joint name
                jntname = self.read_string()
                if len(jntname) == 0: raise Exception('Invalid joint name.')

                # read number of keyframes
                n_keys = self.read_int()
                if n_keys < 1: raise Exception('Invalid number of keyframes for joint[' + jntname + '] in animation[' + anim.name + '].')

                # read keyframes
                kfdict = {}
                for k in range(n_keys):

                    # read frame index
                    frame = self.read_int()
                    if frame < 0: raise Exception('Invalid keyframe for joint[' + jntname + '] in animation[' + anim.name + '].')

                    # frame must not already exist
                    if frame in kfdict: raise Exception('Keyframe index {} appears more than once for joint[{}] in animation[{}].'.format(frame, jntname, anim.name))

                    # read position, rotation and scale
                    kf = MeshUTFAnimationKeyframe()
                    kf.position = self.read_vector3()
                    kf.rotation = self.read_vector4()
                    kf.scale = self.read_vector3()

                    # insert keyframe into keyframe dictionary kfdict[timeindex] = keyframe data
                    kfdict[frame] = kf

                # insert keyframe dictionary into joint map
                anim.jointmap[jntname] = kfdict

            # insert animation
            self.animlist.append(anim)

        # build animations in Blender
        return self.build_animations()

    ##
    #
    def build_animations(self):
    
        # for each animation
        for anim in self.animlist:

            # create action object
            action = bpy.data.actions.new(anim.name)

            # for each joint there is a keyframe dictionary
            for jname, kfd in anim.jointmap.items():

                # create F-curves for each bone
                lcurve = [action.fcurves.new('pose.bones[\"{}\"].location'.format(jname), i) for i in range(3)]
                qcurve = [action.fcurves.new('pose.bones[\"{}\"].rotation_quaternion'.format(jname), i) for i in range(4)]
                scurve = [action.fcurves.new('pose.bones[\"{}\"].scale'.format(jname), i) for i in range(3)]

                # assign keyframes
                for frame, keyframe in kfd.items():

                    # location
                    lcurve[0].keyframe_points.insert(frame, keyframe.position[0], {'FAST'})
                    lcurve[1].keyframe_points.insert(frame, keyframe.position[1], {'FAST'})
                    lcurve[2].keyframe_points.insert(frame, keyframe.position[2], {'FAST'})

                    # rotation_quaternion
                    qcurve[0].keyframe_points.insert(frame, keyframe.rotation[0], {'FAST'})
                    qcurve[1].keyframe_points.insert(frame, keyframe.rotation[1], {'FAST'})
                    qcurve[2].keyframe_points.insert(frame, keyframe.rotation[2], {'FAST'})
                    qcurve[3].keyframe_points.insert(frame, keyframe.rotation[3], {'FAST'})

                    # scale
                    scurve[0].keyframe_points.insert(frame, keyframe.scale[0], {'FAST'})
                    scurve[1].keyframe_points.insert(frame, keyframe.scale[1], {'FAST'})
                    scurve[2].keyframe_points.insert(frame, keyframe.scale[2], {'FAST'})

        # update scene
        bpy.context.scene.update()
        return True

    ##
    #        
    def process_collision_meshes(self):

        # read number of collision meshes
        n_mesh = self.read_int()
        if n_mesh < 0: raise Exception('Invalid number of collision meshes.')

        # read collision mesh
        self.colllist = []
        for i in range(n_mesh):

            # read number of vertices
            n_verts = self.read_int()
            if n_verts < 1: raise Exception('Invalid number of collision mesh vertices.')

            # read vertices
            verts = []
            for j in range(n_verts): verts.append(self.read_vector3())

            # read number of faces
            n_faces = self.read_int()
            if n_faces < 1: raise Exception('Invalid number of collision mesh faces.')

            # read faces
            faces = []
            for j in range(n_faces): faces.append(self.read_face3())

            # append collision mesh
            mesh = MeshUTFCollisionMesh()
            mesh.verts = verts
            mesh.faces = faces
            self.colllist.append(mesh)

        return self.build_collision_meshes()

    ##
    #
    def build_collision_meshes(self):

        # nothing to do
        n_mesh = len(self.colllist)
        if n_mesh == 0: return True

        # build collision meshes
        for i, cm in enumerate(self.colllist):

            # create mesh name
            name = 'collision'
            if n_mesh > 1: name = name + '_' + str(i)

            # create mesh object
            object = bpy.ops.object.add(type='MESH')
            object = bpy.context.active_object
            object.name = name
            object.data.name = name
            object.location = [0, 0, 0]
            object['entity_type'] = 'COLLISION_MESH'

            # bmesh representation
            bm = bmesh.new()
            bm.from_mesh(object.data)

            # add vertices
            for v in cm.verts: bm.verts.new(v)
            bm.verts.ensure_lookup_table()

            # add polygons
            for face in cm.faces: bm.faces.new([bm.verts[index] for index in face])

            # clean up and convert bmesh to mesh
            bm.to_mesh(object.data)
            bm.free()

        return True

    ##
    #
    def read_materials(self):

        # read number of materials
        n_mats = self.read_int()
        if n_mats < 0: raise Exception('Invalid number of materials.')

        # nothing to do
        if n_mats == 0: return True

        # for each material
        for i in range(n_mats):

            # read name
            material = MeshUTFMaterial()
            material.name = self.read_string()
            if len(material.name) == 0: raise Exception('Invalid material name.')

            # read material index
            material.index = self.read_int()
            if material.index < n_mats == False: raise Exception('Invalid material index.')

            # read number of textures
            n_textures = self.read_int()
            if n_textures < 0: raise Exception('Invalid number of textures.')

            # read textures
            material.textures = []
            for j in range(n_textures):

                # read texture name
                tobj = MeshUTFTexture()
                tobj.name = self.read_string()
                if len(tobj.name) == 0: raise Exception('Invalid texture name.')

                # read texture semantic
                tobj.semantic = self.read_string()
                if len(tobj.semantic) == 0: raise Exception('Invalid texture semantic.')

                # read UV channel
                tobj.uv_layer = self.read_int()
                if tobj.uv_layer < 0 or tobj.uv_layer > 3: raise Exception('Invalid UV layer index.')

                # read filename
                tobj.filename = self.read_string()
                if len(tobj.filename) == 0: raise Exception('Invalid texture filename.')

                # append texture
                material.textures.append(tobj)

            # append material
            self.mat_list.append(material)

        return True

    ##
    #
    def make_materials(self):

        # for each material
        for mat in self.mat_list:

            # create material object
            material = bpy.data.materials.new(mat.name)

            for i, tobj in enumerate(mat.textures):

                # create material texture slot
                slot = material.texture_slots.create(i)
                #slot.name = tobj.name
                slot.texture_coords = 'UV'
                slot.use = True

                # set semantic
                if tobj.semantic == 'diffuse':
                    slot.use_map_diffuse = True
                elif tobj.semantic == 'normal':
                    slot.use_map_normal = True
                elif tobj.semantic == 'specular':
                    slot.use_map_spec = True
                elif tobj.semantic == 'lightmap':
                    slot.use_map_ambient = True
                elif tobj.semantic == 'alpha':
                    slot.use_map_alpha = True
                elif tobj.semantic == 'emmision':
                    slot.use_map_emmision = True
                elif tobj.semantic == 'reflection':
                    slot.use_map_reflection = True

                # set UV layer
                slot.uv_layer = 'uvchannel_{}'.format(tobj.uv_layer)

                # set filename (must create texture and image objects first)
                # if we can't set the filename, just load the data as custom properties
                slot.texture = bpy.data.textures.new(tobj.name, 'IMAGE')
                try:
                    slot.texture.image = bpy.data.images.load(tobj.filename)
                except:
                    slot.texture['filename'] = tobj.filename
                    slot.texture['semantic'] = tobj.semantic

        return True

    ##
    #
    def read_meshes(self):

        # read number of meshes
        n_mesh = self.read_int()
        if n_mesh < 0: raise Exception('Invalid number of meshes.')

        # read meshes
        self.meshlist = []
        for i in range(n_mesh):

            # read mesh name
            name = self.read_string()
            if len(name) == 0: raise Exception('Invalid mesh name.')

            # read number of vertices
            n_verts = self.read_int()
            if n_verts < 1: raise Exception('Invalid number of mesh vertices.')

            # read number of UV channels
            n_uvs = self.read_int()
            if n_uvs < 1 or n_uvs > 4: raise Exception('Invalid number of mesh UV channels.')

            # read number of color channels
            n_colors = self.read_int()
            if n_colors < 0 or n_colors > 4: raise Exception('Invalid number of mesh color channels.')

            # initialize buffers
            verts = []
            normals = []
            bw = []
            bi = []
            uvs = []
            colors = []
            for j in range(n_uvs): uvs.append([])
            for j in range(n_colors): colors.append([])

            # read vertices
            for j in range(n_verts):

                # read vertex position
                verts.append(self.read_vector3())

                # read vertex normal
                normals.append(self.read_vector3())

                # read UV channels
                for k in range(n_uvs): uvs[k].append(self.read_vector2())

                # read blendindices and blendweights
                if self.skeleton.n_jnts > 0:

                   bw.append(self.read_blendindices())
                   bw.append(self.read_blendweights())

                # read color channels
                for k in range(n_colors): colors[k].append(self.read_vector3())

            # read number of total faces
            n_faces = self.read_int()
            if n_faces < 0: raise Exception('Invalid number of total mesh faces.')

            # read number of surfaces
            n_surfaces = self.read_int()
            if n_surfaces < 0: raise Exception('Invalid number of mesh surfaces.')

            # read surfaces (submeshes)
            surfaces = []
            total_faces = 0
            for i in range(n_surfaces):

                # read number of faces
                surf = MeshUTFSurface()
                surf.n_faces = self.read_int()
                if surf.n_faces < 1: raise Exception('Invalid number of mesh faces.')

                # read material index
                surf.material = self.read_int()
                if surf.material < len(self.mat_list) == False: raise Exception('Invalid material reference {} in mesh {}.'.format(surf.material, name))

                # read faces
                surf.facelist = []
                for j in range(surf.n_faces): surf.facelist.append(self.read_face3())
                total_faces = total_faces + surf.n_faces

                # append surface
                surfaces.append(surf)

            # total faces must match
            # technically, we could just set n_faces = total_faces in case of a mismatch, but it's better to flag this as an error
            if total_faces != n_faces:
                raise Exception('Mismatching number of faces {} (total) versus {} (submesh total) in mesh {}.'.format(n_faces, total_faces, name))

            # append collision mesh
            mesh = MeshUTFMesh()
            mesh.name = name
            mesh.n_verts = n_verts
            mesh.n_uvs = n_uvs
            mesh.n_colors = n_colors
            mesh.verts = verts
            mesh.normals = normals
            mesh.bw = bw
            mesh.bi = bi
            mesh.uvs = uvs
            mesh.colors = colors
            mesh.n_faces = n_faces
            mesh.surfaces = surfaces
            self.meshlist.append(mesh)

        return True

    ##
    #
    def make_meshes(self):

        # for each mesh
        for mesh in self.meshlist:

            # create mesh object
            object = bpy.ops.object.add(type='MESH')
            object = bpy.context.active_object
            object.name = mesh.name
            object.data.name = mesh.name
            object.location = [0, 0, 0]

            # bmesh representation
            bm = bmesh.new()
            bm.from_mesh(object.data)

            # add vertices
            for v in mesh.verts: bm.verts.new(v)
            bm.verts.ensure_lookup_table()

            # for each surface
            for surface in mesh.surfaces:

                for face in surface.facelist:

                    bm.faces.new([bm.verts[index] for index in face])

            # clean up and convert bmesh to mesh
            bm.to_mesh(object.data)
            bm.free()

            # use custom normals
            bpy.ops.mesh.customdata_custom_splitnormals_add()
            object.data.normals_split_custom_set_from_vertices(mesh.normals)
            object.data.use_auto_smooth = True

        return True

class ImportMeshUTFOperator(bpy.types.Operator):

    ##
    #  @brief 
    #  @warning Do not use 'import' as it is a keyword in python. Use 'import_mesh' instead. Using
    #  bpy.ops.import.mesh_utf will cause an error while bpy.ops.import_mesh.mesh_utf will not.
    bl_idname = 'import_mesh.mesh_utf'
    
    ##
    #  @brief Required member for fileselect_add to label button in selection dialog.
    bl_label = 'Import'
    
    ##
    #  @brief Required member for fileselect_add to store filename from selection dialog.
    filepath = bpy.props.StringProperty(subtype='FILE_PATH')
    
    ##
    #  @brief Defines conditions for which menu item should be enabled. True = always enabled.
    @classmethod
    def poll(cls, context): return True
    
    ##
    #  @brief Defines what happens immediately after menu item is selected.
    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'} # stay open and wait until user clicks 'Import' or 'Cancel'

    ##
    #  @brief Called by the fileselect_add dialog after the user clicks the 'Import' button.
    def execute(self, context):
        obj = MeshUTFImporter()
        obj.execute(self.filepath)
        return {'FINISHED'}

##
#  @brief Defines what happens when File > Import > Mesh UTF Import is selected from the main menu.
def ImportMeshUTF_MenuFunction(self, context):
    self.layout.operator_context = 'INVOKE_DEFAULT'
    self.layout.operator(ImportMeshUTFOperator.bl_idname, text='Mesh UTF Import (.txt)')

# UNCOMMENT IF PLUGIN
# ##
# def register():
#     bpy.utils.register_module(__name__)
#     bpy.types.INFO_MT_file_import.append(ImportMeshUTF_MenuFunction)
# 
# ##
# def unregister():
#     bpy.utils.unregister_module(__name__)
#     bpy.types.INFO_MT_file_import.remove(ImportMeshUTF_MenuFunction)
#     
# ##
# if __name__ == '__main__':
#     register()

# UNCOMMENT IF OPERATOR
#
# # register operator and add to the main menu
# bpy.utils.register_class(ImportMeshUTFOperator)
# bpy.types.INFO_MT_file_import.append(ImportMeshUTF_MenuFunction)
    
# UNCOMMENT IF IN PANEL

# register operator and test call
bpy.utils.register_class(ImportMeshUTFOperator)
bpy.ops.import_mesh.mesh_utf('INVOKE_DEFAULT')
    