#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

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

void main(){
	
	vec4 positionInWorld = push.modelMatrix * vec4(position, 1.0);

	gl_Position = ubo.projectionView * positionInWorld; // Matrix * (Matrix * vec3) slightly faster

	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	
	fragPosWorld = positionInWorld.xyz;
	
	fragColor = color;
	
}