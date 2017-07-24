#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 aVertexPosition;

// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;
uniform mat4 modelMat;

void main(){
	gl_Position =  depthMVP * modelMat * vec4(aVertexPosition,1);
}

