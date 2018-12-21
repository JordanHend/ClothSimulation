#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <glad\glad.h>
#include <assimp/cimport.h>
#include <assimp/config.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "GLCamera.h"
#define PI 3.14159265f
#define WIDTH 512
#define HEIGHT 512
#define COLORBITS 32
#define FOV 45.0f
#define ASPECTRATIO (GLfloat) WIDTH / (GLfloat) HEIGHT
#define NEARPLANE 0.01f
#define FARPLANE 100.0f
#define CAMERA_DISTANCE 2.0f
#define ANIMATION_TICKS_PER_SECOND 20.0
#define MULTISAMPLING 4
#define MAXBONESPERMESH 60 //This value has to be changed in the shader code as well, boneMatrices[MAXBONESPERMESH]
#define MAXBONEPERVERTEX 4
#ifndef SCREEN_SIZE
#define SCREEN_SIZE
#define RAD2DEG(a) ((a)*(180.0/PI))
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
#endif
extern GLCamera mainCamera;
using std::string;
using std::cerr;
using std::cout;
using std::endl;


static aiMatrix4x4 GLMMat4ToAi(glm::mat4 mat)
{
	return aiMatrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
		mat[1][0], mat[1][1], mat[1][2], mat[1][3],
		mat[2][0], mat[2][1], mat[2][2], mat[2][3],
		mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
}

static glm::mat4 AiToGLMMat4(aiMatrix4x4& in_mat)
{
	glm::mat4 tmp;
	tmp[0][0] = in_mat.a1;
	tmp[1][0] = in_mat.b1;
	tmp[2][0] = in_mat.c1;
	tmp[3][0] = in_mat.d1;

	tmp[0][1] = in_mat.a2;
	tmp[1][1] = in_mat.b2;
	tmp[2][1] = in_mat.c2;
	tmp[3][1] = in_mat.d2;

	tmp[0][2] = in_mat.a3;
	tmp[1][2] = in_mat.b3;
	tmp[2][2] = in_mat.c3;
	tmp[3][2] = in_mat.d3;

	tmp[0][3] = in_mat.a4;
	tmp[1][3] = in_mat.b4;
	tmp[2][3] = in_mat.c4;
	tmp[3][3] = in_mat.d4;
	return glm::transpose(tmp);
}


static float distance3D(glm::vec3 point, glm::vec3 point2)
{
	return (float)(abs(point2.x - point.x) + abs(point2.y - point.y) + abs(point2.z - point.z)) / 20.f;
}
#endif