#ifndef ANIMATION_H
#define ANIMATION_H
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
#include <thread>
#include "Mesh.h"





enum mAnimBehaviour
{
	/** The value from the default node transformation is taken*/
	mAnimBehaviour_DEFAULT = 0x0,

	/** The nearest key value is used without interpolation */
	mAnimBehaviour_CONSTANT = 0x1,

	/** The value of the nearest two keys is linearly
	*  extrapolated for the current time value.*/
	mAnimBehaviour_LINEAR = 0x2,

	/** The animation is repeated.
	*
	*  If the animation key go from n to m and the current
	*  time is t, use the value at (t-n) % (|m-n|).*/
	mAnimBehaviour_REPEAT = 0x3,
};
struct QuatKey
{
	double time;
	glm::quat value;

};

struct VecKey
{
	double time;
	glm::vec3 value;
};
struct mAnimNode
{
	std::string name;
	std::vector<QuatKey> rotations;
	std::vector<VecKey> scales;
	std::vector<VecKey> positions;
	mAnimBehaviour mPreState;
	mAnimBehaviour mPostState;
	std::vector<mAnimNode> mChildren;
	mAnimNode * parent;
};

struct mMeshAnim
{

};

struct mNode
{
	std::string name;
	std::vector<mNode*> mChildren;
	mNode * parent;

};
struct mAnimation
{
	std::string name;
	double mDuration;
	double mTicksPerSecond;
	std::vector<mAnimNode> mChannels;
	std::vector<mMeshAnim> mMeshChannels;

};
struct AnimInfo
{
	std::string name;
	unsigned int id = 0;
	float loopTime = 0.f;
	bool loop = true;
	float anim_rate = 1.0f;
	
};

class Animation
{
public:
	Animation(const aiScene * scene, std::vector<Mesh*> * meshes);
	Animation();
	~Animation();
	void Init(const aiScene * scene, std::vector<Mesh*> * meshes);
	void loadBones(const aiScene * m_scene, const aiMesh * mesh, std::vector<Vertex> * vertices);
	void getBoneTransforms(std::vector<glm::mat4> * transforms);
	void resetAnimation();
	void setCurrentAnimation(unsigned int anim);
	void setLoop(bool loop);
	void setPlayRate(float rate);
	void setLoopStartTime(float time);
	void setFromInfo(AnimInfo info);

private:

	// A bunch of structures that no one else really needs to know of.
	//These are mainly just structures that hold the data from Assimp's internal ones. It streamlines it so I don't 
	//have the bloat that comes with Assimp's structures.
	struct Bone
	{
		unsigned int id;
		std::string name;
		glm::mat4 offset_matrix;

	};

	unsigned int meshindex = 0;





	//Root node containing the layout of the model. Uses simple names for comparisons to grab animation data.
	mNode * root;


	//Should work


	// Recursively process each node, starting with the root of the scene. 
	// @param AnimationTime is just the current time the animation is in, in ticks. 
	// AnimationTime is processed in playAnimation function, to be in terms of TICKS.
	void ReadNodeHeirarchy(float AnimationTime, mNode * pNode, glm::mat4 parent_transforms, std::vector<glm::mat4> * transforms);

	/*   Helper functions inside the interpolation calc functions. Finds the next entry in the aiAnimation's mChannels array  */
	unsigned int FindRotation(float time, const mAnimNode * animNode);
	unsigned int FindPosition(float time, const mAnimNode * animNode);
	unsigned int FindScaling(float time, const mAnimNode * animNode);

	/*  Used to calculate the interpolated postitions between keyframes that are in the aiAnimation's channels. Called in ReadNodeHeirarchy. */
	glm::vec3 CalcInterpolatedPosition(float time, const mAnimNode * pNodeAnim);
	glm::quat CalcInterpolatedRotation(float time, const mAnimNode * pNodeAnim);
	glm::vec3 CalcInterpolatedScale(float time, const mAnimNode * pNodeAnim);

	/* Bone data structures */
	std::vector<Bone> bones;
	std::map<std::string, unsigned int> boneMap;

	/* Inverse bind pose */
	glm::mat4 globalInverseTransform;

	//Animations of the model.
	std::vector<mAnimation> animations;

	mNode * processNode(aiNode * node);
	mAnimation loadAnimation(aiAnimation * anim);
	void deleteNode(mNode * m);
	AnimInfo currentAnimation;


	float start_time = 0;
	float anim_time = 0;

	float TicksPerSecond = 0;
	float TimeInTicks = 0;
	float AnimationTime = 0;
};

#endif