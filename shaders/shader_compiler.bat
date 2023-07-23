@echo off

del simple_shader.vert.spv
del simple_shader.frag.spv

glslc.exe simple_shader.vert -o simple_shader.vert.spv
glslc.exe simple_shader.frag -o simple_shader.frag.spv
pause