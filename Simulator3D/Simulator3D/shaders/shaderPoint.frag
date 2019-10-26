#version 330 core
out vec4 out_color;

in vec3 color;
in vec3 fpos;
in vec3 normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientLight;


void main()
{    
	vec3 norm = normal;
	vec3 ligthDir = normalize(fpos - lightPos);

	float diff = max(0,dot(-ligthDir, norm));

	vec3 diffLight = lightColor * diff;
	
    out_color = vec4(color * (ambientLight + diffLight), 1.0f);
}  