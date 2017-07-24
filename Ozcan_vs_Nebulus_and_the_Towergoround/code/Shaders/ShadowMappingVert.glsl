#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec2 aTextureCoord;
layout(location = 2) in vec3 aVertexNormal;

// Output data ; will be interpolated for each fragment.
out vec2 vTextureCoord;
//out vec3 Position_worldspace;
out vec3 vTransformedNormal;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec4 vShadowCoord;

// Values that stay constant for the whole mesh.
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix; // look at matrix
uniform mat4 uModelMatrix; // the position of the modal
uniform vec3 LightInvDirection_worldspace;
uniform mat4 uViewProjectionLightMatrix;


void main(){
	
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aVertexPosition,1);
	
	vShadowCoord = uViewProjectionLightMatrix * uModelMatrix * vec4(aVertexPosition,1); // mofidied form: uViewProjectionLightMatrix * vec4(aVertexPosition,1);
	
	// Position of the vertex, in worldspace : M * position
	//Position_worldspace = (uModelMatrix * vec4(aVertexPosition,1)).xyz;
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	EyeDirection_cameraspace = vec3(0,0,0) - ( uViewMatrix * uModelMatrix * vec4(aVertexPosition,1)).xyz;

	// Vector that goes from the vertex to the light, in camera space
	LightDirection_cameraspace = (uViewMatrix * vec4(LightInvDirection_worldspace,0)).xyz;
	
	// Normal of the the vertex, in camera space
	vTransformedNormal = ( uViewMatrix * uModelMatrix * vec4(aVertexNormal,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	
	// vTextureCoord of the vertex. No special space for this one.
	vTextureCoord = aTextureCoord;
}

