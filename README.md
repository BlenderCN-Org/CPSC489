# CPSC489<br />
Game Development Project<br />
<br />
Week 1:<br />
01/22/2018 - 01/28/2018<br />
Added and worked on a Blender plugin to export models to an ASCII file.<br />
Worked an unknown amount of hours on it.<br />
<br />
Week 2:<br />
01/29/2018<br />
Added base window and application code. Worked a good 3 - 4 hours on it.<br />
Added basic Direct3D initialization code.<br />
02/04/2018<br />
Added input layouts, started work on shaders.<br />
<br />
Week 3:<br />
02/05/2018<br />
Added camera class.<br />
Added math.h, matrix4x4.h, vector3.h and ray.h files.<br />
02/06/2018<br />
Added grid.h file. Support for some basic geometry.<br />
Added code to "gfx" units for creating buffers and doing input assembly.<br />
02/08/2018<br />
Fixed some issues with the orbit box.<br />
Turntable camera system now fully working.<br />
02/09/2018<br />
Added developer menu.<br />
Added skeleton axes test to developer menu.<br />
Week 4:<br />
02/12/2018<br />
Week 5:<br />
02/19/2018<br />
Week 6:<br />
02/26/2018<br />
Week 7:<br />
03/05/2018<br />
Week 8:<br />
03/12/2018<br />
Week 9:<br />
03/19/2018<br />
Added a few intersection routines for points, spheres, AABBs, and OBBs.<br />
Added AABB bounding volume hierarchy class, bvh.h and bvh.cpp.<br />
Works but not implemented ingame yet.<br />
Added program and code to generate characters from skeletons.<br />
03/20/2018<br />
Added a fifth vertex shader, VS_AABB_MINMAX + IL_AABB_MINMAX, for drawing instanced
minmax AABBs. This is a debugging vertex shader, and will be used to test the AABB
bounding volume hierarchy tree.<br />
03/26/2018<br />
Revised Blender script.<br />
Added custom property "path" to Blender script for Texture objects.<br />
Added custom property "type" to Blender script for Texture objects.<br />
03/27/2018<br />
Added custom property "entity_type" to Blender script.<br />
Added support in Blender script for collision meshes.<br />
Added a documentation folder to document custom properties in Blender.<br />
03/28/2018<br />
Started rewriting mesh class.<br />
03/29/2018<br />
Changed the way materials were defined in the mesh file format.<br />
Added a mesh test to test opening, loading and saving meshes.<br />
Began work on sound controllers.<br />
Added functions for file input/output with big endian support.<br />
Added a binary stream stream class to read binary file data efficiently.<br />
03/30/2018<br />
Worked on XAudio code.<br />
03/31/2018<br />
Integrated sound code into map file format.<br />
04/01/2018<br />
Added support for splitscreen play.<br />
04/04/2018<br />
Worked on Blender importer for model format. Currently imports skeletons and animations.<br />