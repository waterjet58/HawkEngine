#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 projectionViewMatrix;
	vec4 ambientLight;
	vec3 lightPosition;
	vec4 lightColor;
} ubo;

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main(){
	
	vec4 positionInWorld = push.modelMatrix * vec4(position, 1.0);

	gl_Position = ubo.projectionViewMatrix * positionInWorld;

	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

	vec3 directionToLight = ubo.lightPosition - positionInWorld.xyz;

	float attentuation = 1.0 / dot(directionToLight, directionToLight); //the length of the vector squared

	vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attentuation; //Scale the light color by its intensity and distance to the source
	
	vec3 ambientLight = ubo.ambientLight.xyz * ubo.ambientLight.w; //Scale the ambient color by its intensity
	
	vec3 diffuseLight = lightColor * max(dot(normalWorldSpace, normalize(directionToLight)), 0);

	fragColor = (diffuseLight + ambientLight) * color;
}