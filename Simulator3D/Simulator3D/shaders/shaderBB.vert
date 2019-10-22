#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec3 normal;

uniform mat4 projectionView;
uniform vec3 colorBBox;

out vec3 color;
out vec3 o_normal;
out vec3 frag_pos;

void main()
{
	color = abs(normal);
	o_normal = -normal;
	frag_pos = vertex;

    gl_Position = projectionView * vec4(vertex, 1.0);
}  