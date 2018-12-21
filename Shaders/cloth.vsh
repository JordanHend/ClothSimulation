#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aOld_Pos;
layout (location = 2) in vec4 aAcceleration;
layout (location = 3) in vec4 aNormal;


const int MAX_BONES = 100;
uniform bool hasAnimations;
uniform mat4 model;
uniform mat4 MVPMatrix;
uniform mat4 aBones[MAX_BONES];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{

Normal = aNormal.xyz;
FragPos = vec3(model  *  vec4(aPos.xyz, 1.0));
TexCoords = vec2(0, 0);
gl_Position = MVPMatrix  * vec4(aPos.xyz, 1.0);


}