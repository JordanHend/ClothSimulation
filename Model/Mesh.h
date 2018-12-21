#ifndef MESH_H
#define MESH_H
//#define string std::string
//#define vector std::vector
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glfw3.h>
#include <string>
#include <vector>
#include <iostream>
#include "../Utility/Shader.h"
#include "AABB.h"
#include <map>
#include "../Utility/Timer.h"

extern Shader compute;
extern Shader clothShader;
extern glm::vec3 force;
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	unsigned int boneID[4] = { 0, 0, 0, 0 };
	float weight[4] = { 0, 0, 0, 0 };

};

struct Face
{
	std::vector<unsigned int> vertexIDs;




};

#ifndef TEXTURES
#define TEXTURES
struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};
#endif
class Mesh
{
public:
	unsigned int VAO;
	std::string meshname;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	AABB collider;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture>);
	void Draw(Shader shader);
	void DrawShadow(Shader shader);
	void Serialize(std::ofstream * stream);
	void FromSerialize(std::ifstream * stream);
private:
	//Render Data
	unsigned int VBO, EBO;

	void setupMesh();


};

struct ComputeParticle
{
	glm::vec4 pos;
	glm::vec4 old_pos;
	glm::vec4 acceleration = glm::vec4(0, 0, 0, 1);
	glm::vec4 accumulated_normal = glm::vec4(0, 0, 0, 0);
};
struct ComputeConstraint
{
	glm::uvec4 Particles = glm::uvec4(0, 0, 0, 0);
	glm::vec4 rest_distance = glm::vec4(0, 0, 0, 0);
};

float vecLength(glm::vec3 f);


class Cloth
{
public:
	unsigned int VAO;
	std::string name;

	std::vector<ComputeParticle> cParticles;
	std::vector<ComputeConstraint> cConstraints;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	void makeConstraint(unsigned int i, unsigned int x)
	{
		ComputeConstraint xo;
		xo.Particles.x = i;
		xo.Particles.y = x;
		 
		glm::vec3 vec = glm::vec3(cParticles[x].pos) - glm::vec3(cParticles[i].pos);
		xo.rest_distance = glm::vec4(glm::length(vec));
		cConstraints.push_back(xo);
	}

	void verticesShared(Face a, Face b);
	Cloth(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	
	void Draw(float runningTime, glm::vec3 model, Shader shader);

	Timer timer;
	void setupCloth();
private:
	//Render Data
	unsigned int VBO, EBO;
	//Shader Storage Buffer Objects for mesh data 
	unsigned int ssbo2, ssbo3;
	unsigned int workGroupSizeX, workGroupSizeY;

	void setUpSSBO();
	
};
#endif