#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec3 colorParticle;
layout (location = 2) in vec3 offset;
layout (location = 3) in vec3 aNormal;

uniform mat4 projectionView;

out vec3 color;
out vec3 fpos;
out vec3 normal;

void main()
{
	vec3 v = vertex * 0.005;
	
	color = colorParticle;
	normal = aNormal;
	fpos = v;

    gl_Position = projectionView * vec4(v + offset, 1.0);
}  