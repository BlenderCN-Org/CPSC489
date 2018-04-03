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

# 'panel', 'operator' or 'plugin'
scriptType = 'panel'

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

class MeshUTFImporter:
    
    # 
    
    def __init__(self, filepath):
        self.filename = filepath

    def execute():
        file = open(self.filename, 'r')
    
class ImportMeshUTFOperator(bpy.types.Operator):
    #
    bl_idname = 'import.mesh_utf'
    
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
        obj = MeshUTFImporter(self.filepath)
        obj.execute()
        return {'FINISHED'}

##
#  @brief Defines what happens when File > Import > Mesh UTF Import is selected from the main menu.
def ImportMeshUTF_MenuFunction(self, context):
    self.layout.operator_context = 'INVOKE_DEFAULT'
    self.layout.operator(ImportMeshUTFOperator.bl_idname, text='Mesh UTF Import (.txt)')
        
#
if scriptType == 'plugin':

    ##
    def register():
        bpy.utils.register_module(__name__)
        bpy.types.INFO_MT_file_import.append(ImportMeshUTF_MenuFunction)

    ##
    def unregister():
        bpy.utils.unregister_module(__name__)
        bpy.types.INFO_MT_file_import.remove(ImportMeshUTF_MenuFunction)

    ##
    if __name__ == '__main__':
        register()
#
elif scriptType == 'operator':

    # register operator and add to the main menu
    bpy.utils.register_class(ImportMeshUTFOperator)
    bpy.types.INFO_MT_file_import.append(ImportMeshUTF_MenuFunction)
    
#
elif scriptType == 'panel':

    # register operator and test call
    bpy.utils.register_class(ImportMeshUTFOperator)
    bpy.ops.import.mesh_utf('INVOKE_DEFAULT')



        