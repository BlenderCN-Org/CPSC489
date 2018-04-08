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

#
# PYTHON IMPORTS
#
import os
import re

#
# BLENDER IMPORTS
#
import bpy
import mathutils

##
#  @brief   Required for all plugins.
#  @details A descriptor dictionary that is used by Blender to display information about the plugin,
#  such as the name, author, and version of the plugin.
#
bl_info = {
'name': 'Export World UTF',
'author': 'Steven Emory',
'version': (1, 0),
'blender': (2, 79, 0),
'location': 'File > Export > World UTF (.txt)',
'description': 'Exports a World UTF file.',
'warning': '',
'category': 'Import-Export',
}

##
#  @brief 
#  @details
class WorldUTFExporter:

    export_path = None
    export_name = None
    export_fext = None
    filename = None
    file = None

    ##
    #  @brief
    #  @details 
    def __init__(self):
        pass

    ##
    #  @brief
    #  @details 
    def execute(self):

        # build export path
        if 'export_path' in bpy.context.scene: self.export_path = bpy.context.scene['export_path']
        else: self.export_path = os.path.dirname(bpy.data.filepath) + '\\'
        print(self.export_path)

        # build export name
        if 'export_name' in bpy.context.scene: self.export_name = bpy.context.scene['export_name']
        else: self.export_name = os.path.splitext(os.path.basename(bpy.data.filepath))[0]
        print(self.export_name)

        # build export file extension
        if 'export_fext' in bpy.context.scene: self.export_fext = bpy.context.scene['export_fext']
        else: self.export_fext = 'txt'
        print(self.export_fext)

        # create file for writing
        self.filename = self.export_path + self.export_name + '.' + self.export_fext
        print(self.filename)
        self.file = open(self.filename, 'w')

        # export camera markers
        self.ExportCameraMarkers()

    ##
    #  @brief
    #  @details 
    def ExportCameraMarkers(self):

        pass

##
#  @brief   ExportWorldUTF Blender operator.
#  @details Defines the operation(s) to be performed when menu item in Blender is chosen. This
#  operator invokes the file manager and executes an export operation on the chosen filename.
#
class ExportWorldUTFOperator(bpy.types.Operator):

    ##
    #  @brief   bpy.ops.bl_idname identfier.
    #  @details The name of the bpy.ops identifier that is used to invoke this script plugin from
    #  within Blender.
    bl_idname = 'export_world.world_utf'
    
    ##
    #  @brief   Selection dialog label.
    #  @details Required member for fileselect_add to label button in selection dialog.
    bl_label = 'Export'
    
    ##
    #  @brief   Menu item enabler/disabler.
    #  @details Defines conditions for which menu item should be enabled. True = always enabled.
    @classmethod
    def poll(cls, context): return True
    
    ##
    #  @brief   Invoke plugin.
    #  @details Defines what happens immediately after menu item is selected and plugin enters the
    #  invoked state. Simply just calls execute.
    def invoke(self, context, event):
        return self.execute(context)

    ##
    #  @brief   Execute export code.
    #  @details Called by invoke to execute export code.
    def execute(self, context):
        obj = WorldUTFExporter()
        obj.execute()
        return {'FINISHED'}

##
#  @brief   Menu command function.
#  @details Defines what happens when File > Export > World UTF is selected from the main menu.
def ExportWorldUTF_MenuFunction(self, context):
    self.layout.operator_context = 'INVOKE_DEFAULT'
    self.layout.operator(ExportWorldUTFOperator.bl_idname, text='World UTF (.txt)')

# UNCOMMENT REGION IF PLUGIN
# region

# ##
# def register():
#     bpy.utils.register_module(__name__)
#     bpy.types.INFO_MT_file_export.append(ExportWorldUTF_MenuFunction)
# 
# ##
# def unregister():
#     bpy.utils.unregister_module(__name__)
#     bpy.types.INFO_MT_file_export.remove(ExportWorldUTF_MenuFunction)
#     
# ##
# if __name__ == '__main__':
#     register()

# endregion

# UNCOMMENT REGION IF OPERATOR
# region

# # register operator and add to the main menu
# bpy.utils.register_class(ExportWorldUTFOperator)
# bpy.types.INFO_MT_file_export.append(ExportWorldUTF_MenuFunction)

# endregion
    
# UNCOMMENT REGION IF IN PANEL
# region

# register operator and test call
bpy.utils.register_class(ExportWorldUTFOperator)
bpy.ops.export_world.world_utf('INVOKE_DEFAULT')

# endregion