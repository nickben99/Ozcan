//precision mediump float;

//subroutine vec3 LightingSelection();
//subroutine uniform LightingSelection lightingRender;

//subroutine vec4 ColorSelection(vec3 lightWeighting);
//subroutine uniform ColorSelection colorRender;

//subroutine vec4 TextureSelection();
//subroutine uniform TextureSelection textureRender;

//subroutine void MainSelection();
//subroutine uniform MainSelection mainRender;

varying vec2 vTextureCoord;
varying vec3 vTransformedNormal;
varying vec4 vPosition;
varying vec4 vShadowCoord; // for shadow mapping

uniform float uMaterialShininess;

uniform vec3 uAmbientColor;

uniform vec3 uPointLightingLocation;
uniform vec3 uPointLightingSpecularColor;
uniform vec3 uPointLightingDiffuseColor;
uniform vec4 uPointLightingEmissiveColor;

uniform sampler2D uSampler;
uniform sampler2D uShadowMap; // for shadow mapping

uniform bool uIsDebugging; // should be a subroutine function pointer

uniform float shadowCoordBias;
uniform float uVisibilityDecrement;

// DEBUG PARAMS --------------------------------------------------------------------------------------------------------
uniform bool uIsDebugRendering; // debug spheres/boxes etc
uniform bool uShowSpecularHighlights;
uniform bool uPoisson;

uniform bool uUseAmbient;
uniform bool uUseDiffuse;
uniform bool uUseEmmissive;

uniform bool uUseMaterialShininessOverride;
uniform float materialShininessOverride;
uniform bool uUseAmbientOverride;
uniform vec3 uAmbientOverride;
uniform bool uUseDiffuseOverride;
uniform vec3 uDiffuseOverride;
uniform bool uUseEmmissiveOverride;
uniform vec4 uEmmissiveOverride;
uniform bool uUseSpecularOverride;
uniform vec3 uSpecularOverride;
// DEBUG PARAMS ----------------------------------------------------------------------------------------------------------

vec2 poissonDisk[4];

//subroutine( TextureSelection )
vec4 RenderWithTexture()
{
	return texture2D(uSampler, vec2(vTextureCoord.s, vTextureCoord.t));
}

//subroutine( TextureSelection )
vec4 RenderNOTexture()
{
	return vec4(1.0, 1.0, 1.0, 1.0);
}
	
//subroutine( LightingSelection )
vec3 CalculateSceneLighting_ReleaseBuild()
{
	poissonDisk[0] = vec2( -0.94201624, -0.39906216 );
	poissonDisk[1] = vec2( 0.94558609, -0.76890725 );
	poissonDisk[2] = vec2( -0.094184101, -0.92938870 );
	poissonDisk[3] = vec2( 0.34495938, 0.29387760 );

	vec3 lightWeighting = vec3(0.0, 0.0, 0.0);
	vec3 lightDirection = normalize(uPointLightingLocation - vPosition.xyz);
	vec3 normal = normalize(vTransformedNormal);

	float visibility = 1.0;
	float coordZ = (vShadowCoord.z-shadowCoordBias);
	for (int i = 0; i < 4; i++)
	{
		visibility -= (uVisibilityDecrement * step(texture2D( uShadowMap, vShadowCoord.xy + poissonDisk[i]/700.0 ).z, coordZ));
	}
	
	visibility *= float(gl_FrontFacing);
	
	float specularLightWeighting = 0.0;	
	vec3 eyeDirection = normalize(-vPosition.xyz); // this is because the camera doesn't move, the world moves away from the camera
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	specularLightWeighting = pow(max(dot(reflectionDirection, eyeDirection), 0.0), uMaterialShininess);		

	float diffuseLightWeighting = max(dot(normal, lightDirection), 0.0);
		
	lightWeighting += uAmbientColor;
	lightWeighting += uPointLightingSpecularColor * specularLightWeighting * visibility;			
	lightWeighting += uPointLightingDiffuseColor * diffuseLightWeighting * visibility;
	return lightWeighting;
}

//subroutine( ColorSelection )
vec4 CalculateFragmentColor_ReleaseBuild(vec3 lightWeighting)
{	
	//vec4 fragmentColor = textureRender();
	vec4 fragmentColor = RenderWithTexture();
	
	fragmentColor *= uPointLightingEmissiveColor;	
	return vec4(fragmentColor.rgb * lightWeighting, fragmentColor.a);	
}

//subroutine( LightingSelection )
vec3 CalculateSceneLighting_DebugBuild()
{
	poissonDisk[0] = vec2( -0.94201624, -0.39906216 );
	poissonDisk[1] = vec2( 0.94558609, -0.76890725 );
	poissonDisk[2] = vec2( -0.094184101, -0.92938870 );
	poissonDisk[3] = vec2( 0.34495938, 0.29387760 );

	vec3 lightWeighting = vec3(0.0, 0.0, 0.0);
	vec3 lightDirection = normalize(uPointLightingLocation - vPosition.xyz);
	vec3 normal = normalize(vTransformedNormal);

	float visibility = 1.0;
	if (!uIsDebugRendering)
	{		
		float coordZ = (vShadowCoord.z-shadowCoordBias);
		if (uPoisson)
		{ 
			for (int i=0;i<4;i++)
			{	
				visibility -= (uVisibilityDecrement * step(texture2D( uShadowMap, vShadowCoord.xy + poissonDisk[i]/700.0 ).z, coordZ));
			}			
		}
		else
		{
			visibility -= (uVisibilityDecrement * 4.0 * step(texture2D( uShadowMap, vShadowCoord.xy ).z, coordZ));
		}
	}
	
	visibility *= float(gl_FrontFacing);
	
	float specularLightWeighting = 0.0;
	if (uShowSpecularHighlights) {
		vec3 eyeDirection = normalize(-vPosition.xyz); // this is because the camera doesn't move, the world moves away from the camera
		vec3 reflectionDirection = reflect(-lightDirection, normal);

		if (uUseMaterialShininessOverride)
		{
			specularLightWeighting = pow(max(dot(reflectionDirection, eyeDirection), 0.0), materialShininessOverride);
		}
		else
		{
			specularLightWeighting = pow(max(dot(reflectionDirection, eyeDirection), 0.0), uMaterialShininess);
		}
	}

	float diffuseLightWeighting = max(dot(normal, lightDirection), 0.0);
	
	if (uUseAmbient)
	{
		if (uUseAmbientOverride)
		{
			lightWeighting += uAmbientOverride;
		}
		else
		{
			lightWeighting += uAmbientColor;
		}
	}
	
	if (uUseSpecularOverride)
	{
		lightWeighting += uSpecularOverride * specularLightWeighting * visibility;
	}
	else
	{
		lightWeighting += uPointLightingSpecularColor * specularLightWeighting * visibility;			
	}
	
	if (uUseDiffuse)
	{
		if (uUseDiffuseOverride)
		{
			lightWeighting += uDiffuseOverride * diffuseLightWeighting * visibility;
		}
		else
		{
			lightWeighting += uPointLightingDiffuseColor * diffuseLightWeighting * visibility;
		}
	}
	return lightWeighting;
}

//subroutine( ColorSelection )
vec4 CalculateFragmentColor_DebugBuild(vec3 lightWeighting)
{
	vec4 emmissive = uPointLightingEmissiveColor;
	if (uUseEmmissiveOverride)
	{
		emmissive = uEmmissiveOverride;
	}
	
	//vec4 fragmentColor = textureRender();
	vec4 fragmentColor = RenderWithTexture();
	
	if (uUseEmmissive)
	{
		fragmentColor *= emmissive;
	}		
	return vec4(fragmentColor.rgb * lightWeighting, fragmentColor.a);	
}

//subroutine( LightingSelection )
vec3 RenderNoLighting()
{
	return vec3(1.0, 1.0, 1.0);
}

//subroutine( MainSelection )
void MainShadowMapCreation()
{
	
}

//subroutine( MainSelection )
void RenderScene()
{
	//vec3 lightWeighting = lightingRender();	
	//gl_FragColor = colorRender(lightWeighting);
	vec3 lightWeighting = CalculateSceneLighting_ReleaseBuild();	
	gl_FragColor = CalculateFragmentColor_ReleaseBuild(lightWeighting);
}

void main(void) 
{
	//mainRender();
	RenderScene();
}