#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec3 normal;

uniform mat4 projectionView;
uniform vec3 colorBBox;
uniform vec3 camera;

out vec3 color;

void main()
{
	color = normal * 0.5 + 0.5;
    gl_Position = projectionView * vec4(vertex, 1.0);
}  