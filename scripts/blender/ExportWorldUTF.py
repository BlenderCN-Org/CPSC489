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

class CameraMarker:
    # location          - vector3
    # orientation       - matrix4
    # index             - uint16
    # speed             - real32
    # interpolate_speed - bool
    # fovy              - real32
    # interpolate_fovy  - bool
    pass
class CameraAnimation:
    # location    - vector3
    # orientation - matrix4
    # start       - uint16
    # markers     - CameraMarker[]
    pass

##
#  @brief 
#  @details
class WorldUTFExporter:

    export_path = None
    export_name = None
    export_fext = None
    filename = None
    file = None

    # camera animations
    camera_animations = []

    ##
    #  @brief
    #  @details 
    def __init__(self):
        pass

    ##
    #  @brief
    #  @details 
    def WriteInt(self, v): file.write('{}\n'.format(int(v)))

    ##
    #  @brief
    #  @details 
    def WriteFloat(self, v): file.write('{}\n'.format(float(v)))

    ##
    #  @brief
    #  @details 
    def WriteString(self, s): file.write('{}\n'.format(str(s)))

    ##
    #  @brief
    #  @details 
    def WriteVector3(self, v): file.write('{} {} {}\n'.format(v[0], v[1], v[2]))

    ##
    #  @brief
    #  @details 
    def WriteMatrix4(self, m):
        file.write('{} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} \n'.format(
            m[ 0], m[ 1], m[ 2], m[ 3],
            m[ 4], m[ 5], m[ 6], m[ 7],
            m[ 8], m[ 9], m[10], m[11],
            m[12], m[13], m[14], m[15]))

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

        # examine objects
        prev_mode = bpy.context.mode
        if bpy.context.mode != 'OBJECT': bpy.ops.object.mode_set(mode='OBJECT')
        self.ExamineObjects()
        bpy.ops.object.mode_set(mode=prev_mode)

        # export camera markers
        self.ExportCameraMarkers()

    ##
    #  @brief
    #  @details 
    def ExamineObjects(self):

        for object in bpy.data.objects:

            print(object.name + ' - ' + object.type)
            if 'entity_type' in object:

                entity_type = object['entity_type']
                if entity_type == 'CAMERA_ANIMATION': self.ProcessCameraAnimation(object)
                elif entity_type == 'CAMERA_MARKER': pass

    ##
    #  @brief
    #  @details 
    def ProcessCameraAnimation(self, object):

        # must be a Plain Axes or Mesh Group object
        if object.type != 'EMPTY': raise Exception('Camera animation objects must be EMPTY Blender objects.')

        # nothing to do
        if len(object.children) == 0: return

        # initialize a camera animation object
        cao = CameraAnimation()
        cao.location = object.matrix_world * object.location
        cao.orientation = object.matrix_world * object.matrix_local
        cao.start = 0
        cao.markers = []

        # process children
        index = 0
        for child in object.children:

            # must be a Plain Axes or Mesh Group object
            if child.type != 'EMPTY': raise Exception('Camera markers must be EMPTY Blender objects.')

            # must be a CAMERA_MARKER entity
            if ('entity_type' not in child) or (child['entity_type'] != 'CAMERA_MARKER'):
                continue
 
            # initialize a camera animation object
            cmo = CameraMarker()
            cmo.location = child.matrix_world * child.location
            cmo.orientation = child.matrix_world * child.matrix_local
            cmo.index             = index
            cmo.speed             = 1.0
            cmo.interpolate_speed = True
            cmo.fovy              = 60.0
            cmo.interpolate_fovy  = True

            # read properties (if present)
            if 'index' in child: cmo.index = int(child['index'])
            if 'speed' in child: cmo.speed = float(child['speed'])
            if 'interpolate_speed' in child: cmo.interpolate_speed = bool(child['interpolate_speed'])
            if 'fovy' in child: cmo.fovy = child['fovy']
            if 'interpolate_fovy' in child: cmo.interpolate_fovy = child['interpolate_fovy']

            # validate properties
            

            # append marker object and increment index
            cao.markers.append(cmo)
            index = index + 1

        # append camera animation object only if there are markers
        if len(cao.markers): self.camera_animations.append(cao)

    ##
    #  @brief
    #  @details 
    def ExportCameraMarkers(self):

        self.file.write('\n')
        self.file.write('# CAMERA ANIMATIONS\n')
        self.file.write('\n')

        # save number of camera animations
        n_anim = len(self.camera_animations)
        self.file.write('{} # number of camera animations\n'.format(n_anim))

        # save camera animations
        for cao in self.camera_animations:

            # save camera animation properties
            self.WriteVector3(cao.location)
            self.WriteMatrix4(cao.orientation)
            self.WriteInt(cao.start)
            self.WriteString('{} # of markers'.format(len(cao.markers)))
            # save camera markers
            for marker in cao.markers:
                self.WriteVector3(marker.location)
                self.WriteMatrix4(marker.orientation)
                self.WriteInt(marker.index)
                self.WriteFloat(marker.speed)
                self.WriteInt(marker.interpolate_speed)
                self.WriteFloat(marker.fovy)
                self.WriteInt(marker.interpolate_fovy)


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