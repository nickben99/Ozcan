#version 330 core

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec2 aTextureCoord;
layout(location = 2) in vec3 aVertexNormal;

uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uViewProjectionLightMatrix;
uniform vec3 LightInvDirection_worldspace;

out vec2 vTextureCoord;
out vec3 vTransformedNormal;
out vec4 vPosition;
out vec4 vShadowCoord; // for shadow mapping

void RenderScene()
{
	vShadowCoord = uViewProjectionLightMatrix * uModelMatrix * vec4(aVertexPosition, 1.0); 

	vTextureCoord = aTextureCoord;
	vTransformedNormal = (uViewMatrix * uModelMatrix * vec4(aVertexNormal, 0.0)).xyz;
}

void main() {
	vPosition = uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0);
	gl_Position = uProjectionMatrix * vPosition;

	RenderScene();
}

