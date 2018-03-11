import bpy
import os
import re

class Bone:
    # self.name
    # self.parent
    # self.position[3]
    # self.matrix[3][3]
    pass

def MeshExporter():
    
    if bpy.context.mode != 'OBJECT':
        raise Exception("You must be in OBJECT mode to run this script.")

    if len(bpy.data.meshes) == 0:
        raise Exception('There are no meshes to export.')
            
    # create file
    filename = os.path.splitext(bpy.data.filepath)[0] + ".txt"
    file = open(filename, 'w')
    print("Saving file to " + filename + ".")

    # export skeleton
    has_skeleton = False
    armature = None
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE':
            has_skeleton = True
            armature = obj
            ExportArmature(file, armature, armature.data)
            break
        
    # no skeleton
    if has_skeleton == False:
        file.write("0 # number of bones\n")
        
    # export animations
    if has_skeleton == True:
        ExportAnimations(file, armature)
    else:
        file.write('0 # number of animations\n')

    # save number of meshes
    # TODO: do not rely on len(bpy.data.meshes) to return the correct number
    # of meshes. if you delete a few meshes, they will still show up!!!
    file.write("{} # number of meshes\n".format(len(bpy.data.meshes)))
            
    # export meshes
    for obj in bpy.data.objects:
         if obj.type == 'MESH':
            ExportMesh(file, obj, obj.data, armature)
    
    #close file
    file.close()

def ExportArmature(file, armature, skeleton):

        # save number of bones    
        n_bones = len(skeleton.bones)
        file.write("{} # number of bones\n".format(n_bones))
        if n_bones == 0: return

        # PHASE 1        
        # associate name with an index
        indexmap = {}
        for index, bone in enumerate(skeleton.bones):
            indexmap[bone.name] = index

        dx = 0 #armature.location[0]
        dy = 0 #armature.location[1]
        dz = 0 #armature.location[2]
        
        # PHASE 2
        # construct bonelist
        bonelist = []
        for index, bone in enumerate(skeleton.bones):
            bonelist.append(Bone())
            bonelist[index].name = bone.name
            bonelist[index].parent = -1
            if bone.parent != None: bonelist[index].parent = indexmap[bone.parent.name]
            bonelist[index].position = [
                dx + bone.head_local[0],
                dy + bone.head_local[1],
                dz + bone.head_local[2]]
            bonelist[index].matrix = [
                [bone.matrix_local[0][0], bone.matrix_local[0][1], bone.matrix_local[0][2]],
                [bone.matrix_local[1][0], bone.matrix_local[1][1], bone.matrix_local[1][2]],
                [bone.matrix_local[2][0], bone.matrix_local[2][1], bone.matrix_local[2][2]]]

            print(bone.matrix_local)
            print(bone.matrix)
                
        # PHASE 3
        # save bone data
        for bone in bonelist:
            file.write(bone.name + "\n")
            file.write("{}\n".format(bone.parent))
            file.write("{} {} {}\n".format(bone.position[0], bone.position[1], bone.position[2]))
            file.write("{} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}\n".format(
                bone.matrix[0][0], bone.matrix[0][1], bone.matrix[0][2], 0.0,
                bone.matrix[1][0], bone.matrix[1][1], bone.matrix[1][2], 0.0,
                bone.matrix[2][0], bone.matrix[2][1], bone.matrix[2][2], 0.0,
                0.0, 0.0, 0.0, 1.0))   

def ExportAnimations(file, armature):

    # number of actions
    n_actions = len(bpy.data.actions)
    if n_actions == 0: pass
    file.write('{} # number of animations\n'.format(n_actions))
        
    # export animations
    for i in bpy.data.actions:
        
        # map to associate bone with key
        bonemap = {}
        for bone in armature.data.bones: bonemap[bone.name] = {}
        
        # for each curve
        for fcu in i.fcurves:
            
            # extract bone name and key type
            pattern = r'pose\.bones\[\"(.*)\"\]\.(.*)'
            str = fcu.data_path
            print(fcu.data_path)
            m = re.match(pattern, str)
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
                    keydict[frame] = [
                        [0.0, 0.0, 0.0],      # position
                        [1.0, 0.0, 0.0, 0.0], # quaternion
                        [1.0, 1.0, 1.0]]      # scale
                
                # assign values
                if g2 == 'location':
                    if (fcu.array_index >= 0 and fcu.array_index <= 2):
                        keydict[frame][0][fcu.array_index] = value
                if g2 == 'rotation_euler':
                    pass
                if g2 == 'rotation_quaternion':
                    if (fcu.array_index >= 0 and fcu.array_index <= 4):
                        keydict[frame][1][fcu.array_index] = value
                if g2 == 'scale':
                    if (fcu.array_index >= 0 and fcu.array_index <= 2):
                        keydict[frame][2][fcu.array_index] = value

        # pre-iterate through <bone, keys> dictionary
        n_keyable = 0
        for name, keydict in bonemap.items():
            n_keys = len(keydict)
            if n_keys > 0: n_keyable = n_keyable + 1
            
        # print animation
        file.write(i.name + '\n')
        file.write('{}'.format(n_keyable) + ' # number of keyframed bones\n')

        dx = 0 #armature.location[0]
        dy = 0 #armature.location[1]
        dz = 0 #armature.location[2]

        # iterate through <bone, keys> dictionary
        for name, keydict in bonemap.items():
            n_keys = len(keydict)
            if n_keys > 0:
                file.write(name + '\n')
                file.write('{}'.format(len(keydict)) + ' # number of keys\n')
                for frame, transforms in sorted(keydict.items()):
                    file.write('{}\n'.format(int(frame)))
                    file.write('{} {} {}\n'.format(transforms[0][0] + dx, transforms[0][1] + dy, transforms[0][2] + dz))
                    file.write('{} {} {} {}\n'.format(transforms[1][0], transforms[1][1], transforms[1][2], transforms[1][3]))
                    file.write('{} {} {}\n'.format(transforms[2][0], transforms[2][1], transforms[2][2]))

def ExportMesh(file, obj, mesh, armature):
    
    # number of verts, texture channels, materials, and color channels
    n_verts = len(mesh.vertices)
    n_channels = len(mesh.uv_layers)
    n_material = len(mesh.materials)
    n_colors = len(mesh.vertex_colors)
    
    if n_material == 0:
        pass
    elif n_material == 1:
        pass
    else:
        raise Exception('Only one material per mesh is supported.')

    file.write(obj.name + "\n")
    file.write("{} # number of vertices\n".format(n_verts))
    file.write("{} # number of UV channels\n".format(n_channels))
    file.write("{} # number of color channels\n".format(n_colors))
     
    # save material
    if n_material == 1:

        # compute number of used texture slots
        slots_used = 0
        for slot in mesh.materials[0].texture_slots:
            if slot != None and slot.use == True:
                slots_used = slots_used + 1

        # save number of textures
        file.write("{} # number of textures\n".format(slots_used))
            
        # iterate through all texture slots
        for slot in mesh.materials[0].texture_slots:
            
            # texture slot is used    
            if slot != None and slot.use == True:
                
                # must use UV coordinates
                if slot.texture_coords != 'UV':
                    raise 'Materials must refer to UV texture coordinates.'
                
                # must use IMAGE textures only
                if slot.texture.type != 'IMAGE':
                    raise Exception('This material uses a non-image texture.')
                    
                # get UV channel index from slot name  
                channel = -1
                for index, uv_layer in enumerate(mesh.uv_layers):
                    if uv_layer.name == slot.uv_layer:
                        channel = index;
                if channel == -1:
                    raise Exception('This material refers to a UV map that doesn\'t exist.')
                
                # write data
                if slot.texture.image != None:
                    file.write(slot.name + "\n" + str(channel) + "\n" + slot.texture.image.filepath + "\n")
                else:
                    file.write(slot.name + "\n" + str(channel) + "\n" + "default.bmp" + "\n")

    # do we have bones?
    has_bones = ((armature != None) and (len(armature.data.bones) > 0))
                        
    # associate each bone name with an index
    # this is used to map vertex groups to bones
    bonemap = {}
    if has_bones == True:
        for index, bone in enumerate(armature.data.bones): bonemap[bone.name] = index
    
    # dictionary<vertex group index, vertex group name>
    vgdict = {}
    for vg in obj.vertex_groups:
        vgdict[vg.index] = vg.name

    # initialize vertex buffer (UVs done later)
    vbuffer1 = []
    vbuffer2 = []
    vbuffer3 = []
    vbuffer4 = []
    vbuffer5 = []
    vbuffer6 = []
    vbuffer7 = []
    vbuffer8 = []
    for i in range(len(mesh.vertices)):
        
        # multiply vertices by matrix_world to get them in world coordinates
        v = mesh.vertices[i]
        temp = obj.matrix_world * v.co
        temp[0] = temp[0]
        temp[1] = temp[1]
        temp[2] = temp[2]
        vbuffer1.append(temp)
        
        # normal
        vbuffer2.append(v.normal)
        
        # uv
        vbuffer3.append([0.0, 0.0])
        vbuffer4.append([0.0, 0.0])

        # if there are bones, assign blendindices and blendweights
        if has_bones == True:

            bi = [0, 0, 0, 0]
            bw = [0.0, 0.0, 0.0, 0.0]
            
            # get list of (vertex group index, weight)
            vglist = v.groups
            if len(vglist) > 4: raise Exception('Cannot have more than four weights per vertex.')
            
            # for each (vertex group index, weight)
            for i, vg in enumerate(vglist):
                if vg.group not in vgdict:
                    raise Exception('The vertex group ({}) was not found in vertex group dictionary.'.format(vg.group))
                vg_name = vgdict[vg.group]
                if vg_name not in bonemap:
                    raise Exception('The vertex group name ({}) was not found in bone dictionary'.format(vg_name))
                bone_index = bonemap[vg_name]
                bi[i] = bone_index
                bw[i] = vg.weight

            # insert values
            vbuffer5.append(bi)
            vbuffer6.append(bw)
        
        # color
        vbuffer7.append([0.0, 0.0, 0.0, 1.0])
        vbuffer8.append([0.0, 0.0, 0.0, 1.0])
        
    # for each polygon
    facelist = []
    for poly in mesh.polygons:

        # must have 3 vertices in polygon loop
        if poly.loop_total != 3:
            raise RuntimeError('Mesh geometry contains non-triangles.')
            
        # for each vertex in polygon loop
        face = []
        for loop_index in poly.loop_indices:
            
            vindex = mesh.loops[loop_index].vertex_index
            face.append(vindex)
            
            if n_channels > 0:
                data = mesh.uv_layers[0].data[loop_index]
                vbuffer3[vindex] = data.uv
            
            if n_channels > 1:
                data = mesh.uv_layers[1].data[loop_index]
                vbuffer4[vindex] = data.uv

            if n_colors > 0:
                data = mesh.vertex_colors[0].data[loop_index]
                vbuffer7[vindex][0] = data.color[0]
                vbuffer7[vindex][1] = data.color[1]
                vbuffer7[vindex][2] = data.color[2]
                
            if n_colors > 1:
                data = mesh.vertex_colors[1].data[loop_index]
                vbuffer8[vindex][0] = data.color[0]
                vbuffer8[vindex][1] = data.color[1]
                vbuffer8[vindex][2] = data.color[2]
    
        facelist.append(face)
                    
    # save vertex buffer
    for i in range(len(vbuffer1)): 
        file.write("{:.4f} {:.4f} {:.4f}\n".format(vbuffer1[i][0], vbuffer1[i][1], vbuffer1[i][2]))
        file.write("{:.4f} {:.4f} {:.4f}\n".format(vbuffer2[i][0], vbuffer2[i][1], vbuffer2[i][2]))    
        if n_channels > 0: file.write("{:.4f} {:.4f}\n".format(vbuffer3[i][0], vbuffer3[i][1]))  
        if n_channels > 1: file.write("{:.4f} {:.4f}\n".format(vbuffer4[i][0], vbuffer4[i][1]))
        if has_bones == True:
            file.write("{} {} {} {}\n".format(vbuffer5[i][0], vbuffer5[i][1], vbuffer5[i][2], vbuffer5[i][3]))  
            file.write("{:.4f} {:.4f} {:.4f} {:.4f}\n".format(vbuffer6[i][0], vbuffer6[i][1], vbuffer6[i][2], vbuffer6[i][3]))
        if n_colors > 0: file.write("{:.4f} {:.4f} {:.4f} {:.4f}\n".format(vbuffer7[i][0], vbuffer7[i][1], vbuffer7[i][2], vbuffer7[i][3]))  
        if n_colors > 1: file.write("{:.4f} {:.4f} {:.4f} {:.4f}\n".format(vbuffer8[i][0], vbuffer8[i][1], vbuffer8[i][2], vbuffer8[i][3]))
    
    # save index buffer
    file.write("{} # of faces\n".format(len(facelist)))
    for face in facelist:
        if len(face) == 3:
            file.write("{} {} {}\n".format(face[0], face[1], face[2]))
        
MeshExporter()