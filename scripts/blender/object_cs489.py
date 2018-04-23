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

#region PYTHON_IMPORTS

import os
import re

#endregion

#region BLENDER_IMPORTS

import bpy
import mathutils

#endregion

#region BLENDER_PLUGIN_DESCRIPTOR

bl_info = {
'name': 'CS489 Plugins',
'author': 'Steven Emory',
'version': (1, 0),
'blender': (2, 79, 0),
'location': 'Object > CS489',
'description': 'CS489 menuitems',
'category': 'Object',
}

#endregion

#region MODE_FUNCTIONS

def GetMode(self): return bpy.context.mode
def InObjectMode(self): return (bpy.context.mode == 'OBJECT')
def InEditArmatureMode(self): return (bpy.context.mode == 'EDIT_ARMATURE')
def InEditMeshMode(self): return (bpy.context.mode == 'EDIT_MESH')
def InPoseMode(self): return (bpy.context.mode == 'POSE')

#endregion

#region OBJECT_FUNCTIONS

def GetObjects(): return bpy.data.objects
def GetSelectedObjects():
    rv = []
    for obj in bpy.data.objects:
        if obj.select: rv.append(obj)
    return rv
def IsArmatureObject(obj): return (True if obj.type == 'ARMATURE' else False)
def IsEmptyObject(obj): return (True if obj.type == 'EMPTY' else False)
def IsMeshObject(obj): return (True if obj.type == 'MESH' else False)
def GetArmatureObjects():
    r = []
    for obj in GetObjects():
        if IsArmatureObject(obj): r.append(obj)
    return r
def GetSelectedArmatureObjects():
    r = []
    for obj in GetObjects():
        if (IsArmatureObject(obj) and obj.select): r.append(obj)
    return r
def GetMeshObjects():
    rv = []
    for obj in GetObjects():
        if IsMeshObject(obj) and IsMeshDataValid(obj.data): rv.append(obj)
    return rv
def GetSelectedMeshObjects():
    rv = []
    for obj in GetObjects():
        if IsMeshObject(obj) and IsMeshDataValid(obj.data) and obj.select: rv.append(obj)
    return rv

#endregion

#region PROPERTY_FUNCTIONS

def HasProperty(obj, prop):
    if obj is None: return False
    return (prop in obj)
def GetPropertyValue(obj, prop):
    if obj is None: return False
    if prop in obj: return obj[prop]
    return None
def SetPropertyValue(obj, prop, value):
    if obj is None: return
    obj[prop] = value
def InsertProperty(obj, prop, value):
    if obj is None: return
    if HasProperty(obj, prop): return
    SetPropertyValue(obj, prop, value)
def RemoveProperty(obj, prop):
    if obj is None: return
    if prop in obj: del obj[prop]

#endregion

#region ENTITY_FUNCTIONS

def IsEntity(obj, type):
    if obj is None: return False
    if 'entity_type' not in obj: return False
    if obj['entity_type'] == type: return True
    return False
def IsCameraAnimationList(obj): return IsEntity(obj, 'CAMERA_ANIMATION_LIST')
def IsCameraMarker(obj): return IsEntity(obj, 'CAMERA_MARKER')
def IsDoorControllerList(obj): return IsEntity(obj, 'DOOR_CONTROLLER_LIST')
def IsDoorController(obj): return IsEntity(obj, 'DOOR_CONTROLLER')
def IsMusicList(obj): return IsEntity(obj, 'MUSIC_LIST')
def IsMusicHolder(obj): return IsEntity(obj, 'MUSIC_HOLDER')

#endregion

#
# CAMERA OPERATORS
#
#region

class MarkCameraAnimationListOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.mark_camlist_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsEmptyObject(object):
                InsertProperty(object, 'entity_type', 'CAMERA_ANIMATION_LIST')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}

class UnmarkCameraAnimationListOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.unmark_camlist_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsCameraAnimationList(object):
                RemoveProperty(object, 'entity_type')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}

class MarkCameraMarkerOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.mark_cammark_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        index = 0
        for object in objlist:
            if IsEmptyObject(object):
                InsertProperty(object, 'entity_type', 'CAMERA_MARKER')
                InsertProperty(object, 'index', index)
                InsertProperty(object, 'speed', 1.0)
                InsertProperty(object, 'iterpolate_speed', True)
                InsertProperty(object, 'fovy', 60.0)
                InsertProperty(object, 'iterpolate_fovy', True)
                InsertProperty(object, 'stop', False)
                index = index + 1
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}

class UnmarkCameraMarkerOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.unmark_camark_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsCameraMarker(object):
                RemoveProperty(object, 'entity_type')
                RemoveProperty(object, 'index')
                RemoveProperty(object, 'speed')
                RemoveProperty(object, 'iterpolate_speed')
                RemoveProperty(object, 'fovy')
                RemoveProperty(object, 'iterpolate_fovy')
                RemoveProperty(object, 'stop')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}

#endregion

#
# ENTITY ANIMATION AND MARKER OPERATORS
#
#region

class MarkEntityAnimationListOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.mark_entity_anim_list_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsEmptyObject(object):
                InsertProperty(object, 'entity_type', 'ENTITY_MARKER_ANIMATION')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}

class UnmarkEntityAnimationListOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.unmark_entity_anim_list_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsCameraAnimationList(object):
                RemoveProperty(object, 'entity_type')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}

class MarkEntityMarkerOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.mark_entity_marker_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        index = 0
        time = 0.0
        for object in objlist:
            if IsEmptyObject(object):
                InsertProperty(object, 'entity_type', 'ENTITY_MARKER')
                InsertProperty(object, 'index', index)
                InsertProperty(object, 'speed', time)
                InsertProperty(object, 'iterpolate_speed', True)
                InsertProperty(object, 'anim', -1)
                InsertProperty(object, 'anim_loop', False)
                InsertProperty(object, 'sound', -1)
                InsertProperty(object, 'sound_loop', False)
                InsertProperty(object, 'stop', False)
                index = index + 1
                time = time + 1.0
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}

class UnmarkEntityMarkerOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.unmark_entity_marker_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsEntityMarker(object):
                RemoveProperty(object, 'entity_type')
                RemoveProperty(object, 'index')
                RemoveProperty(object, 'speed')
                RemoveProperty(object, 'iterpolate_speed')
                RemoveProperty(object, 'anim')
                RemoveProperty(object, 'anim_loop')
                RemoveProperty(object, 'sound')
                RemoveProperty(object, 'sound_loop')
                RemoveProperty(object, 'stop')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}

#endregion

#
# DOOR CONTROLLER OPERATORS
#
#region
class MarkDoorControllerListOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.mark_dclist_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsEmptyObject(object):
                InsertProperty(object, 'entity_type', 'DOOR_CONTROLLER_LIST')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}
class UnmarkDoorControllerListOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.unmark_dclist_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsDoorControllerList(object):
                RemoveProperty(object, 'entity_type')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}
class MarkDoorControllerOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.mark_dc_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsEmptyObject(object):
                InsertProperty(object, 'entity_type', 'DOOR_CONTROLLER')
                InsertProperty(object, 'door', '')
                InsertProperty(object, 'anim_idle', '')
                InsertProperty(object, 'anim_open', '')
                InsertProperty(object, 'anim_close', '')
                InsertProperty(object, 'sound_open', '')
                InsertProperty(object, 'sound_close', '')
                InsertProperty(object, 'stay_open', False)
                InsertProperty(object, 'close_timer', 5.0)
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}
class UnmarkDoorControllerOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.unmark_dc_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsDoorController(object):
                RemoveProperty(object, 'entity_type')
                RemoveProperty(object, 'door')
                RemoveProperty(object, 'anim_idle')
                RemoveProperty(object, 'anim_open')
                RemoveProperty(object, 'anim_close')
                RemoveProperty(object, 'sound_open')
                RemoveProperty(object, 'sound_close')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}
#endregion

#
# SOUND AND MUSIC OPERATORS
#
#region
class MarkMusicListOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.mark_music_list_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsEmptyObject(object):
                InsertProperty(object, 'entity_type', 'MUSIC_LIST')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}
class UnmarkMusicListOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.unmark_music_list_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsMusicList(object):
                RemoveProperty(object, 'entity_type')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}
class MarkMusicHolderOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.mark_music_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        #
        objlist = GetSelectedObjects()
        index = 0
        time = 0.0
        for object in objlist:
            if IsEmptyObject(object):
                InsertProperty(object, 'entity_type', 'MUSIC_HOLDER')
                InsertProperty(object, 'repeat', True)
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}
class UnmarkMusicHolderOp(bpy.types.Operator):
    #
    bl_idname = 'cs489.unmark_music_op'
    bl_label = 'CS489'
    # menuitem is enabled only if objects are selected
    @classmethod
    def poll(cls, context):
        objlist = GetSelectedObjects()
        return True if len(objlist) > 0 else False
    # menuitem is invoked
    def invoke(self, context, event):
        return self.execute(context)
    # called by invoke
    def execute(self, context):
        objlist = GetSelectedObjects()
        for object in objlist:
            if IsMusicHolder(object):
                RemoveProperty(object, 'entity_type')
                RemoveProperty(object, 'repeat')
        # redraw properties panel
        for area in bpy.context.screen.areas:
            if area.type in ['PROPERTIES']: area.tag_redraw()
        return {'FINISHED'}
#endregion

#
# EXTRA OPERATORS
#
#region
#endregion


## @brief   CS489 Popup Menu
#  @details Use this class to define the popup menu items that are going to appear under the CS489
#           menuitem.
class CS489PopupMenu(bpy.types.Menu):
    bl_idname = 'cs489.popup_menu'
    bl_label = 'CS489'
    def draw(self, context):
        self.layout.operator_context = 'INVOKE_DEFAULT'
        self.layout.operator(MarkMusicListOp.bl_idname, text='Mark Music Holder List')
        self.layout.operator(UnmarkMusicListOp.bl_idname, text='Unmark Music Holder List')
        self.layout.operator(MarkMusicHolderOp.bl_idname, text='Mark Music Holder')
        self.layout.operator(UnmarkMusicHolderOp.bl_idname, text='Unmark Music Holder')
        self.layout.separator()
        self.layout.operator(MarkCameraAnimationListOp.bl_idname, text='Mark Camera Animation List')
        self.layout.operator(UnmarkCameraAnimationListOp.bl_idname, text='Unmark Camera Animation List')
        self.layout.operator(MarkCameraMarkerOp.bl_idname, text='Mark Camera Marker')
        self.layout.operator(UnmarkCameraMarkerOp.bl_idname, text='Unmark Camera Marker')
        self.layout.separator()
        self.layout.operator(MarkEntityAnimationListOp.bl_idname, text='Mark Entity Animation List')
        self.layout.operator(UnmarkEntityAnimationListOp.bl_idname, text='Unmark Entity Animation List')
        self.layout.operator(MarkEntityMarkerOp.bl_idname, text='Mark Entity Marker')
        self.layout.operator(UnmarkEntityMarkerOp.bl_idname, text='Unmark Entity Marker')
        self.layout.separator()
        self.layout.operator(MarkDoorControllerListOp.bl_idname, text='Mark Door Controller List')
        self.layout.operator(UnmarkDoorControllerListOp.bl_idname, text='Unmark Door Controller List')
        self.layout.operator(MarkDoorControllerOp.bl_idname, text='Mark Door Controller')
        self.layout.operator(UnmarkDoorControllerOp.bl_idname, text='Unmark Door Controller')
        #self.layout.separator()
        
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