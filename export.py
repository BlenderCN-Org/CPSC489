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
        print("You must be in OBJECT mode to run this script.")
        return

    #if len(bpy.data.meshes) == 0:
    #    raise 'There are no meshes to export.'
            
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
        file.write("0 # number of bones")
        
    # export animations
    if has_skeleton == True:
        ExportAnimations(file, armature)

    # save number of meshes
    file.write("{} # number of meshes\n".format(len(bpy.data.meshes)))
            
    # export meshes
    for obj in bpy.data.objects:
         if obj.type == 'MESH':
            ExportMesh(file, obj.name, obj.data)
    
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
        
        # PHASE 2
        # construct bonelist
        bonelist = []
        for index, bone in enumerate(skeleton.bones):
            bonelist.append(Bone())
            bonelist[index].name = bone.name
            bonelist[index].parent = -1
            if bone.parent != None: bonelist[index].parent = indexmap[bone.parent.name]
            bonelist[index].position = [
                armature.location[0] + bone.head_local[0],
                armature.location[1] + bone.head_local[1],
                armature.location[2] + bone.head_local[2]]
            bonelist[index].matrix = [
                [bone.matrix_local[0][0], bone.matrix_local[0][1], bone.matrix_local[0][2]],
                [bone.matrix_local[1][0], bone.matrix_local[1][1], bone.matrix_local[1][2]],
                [bone.matrix_local[2][0], bone.matrix_local[2][1], bone.matrix_local[2][2]]]
                            
        # PHASE 3
        # save bone data
        for bone in bonelist:
            file.write(bone.name + "\n")
            file.write("{}\n".format(bone.parent))
            file.write("{} {} {}\n".format(bone.position[0], bone.position[1], bone.position[2]))
            file.write("{} {} {} {} {} {} {} {} {}\n".format(
                bone.matrix[0][0], bone.matrix[0][1], bone.matrix[0][2],
                bone.matrix[1][0], bone.matrix[1][1], bone.matrix[1][2],
                bone.matrix[2][0], bone.matrix[2][1], bone.matrix[2][2]))   

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
            if n_keys > 0:
                n_keyable = n_keyable + 1
                break
            
        # print animation
        file.write(i.name + '\n')
        file.write('{}'.format(n_keyable) + ' # number of keyframed bones\n')
        
        # iterate through <bone, keys> dictionary
        for name, keydict in bonemap.items():
            n_keys = len(keydict)
            if n_keys > 0:
                file.write(name + '\n')
                file.write('{}'.format(len(keydict)) + ' # number of keys\n')
                for frame, transforms in keydict.items():
                    file.write('{}\n'.format(int(frame)))
                    file.write('{} {} {}\n'.format(transforms[0][0], transforms[0][1], transforms[0][2]))
                    file.write('{} {} {} {}\n'.format(transforms[1][0], transforms[1][1], transforms[1][2], transforms[1][3]))
                    file.write('{} {} {}\n'.format(transforms[2][0], transforms[2][1], transforms[2][2]))

def ExportMesh(file, objectname, mesh):
    
    # save number of texture channels
    n_verts = len(mesh.vertices)
    n_channels = len(mesh.uv_layers)
    n_material = len(mesh.materials)
    
    if n_material == 0:
        pass
    elif n_material == 1:
        pass
    else:
        raise 'Only one material per mesh is supported.'
    
    file.write("mesh " + objectname + "\n")
    file.write("{} # number of vertices\n".format(n_verts))
    file.write("{} # number of UV channels\n".format(n_channels))
 
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
                    raise 'This material refers to a UV map that doesn\'t exist.'
                
                # write data
                file.write(slot.name + " " + str(channel) + " " + slot.texture.image.filepath + "\n")
                
    # initialize index buffer (UVs done later)
    vbuffer1 = []
    vbuffer2 = []
    vbuffer3 = []
    vbuffer4 = []
    for i in range(len(mesh.vertices)):
        v = mesh.vertices[i]
        vbuffer1.append(v.co)
        vbuffer2.append(v.normal)
        vbuffer3.append([0.0, 0.0])
        vbuffer4.append([0.0, 0.0])
            
    # for each polygon
    for poly in mesh.polygons:

        # must have 3 vertices in polygon loop
        if poly.loop_total != 3:
            raise RuntimeError('Mesh geometry contains non-triangles.')
            
        # for each vertex in polygon loop
        for loop_index in poly.loop_indices:
            
            vindex = mesh.loops[loop_index].vertex_index
            
            if n_channels > 0:
                data = mesh.uv_layers[0].data[loop_index]
                vbuffer3[vindex] = data.uv
            
            if n_channels > 1:
                data = mesh.uv_layers[1].data[loop_index]
                vbuffer4[vindex] = data.uv

    # save vertex buffer
    for i in range(len(vbuffer1)): 
        file.write("{:.4f} {:.4f} {:.4f}\n".format(vbuffer1[i][0], vbuffer1[i][1], vbuffer1[i][2]))
        file.write("{:.4f} {:.4f} {:.4f}\n".format(vbuffer2[i][0], vbuffer2[i][1], vbuffer2[i][2]))    
        if n_channels > 0: file.write("{:.4f} {:.4f}\n".format(vbuffer3[i][0], vbuffer3[i][1]))  
        if n_channels > 1: file.write("{:.4f} {:.4f}\n".format(vbuffer4[i][0], vbuffer4[i][1]))  
         
MeshExporter()