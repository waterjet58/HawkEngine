#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosInWorld;
layout(location = 2) in vec3 fragNormalInWorld;

layout( location = 0 ) out vec4 outColor;

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 projection;
	mat4 view;
	mat4 projectionView;
	vec4 ambientLight;
	vec3 lightPosition;
	vec4 lightColor;
} ubo;

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	vec3 directionToLight = ubo.lightPosition - fragPosInWorld;

	float attentuation = 1.0 / dot(directionToLight, directionToLight); //the length of the vector squared

	vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attentuation; //Scale the light color by its intensity and distance to the source
	
	vec3 ambientLight = ubo.ambientLight.xyz * ubo.ambientLight.w; //Scale the ambient color by its intensity
	
	vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalInWorld), normalize(directionToLight)), 0);

	outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);

}