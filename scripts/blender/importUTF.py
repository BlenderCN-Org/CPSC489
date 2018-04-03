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

class MeshUTFSkeleton:
    # n_joints
    pass
    
class MeshUTFImporter:

    ##
    #
    def __init__(self):

        self.linelist = []
        self.linecurr = -1;
        
        self.skeleton = MeshUTFSkeleton()

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
    #
    def read_vector3(self):
    
        line = self.linelist[self.linecurr]
        self.linecurr = self.linecurr + 1
        parts = line.split(' ', 2)
        if len(parts) != 3: raise Exception('Expecting vector3.')        
        return [float(parts[0]), float(parts[1]), float(parts[2])]
        
    ##
    #
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
            float(parts[ 0]), float(parts[ 1]), float(parts[ 2]), float(parts[ 3]),
            float(parts[ 4]), float(parts[ 5]), float(parts[ 6]), float(parts[ 7]),
            float(parts[ 8]), float(parts[ 9]), float(parts[10]), float(parts[11]),
            float(parts[12]), float(parts[13]), float(parts[14]), float(parts[15])]
        
    ##
    #
    def execute(self, filename):

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
        if self.process_materials() == False: return
        if self.process_meshes() == False: return
        
        # build objects
        self.build_skeleton()
        

    ##
    #
    def process_bones(self):

        # temporary object
        temp = MeshUTFSkeleton()
        
        # read number of bones
        temp.n_jnts = self.read_int()
        if temp.n_jnts < 0: raise Exception('Invalid number of bones.')

        # read bone name
        temp.name = self.read_string()
        if len(temp.name) == 0: raise Exception('Invalid bone name.')
        
        # read parent ID
        temp.parent = self.read_int()
        if temp.parent >= temp.n_jnts: raise Exception('Invalid bone parent reference.')
        
        # read position and joint matrix
        temp.position = self.read_vector3()
        temp.m_abs = self.read_matrix4()
        
        # assign skeleton
        self.skeleton = temp
        return True
        
    ##
    #
    def process_animations(self):
        return True

    ##
    #        
    def process_collision_meshes(self):
        return True
        
    ##
    #
    def process_materials(self):
        return True

    ##
    #
    def process_meshes(self):
        return True

    ##
    #
    def build_skeleton(self):

        if self.skeleton.n_jnts == 0: return
        bpy.ops.object.armature_add()
        object = bpy.context.active_object
        object.name = 'skeleton'
        object.data.name = 'skeleton'
        
        bpy.ops.object.mode_set(mode='EDIT')
        #build_skeleton(layer_data, object.data.edit_bones)
        bpy.ops.object.mode_set(mode='OBJECT')
        
        pass
        
    ##
    #
    def build_animations(self):
    
        pass

    ##
    #
    def build_collision_meshes(self):
    
        pass

    ##
    #
    def build_materials(self):
    
        pass
        
    ##
    #
    def build_meshes(self):
    
        pass
        
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
    