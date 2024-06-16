@echo off

del *.spv

glslc.exe simple_shader.vert -o simple_shader.vert.spv
glslc.exe simple_shader.frag -o simple_shader.frag.spv
glslc.exe point_light.vert -o point_light.vert.spv
glslc.exe point_light.frag -o point_light.frag.spv

pause