import bpy

def SetCellMeshState(state):
	meshlist = GetSelectedMeshObjects()
	if meshlist == None: return
	for mesh in meshlist: mesh['is_cell'] = state
	
def SetPortalMeshState(state):
	meshlist = GetSelectedMeshObjects()
	if meshlist == None: return
	for mesh in meshlist: mesh['is_portal'] = state
	
class SetCellMeshStateOperator(bpy.types.Operator):
	bl_idname = "object.set_cell"
	bl_label = "Set Cell Mesh"
	@classmethod
	def poll(cls, context):
		return context.active_object is not None
	def execute(self, context):
		for ob in context.scene.objects:
			SetPortalMeshState(False)
			SetCellMeshState(True)
		return {'FINISHED'}
def register(): bpy.utils.register_class(SetCellMeshStateOperator)
def unregister(): bpy.utils.unregister_class(SetCellMeshStateOperator)
if __name__ == "__main__":
	register()
	bpy.ops.object.set_cell()