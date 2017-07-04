#version 330 core

//subroutine void MainSelection();
//subroutine uniform MainSelection mainRender;
uniform bool uMainRenderVertexShader;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aTextureCoord;

uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uViewProjectionLightMatrix;

out vec2 vTextureCoord;
out vec3 vTransformedNormal;
out vec4 vPosition;
out vec4 vShadowCoord; // for shadow mapping

//subroutine( MainSelection )
void MainShadowMapCreation()
{
	
}

//subroutine( MainSelection )
void RenderScene()
{
	vShadowCoord = uViewProjectionLightMatrix * uModelMatrix * vec4(aVertexPosition, 1.0); 

	vTextureCoord = aTextureCoord;
	vTransformedNormal = (uViewMatrix * uModelMatrix * vec4(aVertexNormal, 0.0)).xyz;
}

void main(void) {
	vPosition = uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0);
	gl_Position = uProjectionMatrix * vPosition;
	
	if (uMainRenderVertexShader) {
		MainShadowMapCreation();
	} else {
		RenderScene();
	}
}