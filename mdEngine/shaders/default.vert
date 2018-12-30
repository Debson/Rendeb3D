#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 Weights;

out vec2 TexCoords;
out vec3 FragPos;

const int MAX_BONES = 100;

uniform mat4 gBones[MAX_BONES];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{
	mat4 boneTransform = gBones[BoneIDs[0]] * Weights[0];
	boneTransform	  += gBones[BoneIDs[1]] * Weights[1];
	boneTransform	  += gBones[BoneIDs[2]] * Weights[2];
	boneTransform	  += gBones[BoneIDs[3]] * Weights[3];

	FragPos = vec3(boneTransform * vec4(aPos, 1.0));
	//FragPos = aPos.xyz; 
	TexCoords = aTexCoords;

	gl_Position = projection * view * model * vec4(FragPos, 1.0);
}