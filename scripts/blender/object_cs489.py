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

bl_info = {
'name': 'CS489 Plugins',
'author': 'Steven Emory',
'version': (1, 0),
'blender': (2, 79, 0),
'location': 'Object > CS489',
'description': 'CS489 menuitems',
'category': 'Object',
}

class InsertDCPropsOperator(bpy.types.Operator):

    bl_idname = 'cs489.insert_dc_props'
    bl_label = 'CS489'
    
    # menuitem is always enabled
    @classmethod
    def poll(cls, context): return True
    
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)

    # called by invoke
    def execute(self, context):
        # obj = WorldUTFExporter()
        # obj.execute()
        return {'FINISHED'}

class CS489PopupMenu(bpy.types.Menu):

    bl_idname = 'cs489.popup_menu'
    bl_label = 'CS489'
    
    def draw(self, context):
        self.layout.operator(InsertDCPropsOperator.bl_idname, text='Insert Door Controller Properties1')
        self.layout.separator()
        self.layout.operator(InsertDCPropsOperator.bl_idname, text='Insert Door Controller Properties2')

        
# define menuitem
def InsertDCProps_MenuFunction(self, context):
    self.layout.operator_context = 'INVOKE_DEFAULT'
    self.layout.menu(CS489PopupMenu.bl_idname)
    self.layout.separator()
   
#region

##
def register():
    bpy.utils.register_module(__name__)
    bpy.types.VIEW3D_MT_object.prepend(InsertDCProps_MenuFunction)

##
def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.VIEW3D_MT_object.remove(InsertDCProps_MenuFunction)
    
##
if __name__ == '__main__':
    register()

#endregion