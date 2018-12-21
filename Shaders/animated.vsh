#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitTangent;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aBoneWeights;


const int MAX_BONES = 100;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 aBones[MAX_BONES];
uniform mat3 inverseModel;
uniform bool hasAnimations;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float visibility;

const float density = 0.002;
const float gradient = 2.0;
void main()
{

vec4 posRelativeToCamera = view * model * vec4(aPos, 1.0);
if(hasAnimations)
{
	mat4 boneTransform = aBones[aBoneIDs[0]] * aBoneWeights[0];
	boneTransform += aBones[aBoneIDs[1]] * aBoneWeights[1];
	boneTransform += aBones[aBoneIDs[2]] * aBoneWeights[2];	
	boneTransform += aBones[aBoneIDs[3]] * aBoneWeights[3];


 	gl_Position = projection * view * model * boneTransform * vec4(aPos, 1.0);
 	FragPos = vec3(model  *  vec4(aPos, 1.0));
 	Normal =  mat3(transpose(inverse(model))) * mat3(boneTransform) *  aNormal;; 
}

else
{
	
	gl_Position = projection * view * model  * vec4(aPos, 1.0);
 	FragPos = vec3(model  *  vec4(aPos, 1.0));
 	Normal =  mat3(transpose(inverse(model)))  *  aNormal;; 



}
TexCoords = aTexCoords;
float distance = length(posRelativeToCamera.xyz);
visibility = exp(-pow((distance*density), gradient));
visibility = clamp(visibility, 0.0, 1.0);
}