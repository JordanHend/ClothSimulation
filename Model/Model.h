#ifndef MODEL_H
#define MODEL_H

/*
	Model class to handle loading models, and processing their animations if they have any. 

	Starts in contrusctor -> to loading and processing the model. Processes the model, and if it has info such as textures, materials
	it processes those as well. If there is no bones, it doesn't do any transformation calculations, and instead just renders all the meshes.

	Huge shout out to the OGLDev blog (http://ogldev.atspace.co.uk/index.html) for their tutorial on skinning using assimp; while sometimes a bit confusing,
	it was a huge help in getting this working. 

	And shout out to Joey de Vries with his openGL tutorials at (https://learnopengl.com/), it really helped in helping me understand modern openGL.
*/


#include "Mesh.h"
#include <stb_image.h>
#include <assimp\Importer.hpp>
#include "../Utility/Common.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "../Utility/Shader.h"
#include "../Utility/Timer.h"
#include "AABB.h"
#include <thread>
#include "Animation.h"
unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
class Model
{
public:

	
	virtual void Draw(Shader shader);
	virtual void Init(const char * path);
	/*  Model Data */
	std::vector<Texture> textures_loaded;	// Stores all textures loaded, just as a precaution so we don't load textures more than once.
	std::vector<Mesh*> meshes; // Mesh data
	std::vector<Cloth> cloth;
	std::string directory; // Directory of textures
	bool gammaCorrection;
	std::string filename;
	/*  Functions   */
	Model(const char *path)
	{
		
		loadModel(path);
		
	}
	Model();

	// Displaces the model matrix by the amount given by @displacement. Alters model's position vec.
	void movePosition(glm::vec3 displacement);
	// Sets the model's current scale to the input's x y and z scale values.
	void changeScale(glm::vec3 newScale);
	//A Process to add multiple rotations to a model. Adds the rotation to an array, and when calculating model matrix, checks the amount of rotations and applies them. 

	glm::mat4 model;
	//x = pitch, y = yaw, z = roll
	glm::vec3 eulerAngles;
	glm::quat rotation;

	void DrawShadow(Shader shader);
	AABB getCollider();
	glm::mat4 getModelMatrix();
	glm::vec3 position;
	AABB collider;


	glm::vec3 scale_vector = { 1.0f, 1.0f, 1.0f };


	// Model matrix passed to the shaders to calculate the appropriate normal vectors. Needs to change everytime we change the scale to something thats non-uniform, and when we rotate the model.
	glm::mat3 inverseModelMatrix = glm::mat3(0.0f);


protected:
	virtual void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	virtual Mesh * processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		std::string typeName);

	// Sets the global model inverse thats to be set in the shaders. Only called when changing scale/rotation. 
	void processInverseModel();
};

class AnimatedModel : public Model
{
public:
	AnimatedModel()
	{

	};
	AnimatedModel(const char* path)
	{
		
		loadModel(path);

	};
	void Init(const char * path);
	void setAnimation(AnimInfo anim);
	void Draw(Shader shader);
	void resetAnimation();
	void loadAnimationSet(std::string dir);
	void Serialize(std::ofstream * stream);
	void FromSerialize(std::ifstream * stream);
private:
	Mesh * processMesh(aiMesh *mesh, const aiScene *scene);
	void loadModel(std::string path);
	Animation anim;
	std::vector<glm::mat4> transforms;

	//Limit animation fps.
	Timer animTimer;
};





#endif