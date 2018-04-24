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
import math
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

class StaticModel:
    # group     - string
    # link_path - string
    # link_file - string
    pass
class DynamicModel:
    # group     - string
    # link_path - string
    # link_file - string
    pass
class ModelInstance:
    # name
    # model
    # position
    # rotation
    pass
class SoundItem:
    # link_path - string
    # link_file - string
    pass
class CameraMarker:
    # name              - string
    # position          - vector3
    # rotation          - matrix4
    # euler_angle       - vector3
    # index             - uint16
    # speed             - real32
    # interpolate_speed - bool
    # fovy              - real32
    # interpolate_fovy  - bool
    pass
class CameraAnimation:
    # name     - string
    # position - vector3
    # rotation - matrix4
    # start    - uint16
    # markers  - CameraMarker[]
    pass
class EntityMarker:
    # name              - string
    # position          - vector3
    # rotation          - matrix4
    # euler_angle       - vector3
    # index             - uint32
    # speed             - real32
    # interpolate_speed - bool
    # anim              - uint32 (animation to play)
    # anim_loop         - bool   (repeat animation)
    # sound             - uint32 (sound to play)
    # sound_loop        - bool   (repeat sound)
    pass
class EntityMarkerList:
    # name     - string
    # position - vector3
    # rotation - matrix4
    # modelref - string
    # markers  - EntityMarker[]
    pass
class DoorController:
    # name         - string
    # position     - vector3
    # rotation     - matrix4
    # halfdims     - vector3
    # door         - string
    # anim_default - int
    # anim_enter   - int
    # anim_leave   - int
    # sound_enter  - int
    # sound_leave  - int
    # close_timer  - real32
    # stay_open    - bool
    pass
class Portal:
    # name - string
    # type - uint32
    # vb   - vector3[]
    # ib   - uint32[]
    pass
class PortalCell:
    # instance - string
    # mesh     - string
    # connect  - string[]
    # portals  - string[]
    pass

def IsMeshObject(obj):
    return (True if obj.type == 'MESH' else False)
def IsMeshGroup(obj):
    if obj.type != 'EMPTY': return False
    if obj.dupli_type != 'GROUP': return False
    if obj.dupli_group is None: return False
    return True
def IsMeshGroup(obj, n):
    if obj.type != 'EMPTY': return False
    if obj.dupli_type != 'GROUP': return False
    if obj.dupli_group is None: return False
    if len(obj.dupli_group.objects) != n: return False
    return True
def GetMeshGroupObjects(obj):
    if IsMeshGroup(obj) == False: return None
    return obj.dupli_group.objects
def GetMeshGroupObjectsFromName(name):
    for obj in bpy.data.objects:
        if obj.name == name:
            rv = []
            list = GetMeshGroupObjects(obj)
            for item in list:
                if list.type == 'MESH': rv.append(item)
            return rv
    return None


##
#  @brief   Gets rid of annoying small values.
#  @details 
def ClearVector(v):
    # zero check
    for i in range(len(v)):
        if abs(v[i]) < 1.0e-4: v[i] = 0.0
    # integer check
    for i in range(len(v)):
        if v[i] > 0:
            t = math.modf(v[i])
            if t[0] < 1.0e-4: v[i] = t[1]
            elif (1.0 - t[0]) < 1.0e-4: v[i] = t[1] + 1.0
        elif v[i] < 0:
            t = math.modf(-v[i])
            if t[0] < 1.0e-4: v[i] = -t[1]
            elif (1.0 - t[0]) < 1.0e-4: v[i] = -t[1] - 1.0

def ClearMatrix(m):
    for r in range(4):
        for c in range(4):
            if m[r][c] > 0:
                t = math.modf(m[r][c])
                if t[0] < 1.0e-4: m[r][c] = t[1]
                elif (1.0 - t[0]) < 1.0e-4: m[r][c] = t[1] + 1.0
            elif m[r][c] < 0:
                t = math.modf(-m[r][c])
                if t[0] < 1.0e-4: m[r][c] = -t[1]
                elif (1.0 - t[0]) < 1.0e-4: m[r][c] = -t[1] - 1.0

#region DOOR CONTROLLER FUNCTIONS

##
#  @brief   Queries Blender object.
#  @details 
def IsDoorController(obj):

    # get mesh object from group
    if IsMeshGroup(obj, 1) == False: return False
    mesh_object = obj.dupli_group.objects[0]

    # 1st try - check custom property
    if mesh_object is None: return False
    if 'entity_type' in mesh_object:
        entity_type = mesh_object['entity_type']
        return entity_type == 'DOOR_CONTROLLER'

    # 2nd try - check object name
    list = mesh_object.name.split('_')
    if (len(list) and list[0] == 'DC'): return True

    return False

#endregion DOOR CONTROLLER FUNCTIONS

##
#  @brief 
#  @details
class WorldUTFExporter:

    mapname     = None
    export_path = None
    export_name = None
    export_fext = None
    filename = None
    file = None

    # data lists
    static_models = []
    dynamic_models = []
    soundlist = []
    sounddict = {}

    # instance lists
    smi_list = []
    dmi_list = []

    # entity lists
    cam_list = [] # CAMERA_ANIMATION_LIST
    mar_list = [] # ENTITY_MARKER_LIST
    dct_list = [] # DOOR_CONTROLLER_LIST

    # portal lists
    portal_list = []
    cell_list = []

    ##
    #  @brief
    #  @details 
    def __init__(self):
        pass

    ##
    #  @brief
    #  @details 
    def WriteInt(self, v): self.file.write('{}\n'.format(int(v)))

    ##
    #  @brief
    #  @details 
    def WriteFloat(self, v): self.file.write('{}\n'.format(float(v)))

    ##
    #  @brief
    #  @details 
    def WriteString(self, s): self.file.write('{}\n'.format(str(s)))

    ##
    #  @brief
    #  @details 
    def WriteVector3(self, v): self.file.write('{} {} {}\n'.format(v[0], v[1], v[2]))

    ##
    #  @brief
    #  @details 
    def WriteVector4(self, v): self.file.write('{} {} {} {}\n'.format(v[0], v[1], v[2], v[3]))

    ##
    #  @brief
    #  @details 
    def WriteMatrix4(self, m):
        # tmp1 = '{0:.6f} {0:.6f} {0:.6f} {0:.6f}'.format(m[0][0], m[0][1], m[0][2], m[0][3])
        # tmp2 = '{0:.6f} {0:.6f} {0:.6f} {0:.6f}'.format(m[1][0], m[1][1], m[1][2], m[1][3])
        # tmp3 = '{0:.6f} {0:.6f} {0:.6f} {0:.6f}'.format(m[2][0], m[2][1], m[2][2], m[2][3])
        # tmp4 = '{0:.6f} {0:.6f} {0:.6f} {0:.6f}'.format(m[3][0], m[3][1], m[3][2], m[3][3])
        tmp1 = '{} {} {} {}'.format(m[0][0], m[0][1], m[0][2], m[0][3])
        tmp2 = '{} {} {} {}'.format(m[1][0], m[1][1], m[1][2], m[1][3])
        tmp3 = '{} {} {} {}'.format(m[2][0], m[2][1], m[2][2], m[2][3])
        tmp4 = '{} {} {} {}'.format(m[3][0], m[3][1], m[3][2], m[3][3])
        self.file.write(tmp1 + ' ' + tmp2 + ' ' + tmp3 + ' ' + tmp4 + '\n')

    ##
    #  @brief
    #  @details 
    def execute(self):

        # build export path
        if 'export_path' in bpy.context.scene: self.export_path = bpy.context.scene['export_path']
        else: self.export_path = os.path.dirname(bpy.data.filepath) + '\\'

        # build export name
        if 'export_name' in bpy.context.scene: self.export_name = bpy.context.scene['export_name']
        else: self.export_name = os.path.splitext(os.path.basename(bpy.data.filepath))[0]

        # build export file extension
        if 'export_fext' in bpy.context.scene: self.export_fext = bpy.context.scene['export_fext']
        else: self.export_fext = 'txt'

        # create file for writing
        self.filename = self.export_path + self.export_name + '.' + self.export_fext
        print(self.filename)
        self.file = open(self.filename, 'w')

        # save map name
        if 'mapname' in bpy.context.scene: self.mapname = bpy.context.scene['mapname']
        else: self.export_name = 'default'
        self.WriteString(self.mapname)

        # examine objects
        prev_mode = bpy.context.mode
        if bpy.context.mode != 'OBJECT': bpy.ops.object.mode_set(mode='OBJECT')
        self.ExamineObjects()
        if prev_mode != bpy.context.mode: bpy.ops.object.mode_set(mode=prev_mode)

        # export objects
        self.ExportObjects()

    ##
    #  @brief
    #  @details 
    def ExamineObjects(self):

        # process static models
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'STATIC_MODEL_LIST': self.ProcessStaticModelList(object)
        # process static model instances
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'STATIC_MODEL_INSTANCES': self.ProcessStaticModelInstances(object)

        # process dynamic models
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'DYNAMIC_MODEL_LIST': self.ProcessDynamicModelList(object)
        # process dynamic model instances
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'DYNAMIC_MODEL_INSTANCES': self.ProcessDynamicModelInstances(object)

        # process sound and music files
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'SOUND_LIST': self.ProcessSoundList(object)

        # process camera animations
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'CAMERA_ANIMATION_LIST': self.ProcessCameraAnimationList(object)

        # process entity marker lists
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'ENTITY_MARKER_LIST': self.ProcessEntityMarkerList(object)

        # process door controllers
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'DOOR_CONTROLLER_LIST': self.ProcessDoorControllerList(object)

        # process portals
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'PORTAL_LIST': self.ProcessPortalList(object)

        # process cells
        for object in bpy.data.objects:
            if 'entity_type' not in object: continue
            entity_type = object['entity_type']
            if entity_type == 'CELL_LIST': self.ProcessCellList(object)

    ##
    #  @brief
    #  @details 
    def ExportObjects(self):

        # save static models
        self.WriteString('###')
        self.WriteString('### STATIC MODELS')
        self.WriteString('###')
        n = len(self.static_models)
        self.WriteString('{} # number of static models'.format(n))
        for item in self.static_models: self.WriteString(item.link_path + '\\' + item.link_file)

        # save dynamic models
        self.WriteString('###')
        self.WriteString('### DYNAMIC MODELS')
        self.WriteString('###')
        n = len(self.dynamic_models)
        self.WriteString('{} # number of dynamic models'.format(n))
        for item in self.dynamic_models: self.WriteString(item.link_path + '\\' + item.link_file)

        # save dynamic models
        self.WriteString('###')
        self.WriteString('### SOUNDS')
        self.WriteString('###')
        n = len(self.soundlist)
        self.WriteString('{} # number of sounds'.format(n))
        for item in self.soundlist: self.WriteString(item.link_path + '\\' + item.link_file)

        # save static model instances
        self.WriteString('###')
        self.WriteString('### STATIC MODEL INSTANCES')
        self.WriteString('###')
        n = len(self.smi_list)
        self.WriteString('{} # number of static model instances'.format(n))
        for item in self.smi_list:
            self.WriteString(item.name)
            self.WriteInt(item.model)
            self.WriteVector3(item.position)
            self.WriteMatrix4(item.rotation)

        # save dynamic model instances
        self.WriteString('###')
        self.WriteString('### DYNAMIC MODEL INSTANCES')
        self.WriteString('###')
        n = len(self.dmi_list)
        self.WriteString('{} # number of dynamic model instances'.format(n))
        for item in self.dmi_list:
            self.WriteString(item.name)
            self.WriteInt(item.model)
            self.WriteVector3(item.position)
            self.WriteMatrix4(item.rotation)

        # save camera animations
        self.WriteString('###')
        self.WriteString('### CAMERA ANIMATIONS')
        self.WriteString('###')
        n = len(self.cam_list)
        self.WriteString('{} # number of camera animations'.format(n))
        for cao in self.cam_list:
            self.WriteString(cao.name)
            self.WriteVector3(cao.position)
            self.WriteMatrix4(cao.rotation)
            self.WriteInt(cao.start)
            self.WriteString('{} # of camera markers'.format(len(cao.markers)))
            for marker in cao.markers:
                self.WriteString(marker.name)
                self.WriteVector3(marker.position)
                self.WriteMatrix4(marker.rotation)
                self.WriteVector3(marker.euler_angle)
                self.WriteInt(marker.index)
                self.WriteFloat(marker.speed)
                self.WriteInt(marker.interpolate_speed)
                self.WriteFloat(marker.fovy)
                self.WriteInt(marker.interpolate_fovy)

        # save entity marker lists
        self.WriteString('###')
        self.WriteString('### ENTITY MARKER LISTS')
        self.WriteString('###')
        n = len(self.mar_list)
        self.WriteString('{} # number of entity marker lists'.format(n))
        for list in self.mar_list:
            self.WriteString(list.name)
            self.WriteVector3(list.position)
            self.WriteMatrix4(list.rotation)
            self.WriteString(list.modelref)
            self.WriteString('{} # of entity markers'.format(len(list.markers)))
            for marker in list.markers:
                self.WriteString(marker.name)
                self.WriteVector3(marker.position)
                self.WriteMatrix4(marker.rotation)
                self.WriteVector3(marker.euler_angle)
                self.WriteInt(marker.index)
                self.WriteFloat(marker.speed)
                self.WriteInt(marker.interpolate_speed)
                self.WriteInt(marker.anim)
                self.WriteInt(marker.anim_loop)
                self.WriteInt(marker.sound)
                self.WriteInt(marker.sound_loop)

        # save door controllers
        self.WriteString('###')
        self.WriteString('### DOOR CONTROLLERS')
        self.WriteString('###')
        n = len(self.dct_list)
        self.WriteString('{} # number of door controllers'.format(n))
        for dc in self.dct_list:
            self.WriteString(dc.name)
            self.WriteVector3(dc.position)
            self.WriteMatrix4(dc.rotation)
            self.WriteVector3(dc.halfdims)
            self.WriteInt(dc.door)
            self.WriteInt(dc.anim_default)
            self.WriteInt(dc.anim_enter)
            self.WriteInt(dc.anim_leave)
            self.WriteInt(dc.sound_enter)
            self.WriteInt(dc.sound_leave)
            self.WriteFloat(dc.close_timer)
            self.WriteInt(dc.stay_open)

        # save portals
        self.WriteString('###')
        self.WriteString('### PORTALS')
        self.WriteString('###')
        n = len(self.dct_list)
        self.WriteString('{} # number of portals'.format(n))
        for portal in self.portal_list:
            self.WriteString(portal.name)
            self.WriteInt(portal.type)
            self.WriteVector3(portal.vb[0])
            self.WriteVector3(portal.vb[1])
            self.WriteVector3(portal.vb[2])
            if portal.type == 4: self.WriteVector3(portal.vb[3])
            if portal.type == 3: self.WriteVector3(portal.ib)
            if portal.type == 4: self.WriteVector4(portal.ib)

    ##
    #  @brief
    #  @details 
    def ProcessStaticModelList(self, object):

        # must be an EMPTY object type
        self.static_models = []
        if object.type != 'EMPTY': raise Exception('Static model lists must be EMPTY Blender objects.')

        # nothing to do
        if len(object.children) == 0: return

        for mdlobj in object.children:
        
            # nothing to do
            if mdlobj.type != 'EMPTY': raise Exception('Static model references must be EMPTY Blender objects.')

            # must have path and file defined
            msg = 'Static model list \"{}\" contains an object \"{}\" with missing \"{}\" declaration.'
            if 'group' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'group'))
            if 'link_path' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'link_path'))
            if 'link_file' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'link_file'))
        
            # otherwise static model is valid
            sm = StaticModel()
            sm.group = mdlobj['group']
            sm.link_path = mdlobj['link_path']
            sm.link_file = mdlobj['link_file']
            self.static_models.append(sm)
        
    ##
    #  @brief
    #  @details 
    def ProcessStaticModelInstances(self, object):

        # must be an EMPTY object type
        self.smi_list = []
        if object.type != 'EMPTY': raise Exception('The static model instance list \"{}\" must be an EMPTY Blender object.'.format(object.name))

        # nothing to do
        if len(object.children) == 0: return

        # create a group map to map names to indices
        gmap = {}
        for i, item in enumerate(self.static_models): gmap[item.group] = i

        # for each child
        for item in object.children:

            # child must be a MESH GROUP
            pf = '{}[{}] '.format(object.name, item.name)
            if item.type != 'EMPTY': raise Exception(pf + 'must be an EMPTY Blender object.')
            if item.dupli_type != 'GROUP': raise Exception(pf + 'must reference a Blender mesh group object.')

            # child must be a validate MESH GROUP object
            group = item.dupli_group
            if group is None: raise Exception(pf + 'must reference a valid Blender mesh group object.')
            if len(group.objects) == 0: raise Exception(pf + 'is a Blender mesh group that contains no meshes.')

            # child must index into the STATIC_MODEL_LIST
            if group.name not in gmap:
                raise Exception(pf + 'must reference a model from a static models list.')

            # must be a STATIC_MODEL
            if 'entity_type' in item:
                if item['entity_type'] != 'STATIC_MODEL':
                    raise Exception(pf + 'is not a static model.')
            else:
                print('Warning: ' + pf + 'is missing the entity_type declaration. Assuming STATIC_MODEL.')

            # otherwise static model is valid
            mi = ModelInstance()
            mi.name = item.name
            mi.model = gmap[group.name]
            mi.position = item.location
            mi.rotation = item.matrix_world
            ClearVector(mi.position)
            ClearMatrix(mi.rotation)
            self.smi_list.append(mi)

    ##
    #  @brief
    #  @details 
    def ProcessDynamicModelList(self, object):

        # must be an EMPTY object type
        self.dynamic_models = []
        if object.type != 'EMPTY': raise Exception('Dynamic model lists must be EMPTY Blender objects.')

        # nothing to do
        if len(object.children) == 0: return

        for mdlobj in object.children:
        
            # nothing to do
            if mdlobj.type != 'EMPTY': raise Exception('Dynamic model references must be EMPTY Blender objects.')

            # must have path and file defined
            msg = 'Dynamic model list \"{}\" contains an object \"{}\" with missing \"{}\" declaration.'
            if 'group' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'group'))
            if 'link_path' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'link_path'))
            if 'link_file' not in mdlobj: raise Exception(msg.format(object.name, mdlobj.name, 'link_file'))
        
            # otherwise static model is valid
            dm = DynamicModel()
            dm.group = mdlobj['group']
            dm.link_path = mdlobj['link_path']
            dm.link_file = mdlobj['link_file']
            self.dynamic_models.append(dm)
        
    ##
    #  @brief
    #  @details 
    def ProcessDynamicModelInstances(self, object):

        # must be an EMPTY object type
        self.dmi_list = []
        if object.type != 'EMPTY': raise Exception('The dynamic model instance list \"{}\" must be an EMPTY Blender object.'.format(object.name))

        # nothing to do
        if len(object.children) == 0: return

        # create a group map to map names to indices
        gmap = {}
        for i, item in enumerate(self.dynamic_models): gmap[item.group] = i

        # for each child
        for item in object.children:

            # child must be a MESH GROUP
            pf = '{}[{}] '.format(object.name, item.name)
            if item.type != 'EMPTY': raise Exception(pf + 'must be an EMPTY Blender object.')
            if item.dupli_type != 'GROUP': raise Exception(pf + 'must reference a Blender mesh group object.')

            # child must be a validate MESH GROUP object
            group = item.dupli_group
            if group is None: raise Exception(pf + 'must reference a valid Blender mesh group object.')
            if len(group.objects) == 0: raise Exception(pf + 'is a Blender mesh group that contains no meshes.')

            # child must index into the DYNAMIC_MODEL_LIST
            if group.name not in gmap:
                raise Exception(pf + 'must reference a model from a dynamic models list.')

            # must be a DYNAMIC_MODEL
            if 'entity_type' in item:
                if item['entity_type'] != 'DYNAMIC_MODEL':
                    raise Exception(pf + 'is not a dynamic model.')
            else:
                print('Warning: ' + pf + 'is missing the entity_type declaration. Assuming DYNAMIC_MODEL.')

            # otherwise static model is valid
            mi = ModelInstance()
            mi.name = item.name
            mi.model = gmap[group.name]
            mi.position = item.location
            mi.rotation = item.matrix_world
            ClearVector(mi.position)
            ClearMatrix(mi.rotation)
            self.dmi_list.append(mi)

    ##
    #  @brief
    #  @details 
    def ProcessSoundList(self, object):

        # must be an EMPTY object type
        self.soundlist = []
        if object.type != 'EMPTY': raise Exception('Sound lists must be EMPTY Blender objects.')

        # nothing to do
        if len(object.children) == 0:  return

        # for each child
        for child in object.children:
        
            # nothing to do
            if child.type != 'EMPTY': raise Exception('Sound items must be EMPTY Blender objects.')

            # must have path and file defined
            msg = 'Sound list \"{}\" contains an item \"{}\" with missing \"{}\" declaration.'
            if 'link_path' not in child: raise Exception(msg.format(object.name, child.name, 'link_path'))
            if 'link_file' not in child: raise Exception(msg.format(object.name, child.name, 'link_file'))
        
            # otherwise static model is valid
            item = SoundItem()
            item.name = child.name
            item.link_path = child['link_path']
            item.link_file = child['link_file']
            self.soundlist.append(item)

        # build a dictionary to map <item.name> to <index>
        self.sounddict = {}
        for i, item in enumerate(self.soundlist):
            self.sounddict[item.name] = i
 
    ##
    #  @brief
    #  @details 
    def ProcessCameraAnimationList(self, object):

        if object is None: raise Exception('Invalid argument.')
        pf1 = '{} '.format(object.name)

        # must be a Plain Axes or Group object
        self.cam_list = []
        if object.type != 'EMPTY': raise Exception(pf1 + 'must be an EMPTY Blender object.')

        # nothing to do
        if len(object.children) == 0: return

        # initialize a camera animation object
        cao = CameraAnimation()
        cao.name = object.name
        cao.position = object.matrix_world * object.location
        cao.rotation = object.matrix_world * object.matrix_local
        cao.start = 0
        cao.markers = []
        ClearVector(cao.position)
        ClearMatrix(cao.rotation)

        # process children
        index = 0
        for item in object.children:

            # child must be an EMPTY
            pf2 = '{}[{}] '.format(object.name, item.name)
            if item.type != 'EMPTY': raise Exception(pf2 + 'must be an EMPTY Blender object.')

            # must be a CAMERA_MARKER
            if 'entity_type' in item:
                if item['entity_type'] != 'CAMERA_MARKER':
                    raise Exception(pf + 'is not a CAMERA_MARKER.')
            else:
                print('Warning: ' + pf + 'is missing the entity_type declaration. Assuming CAMERA_MARKER.')
 
            # initialize a camera animation object
            cmo = CameraMarker()
            cmo.name = item.name
            cmo.position = item.location
            cmo.rotation = item.matrix_world
            cmo.euler_angle       = item.rotation_euler
            cmo.index             = index
            cmo.speed             = 1.0
            cmo.interpolate_speed = True
            cmo.fovy              = 60.0
            cmo.interpolate_fovy  = True

            # read properties (if present)
            if 'index' in item: cmo.index = int(item['index'])
            if 'speed' in item: cmo.speed = float(item['speed'])
            if 'interpolate_speed' in item: cmo.interpolate_speed = bool(item['interpolate_speed'])
            if 'fovy' in item: cmo.fovy = item['fovy']
            if 'interpolate_fovy' in item: cmo.interpolate_fovy = item['interpolate_fovy']

            # validate properties
            # TODO: finish this

            # small numbers are annoying
            ClearVector(cmo.position)
            ClearMatrix(cmo.rotation)
            ClearVector(cmo.euler_angle)

            # append marker object and increment index
            cao.markers.append(cmo)
            index = index + 1

        # append camera animation object only if there are markers
        if len(cao.markers): self.cam_list.append(cao)

    ##
    #  @brief
    #  @details 
    def ProcessEntityMarkerList(self, object):

        # must have object
        if object is None: raise Exception('Invalid argument.')
        pf1 = '{} '.format(object.name)

        # must be a Plain Axes or Group object
        self.mar_list = []
        if object.type != 'EMPTY': raise Exception(pf1 + 'must be an EMPTY Blender object.')

        # nothing to do
        if len(object.children) == 0: return

        # initialize EntityMarkerList object
        eml = EntityMarkerList()
        eml.name = object.name
        eml.position = object.matrix_world * object.location
        eml.rotation = object.matrix_world * object.matrix_local
        eml.modelref = object['model'] if 'model' in object else ''
        eml.markers = []
        ClearVector(eml.position)
        ClearMatrix(eml.rotation)

        # process children
        index = 0
        for item in object.children:

            # child must be an EMPTY object
            pf2 = '{}[{}] '.format(object.name, item.name)
            if item.type != 'EMPTY': raise Exception(pf2 + 'must be an EMPTY Blender object.')

            # must be an ENTITY_MARKER
            if 'entity_type' in item:
                if item['entity_type'] != 'ENTITY_MARKER':
                    raise Exception(pf + 'is not an ENTITY_MARKER.')
            else:
                print('Warning: ' + pf + 'is missing the entity_type declaration. Assuming ENTITY_MARKER.')
 
            # initialize a camera animation object
            em = EntityMarker()
            em.name = item.name
            em.position = item.location
            em.rotation = item.matrix_world
            em.euler_angle       = item.rotation_euler
            em.index             = index
            em.speed             = 1.0
            em.interpolate_speed = True
            em.anim              = -1
            em.anim_loop         = False
            em.sound             = -1
            em.sound_loop        = False

            # read properties (if present)
            if 'index' in item: em.index = int(item['index'])
            if 'speed' in item: em.speed = float(item['speed'])
            if 'interpolate_speed' in item: em.interpolate_speed = bool(item['interpolate_speed'])
            if 'anim' in item: em.anim = item['anim']
            if 'anim_loop' in item: em.anim_loop = item['anim_loop']
            if 'sound' in item: em.sound = item['sound']
            if 'sound_loop' in item: em.sound_loop = item['sound_loop']

            # validate properties
            # TODO: finish this

            # small numbers are annoying
            ClearVector(em.position)
            ClearMatrix(em.rotation)
            ClearVector(em.euler_angle)

            # append marker object and increment index
            eml.markers.append(em)
            index = index + 1

        # append camera animation object only if there are markers
        if len(eml.markers): self.mar_list.append(eml)

    ##
    #  @brief
    #  @details 
    def ProcessDoorControllerList(self, object):

        # validate object
        if object.type != 'EMPTY': raise Exception('{} must be an EMPTY Blender object.'.format(object.name))
        if len(object.children) == 0: return

        # create a group map to map names to indices
        gmap = {}
        for i, item in enumerate(self.dmi_list):
            print('name = {}'.format(item.name))
            gmap[item.name] = i

        # process children, appending in case there are multiple lists
        for item in object.children:

            # child must be a GROUP
            pf = '{}[{}] '.format(object.name, item.name)
            if item.type != 'EMPTY': raise Exception(pf + 'must be an EMPTY Blender object.')
            if item.dupli_type != 'GROUP': raise Exception(pf + 'must reference a Blender mesh group object.')

            # child must be a valid GROUP object
            group = item.dupli_group
            if group is None: raise Exception(pf + 'must reference a valid Blender mesh group object.')
            if len(group.objects) == 0: raise Exception(pf + 'is an invalid Blender mesh group object.')
            if group.objects[0].type != 'MESH': raise Exception(pf + 'is an invalid Blender mesh group object.')

            # get mesh object and its bounding box
            meshobj = group.objects[0]
            bb = group.objects[0].bound_box

            # compute entity bounding box
            min_v = [ bb[0][0], bb[0][1], bb[0][2] ]
            max_v = [ bb[0][0], bb[0][1], bb[0][2] ]
            for pt in bb:
                v = mathutils.Vector(pt)
                if v[0] < min_v[0]: min_v[0] = v[0]
                if v[1] < min_v[1]: min_v[1] = v[1]
                if v[2] < min_v[2]: min_v[2] = v[2]
                if max_v[0] < v[0]: max_v[0] = v[0]
                if max_v[1] < v[1]: max_v[1] = v[1]
                if max_v[2] < v[2]: max_v[2] = v[2]

            # door must exist in DYNAMIC_MODEL_LIST
            if 'door' not in item: raise Exception(pf + ' missing \"door\" declaration.')
            door = item['door']
            if door not in gmap:
                raise Exception(pf + 'must reference a model from a dynamic model instance list.'.format(door))

            # lookup sound references
            sndo = -1
            sndc = -1
            if 'sound_open' in item:
                name = item['sound_open']
                if name in self.sounddict: sndo = self.sounddict[name]
            if 'sound_close' in item:
                name = item['sound_close']
                if name in self.sounddict: sndc = self.sounddict[name]

            # initialize door controller
            dc = DoorController()
            dc.name         = item.name
            dc.position     = item.matrix_world * meshobj.location
            dc.rotation     = item.matrix_world * meshobj.matrix_local
            dc.halfdims     = [(max_v[0] - min_v[0])/2.0, (max_v[1] - min_v[1])/2.0, (max_v[2] - min_v[2])/2.0]
            dc.door         = gmap[door]
            dc.anim_default = -1 if 'anim_idle' not in item else item['anim_idle']
            dc.anim_enter   = -1 if 'anim_open' not in item else item['anim_open']
            dc.anim_leave   = -1 if 'anim_close' not in item else item['anim_close']
            dc.sound_enter  = sndo
            dc.sound_leave  = sndc
            dc.close_timer  = 5.0 if 'close_timer' not in item else item['close_timer']
            if 'stay_open' in item:
                dc.stay_open = False if item['stay_open'] == 0 else True
            else:
                dc.stay_open = False

            # insert entity
            self.dct_list.append(dc)

    ##
    #  @brief
    #  @details 
    def ProcessPortalList(self, object):

        # Portals are just the windows that connect adjacent cells.
        # Every cell needs to know what portals it needs to check.

        # validate object
        if object.type != 'EMPTY': raise Exception('{} must be an EMPTY Blender object.'.format(object.name))
        if len(object.children) == 0: return

        # process children, appending in case there are multiple lists
        for item in object.children:

            # meshobj must be a MESH or a GROUP MESH
            pf = '{}[{}] '.format(object.name, item.name)
            meshobj = None
            if item.type == 'MESH':
                meshobj = item
            # child must be a GROUP
            elif item.type == 'EMPTY' and item.dupli_type == 'GROUP':
                if item.group is None: raise Exception(pf + 'is an invalid Blender mesh group object.')
                if len(item.group) == 0: raise Exception(pf + 'is an invalid Blender mesh group object.')
                meshobj = item.group[0]
            # error
            else:
                raise Exception(pf + 'must be an EMPTY or MESH Blender object.')

            # portals must have three or four vertices
            verts = meshobj.data.vertices
            n_verts = len(verts)
            if (n_verts < 3) or (n_verts > 4): raise Exception(pf + 'must be a mesh with 3 or 4 vertices.')

            # portal must have only one polygon
            polys = meshobj.data.polygons
            n_polys = len(polys)
            if n_polys != 1: raise Exception(pf + 'must be a single polygon mesh.')

            # portal must have three or four face indices
            n_indices = len(meshobj.data.loops)
            vb = None
            ib = None
            if n_verts == 3 and n_indices == 3:
                vb = [verts[0].co, verts[1].co, verts[2].co]
                ClearVector(vb[0])
                ClearVector(vb[1])
                ClearVector(vb[2])
                ib = [meshobj.data.loops[0].vertex_index,
                      meshobj.data.loops[1].vertex_index,
                      meshobj.data.loops[2].vertex_index]
            elif n_verts == 4 and n_indices == 4:
                vb = [verts[0].co, verts[1].co, verts[2].co, verts[3].co]
                ClearVector(vb[0])
                ClearVector(vb[1])
                ClearVector(vb[2])
                ClearVector(vb[3])
                ib = [meshobj.data.loops[0].vertex_index,
                      meshobj.data.loops[1].vertex_index,
                      meshobj.data.loops[2].vertex_index,
                      meshobj.data.loops[3].vertex_index]
            else:
                print('n_verts = {} n_indices = {}'.format(n_verts, n_indices))
                raise Exception(pf + 'must be a single polygon mesh with 3 or 4 vertices.')

            # insert portal
            portal = Portal()
            portal.name = item.name
            portal.type = n_verts
            portal.vb = vb
            portal.ib = ib
            self.portal_list.append(portal)

    ##
    #  @brief
    #  @details 
    def ProcessCellList(self, object):

        # class PortalCell:
        #     # instance - string
        #     # mesh     - string
        #     # connect  - string[]
        #     # portals  - string[]
        #     pass

        # validate object
        if object.type != 'EMPTY': raise Exception('{} must be an EMPTY Blender object.'.format(object.name))
        if len(object.children) == 0: return

        # create a group map to map names to indices (for static models)
        gmap = {}
        for i, item in enumerate(self.smi_list):
            print('name = {}'.format(item.name))
            gmap[item.name] = i

        # process children, appending in case there are multiple lists
        for item in object.children:

            # item must be an EMPTY object
            # item can be an AXIS or GROUP object that points to a mesh
            pf = '{}[{}] '.format(object.name, item.name)
            if item.type != 'EMPTY': raise Exception(pf + 'must be an EMPTY Blender object.')

            # must be a CELL
            if 'entity_type' in item:
                if item['entity_type'] != 'CELL':
                    raise Exception(pf + 'is not an CELL.')
            else:
                print('Warning: ' + pf + 'is missing the entity_type declaration. Assuming CELL.')

            # instance must exist in STATIC_MODEL_INSTANCES
            if 'instance' not in item: raise Exception(pf + ' missing \"instance\" declaration.')
            instance = item['instance']
            if instance not in gmap:
                raise Exception(pf + 'must reference a model from a static model instance list.'.format(instance))

            # get mesh list from 'instance'
            meshlist = GetMeshGroupObjectsFromName(instance)
            if (meshlist is None) or (len(meshlist) == 0):
                Exception(pf + 'must reference a model with at least one or more meshes.')

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