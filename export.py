import bpy
import os

def MeshExporter():
    
    if bpy.context.mode != 'OBJECT':
        print("You must be in OBJECT mode to run this script.")
        return

    if len(bpy.data.meshes) == 0:
        raise 'There are no meshes to export.'
            
    # create file
    filename = os.path.splitext(bpy.data.filepath)[0] + ".txt"
    file = open(filename, 'w')
    print("Saving file to " + filename + ".")

    # export skeleton
    has_skeleton = False
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE':
            has_skeleton = True
            ExportArmature(file, obj, obj.data)
            break
        
    # no bones
    if has_skeleton == False:
        file.write("0 # number of bones")

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

    # save location of root node
    file.write("{} {} {}\n".format(armature.location[0], armature.location[1], armature.location[2]))
            
    for bone in skeleton.bones:
        file.write(bone.name + "\n")
        if bone.parent == None: file.write("\n")
        else: file.write("{}\n".format(bone.parent.name))
        #file.write("center {} {} {}\n".format(bone.center[0], bone.center[1], bone.center[2]))
        #file.write("vector {} {} {}\n".format(bone.vector[0], bone.vector[1], bone.vector[2]))
        #file.write("head {} {} {}\n".format(bone.head_local[0], bone.head_local[1], bone.head_local[2]))
        #file.write("tail {} {} {}\n".format(bone.tail_local[0], bone.tail_local[1], bone.tail_local[2]))
        #file.write("x-axis {} {} {}\n".format(bone.x_axis[0], bone.x_axis[1], bone.x_axis[2]))
        #file.write("y-axis {} {} {}\n".format(bone.y_axis[0], bone.y_axis[1], bone.y_axis[2]))
        #file.write("z-axis {} {} {}\n".format(bone.z_axis[0], bone.z_axis[1], bone.z_axis[2]))
        file.write("{} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}\n".format(
            bone.matrix_local[0][0], bone.matrix_local[0][1], bone.matrix_local[0][2], bone.matrix_local[0][3],
            bone.matrix_local[1][0], bone.matrix_local[1][1], bone.matrix_local[1][2], bone.matrix_local[1][3],
            bone.matrix_local[2][0], bone.matrix_local[2][1], bone.matrix_local[2][2], bone.matrix_local[2][3],
            bone.matrix_local[3][0], bone.matrix_local[3][1], bone.matrix_local[3][2], bone.matrix_local[3][3]))
        

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