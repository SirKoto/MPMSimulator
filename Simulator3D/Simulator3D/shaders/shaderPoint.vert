#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec3 colorParticle;

uniform mat4 projectionView;

out vec3 color;

void main()
{
	color = colorParticle;
    gl_Position = projectionView * vec4(vertex, 1.0);
}  