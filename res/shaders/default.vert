#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

out vec4 vertColor;
void main() {
   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
   vertColor   = vec4(col.x, col.y, col.z, 1.0);
}