#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec2 tex>

uniform mat4 projectionView;
uniform vec3 colorBBox;

out vec3 color;

void main()
{
	color = colorBBox;
    gl_Position = projectionView * vec4(vertex, 1.0);
}  