//subroutine void MainSelection();
//subroutine uniform MainSelection mainRender;

attribute vec3 aVertexPosition;
attribute vec3 aVertexNormal;
attribute vec2 aTextureCoord;

uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uViewProjectionLightMatrix;

varying vec2 vTextureCoord;
varying vec3 vTransformedNormal;
varying vec4 vPosition;
varying vec4 vShadowCoord; // for shadow mapping

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
	
	//mainRender();
	RenderScene();
}