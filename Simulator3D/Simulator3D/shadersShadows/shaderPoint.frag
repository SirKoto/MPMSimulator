#version 330 core
out vec4 out_color;

in vec3 color;
in vec3 fpos;
in vec3 normal;
in vec4 fPosLightSpace;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientLight;
uniform sampler2D shadowMap;


float shadowCompute(vec4 fragLightSpace)
{
	vec3 projCoord = fragLightSpace.stp / fragLightSpace.q;
	projCoord = projCoord * 0.5 + 0.5;

	float closestDepth = texture(shadowMap, projCoord.st).r;
	float currDepth = projCoord.p;
	return currDepth > closestDepth ? 0.0 : 1.0;
}

void main()
{    
	vec3 norm = normal;
	vec3 ligthDir = normalize(fpos - lightPos);

	float diff = max(0,dot(-ligthDir, norm));

	vec3 diffLight = lightColor * diff;
	
    float shadow = shadowCompute(fPosLightSpace);

    out_color = vec4(color * (ambientLight + shadow * diffLight), 1.0f);
}  