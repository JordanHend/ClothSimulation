#include "Animation.h"

Animation::Animation(const aiScene * scene, std::vector<Mesh*> * meshes)
{
	
	if (scene->HasAnimations())
	{
		
		// Get global inverse bind pose from node's root transformation
		globalInverseTransform = glm::inverse(AiToGLMMat4(scene->mRootNode->mTransformation));
		//loadBones(scene,scene->mRootNode, meshes);
	}
	else
	{
		std::cout << "Model does not have animations and you are attempting to process animations on it!" << std::endl;
	}
	animations.resize(scene->mNumAnimations);
	for (unsigned int i = 0; i < scene->mNumAnimations; i++)
	{
		animations[i] = (loadAnimation(scene->mAnimations[i]));
	}

	if (scene->mRootNode != NULL)
		root = processNode(scene->mRootNode);
}

Animation::Animation()
{
}

void Animation::deleteNode(mNode * node)
{

	if (node->mChildren.size() != 0)
		for (unsigned int i = 0; i < node->mChildren.size(); i++)
		{
			deleteNode(node->mChildren[i]);
			delete(node->mChildren[i]);

		}


}

Animation::~Animation()
{
	deleteNode(root);

	
}

void Animation::Init(const aiScene * scene, std::vector<Mesh*>* vertices)
{

		
		// Get global inverse bind pose from node's root transformation
		globalInverseTransform = glm::inverse(AiToGLMMat4(scene->mRootNode->mTransformation));
		//loadBones(scene,scene->mRootNode, vertices);
	

	animations.resize(scene->mNumAnimations);
	for (unsigned int i = 0; i < scene->mNumAnimations; i++)
	{
		animations[i] = (loadAnimation(scene->mAnimations[i]));
	}

	if (scene->mRootNode != NULL)
		root = processNode(scene->mRootNode);
}

void Animation::getBoneTransforms(std::vector<glm::mat4>* transforms)
{
	//I believe this code works 100%

	anim_time += ((glfwGetTime() - start_time) * currentAnimation.anim_rate);
	TicksPerSecond = animations[currentAnimation.id].mTicksPerSecond != 0 ? animations[currentAnimation.id].mTicksPerSecond : 25.0f;
	TimeInTicks = anim_time * TicksPerSecond;
	AnimationTime = TimeInTicks;

	ReadNodeHeirarchy(std::min(AnimationTime, (float)animations[currentAnimation.id].mDuration - 1), root, glm::mat4(1.0f), transforms);

	if (TimeInTicks >= animations[currentAnimation.id].mDuration - 1 && currentAnimation.loop)
	{
		
	
		anim_time = 0;
	
	}
	start_time = glfwGetTime();;
}

void Animation::resetAnimation()
{
	anim_time = 0;
}

void Animation::setFromInfo(AnimInfo info)
{
	
		this->currentAnimation = info;

	
}

void Animation::loadBones(const aiScene * m_scene, const aiMesh * mesh, std::vector<Vertex> * vertices)
{


	for (unsigned int i = 0; i < mesh->mNumBones; i++) {
		unsigned int BoneIndex = 0;
		std::string BoneName(mesh->mBones[i]->mName.data);

		// Check to see if this bone has been processed already or not
		if (boneMap.find(BoneName) == boneMap.end()) {

			// If it hasn't, add it to the list to get processed.	
			Bone bi = {};
			BoneIndex = bones.size();
			bones.push_back(bi);

		}
		else {
			// We've already seen this bone, so the index is where it was at previously.
			BoneIndex = boneMap[BoneName];
		}


		// Get bone index from map. 
		boneMap[BoneName] = BoneIndex;

		// Set bone offset matrix
		bones.at(BoneIndex).offset_matrix = (AiToGLMMat4(mesh->mBones[i]->mOffsetMatrix));

		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;

			float Weight = mesh->mBones[i]->mWeights[j].mWeight;
			for (unsigned int k = 0; k < 4; k++)
			{

				if (vertices->at(VertexID).weight[k] == 0)
				{
					vertices->at(VertexID).boneID[k] = BoneIndex;
					vertices->at(VertexID).weight[k] = Weight;
					break;
				}

			}
		}
	}
}
//Proccessing ASSIMP's data structures.

mNode * Animation::processNode(aiNode * node)
{
	mNode * m = new mNode();
	if (node->mNumChildren != 0)
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			mNode *  x = (processNode(node->mChildren[i]));
			
			x->parent = m;
			m->mChildren.push_back(x);
		}
	
	m->name = std::string(node->mName.C_Str());


	return m;
}

mAnimation Animation::loadAnimation(aiAnimation * anim)
{
	mAnimation m;
	m.name = std::string(anim->mName.data);
	m.mDuration = anim->mDuration;
	m.mTicksPerSecond = anim->mTicksPerSecond;

	for (unsigned int i = 0; i < anim->mNumChannels; i++)
	{
		mAnimNode node;
		
		node.name = std::string(anim->mChannels[i]->mNodeName.C_Str());

		for (unsigned int j = 0; j < anim->mChannels[i]->mNumPositionKeys; j++)
		{
			VecKey k;
			k.time = anim->mChannels[i]->mPositionKeys[j].mTime;
			k.value = glm::vec3(anim->mChannels[i]->mPositionKeys[j].mValue.x, anim->mChannels[i]->mPositionKeys[j].mValue.y, anim->mChannels[i]->mPositionKeys[j].mValue.z);
			node.positions.push_back(k);
		}

		for (unsigned int j = 0; j < anim->mChannels[i]->mNumScalingKeys; j++)
		{
			VecKey k;
			k.time = anim->mChannels[i]->mScalingKeys[j].mTime;
			k.value = glm::vec3(anim->mChannels[i]->mScalingKeys[j].mValue.x, anim->mChannels[i]->mScalingKeys[j].mValue.y, anim->mChannels[i]->mScalingKeys[j].mValue.z);
			node.scales.push_back(k);
		}

		for (unsigned int j = 0; j < anim->mChannels[i]->mNumRotationKeys; j++)
		{
			QuatKey k;
			k.time = anim->mChannels[i]->mRotationKeys[j].mTime;
			k.value.x = anim->mChannels[i]->mRotationKeys[j].mValue.x;
			k.value.y = anim->mChannels[i]->mRotationKeys[j].mValue.y;
			k.value.z = anim->mChannels[i]->mRotationKeys[j].mValue.z;
			k.value.w = anim->mChannels[i]->mRotationKeys[j].mValue.w;
			node.rotations.push_back(k);
		}
		m.mChannels.push_back(node);
	}
	return m;
}


//Actual skinning being done.

glm::quat Animation::CalcInterpolatedRotation(float time, const mAnimNode * pNodeAnim)
{
	glm::quat out;

	// we need at least two values to interpolate...
	if (pNodeAnim->rotations.size() == 1) {
		out = pNodeAnim->rotations[0].value;
		return out;
	}

	unsigned int RotationIndex = FindRotation(time, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->rotations.size());
	double deltaTime = pNodeAnim->rotations[NextRotationIndex].time - pNodeAnim->rotations[RotationIndex].time;
	float Factor = (time - (float)pNodeAnim->rotations[RotationIndex].time) / deltaTime;
	
	const glm::quat StartRotationQ = pNodeAnim->rotations[RotationIndex].value;
	const glm::quat EndRotationQ = pNodeAnim->rotations[NextRotationIndex].value;

	out = glm::slerp( StartRotationQ, EndRotationQ, Factor);


	return out;

}
glm::vec3 Animation::CalcInterpolatedScale(float time, const mAnimNode * pNodeAnim)
{

	//If there's only one keyframe for the position, we return it.
	//This avoids any possible out of range errors later on in the function
	//as we're dealing with an array.
	if (pNodeAnim->scales.size() == 1)
	{
		glm::vec3 val = pNodeAnim->scales[0].value;
		return val;
	}

	//The index of our current position, as well as the index that follows.
	//This will allow us to interpolate between the two values.
	unsigned int ScalingIndex = FindScaling(time, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);

	//DeltaTime is the amount of time between the two keyframes.
	float DeltaTime = pNodeAnim->scales[NextScalingIndex].time - pNodeAnim->scales[ScalingIndex].time;
	//The factor is simply how much time has passed since the last keyframe,
	//divided by the amount of time between the two keyframes (the DeltaTime)
	//to get the percentage, or how far along between the two keyframes we are.
	float Factor = (time - (float)pNodeAnim->scales[ScalingIndex].time) / DeltaTime;

	//The start and end positions (the position values of each of the keyframes)
	const glm::vec3 Start = pNodeAnim->scales[ScalingIndex].value;
	const glm::vec3 End = pNodeAnim->scales[NextScalingIndex].value;

	//and here we linearly interpolate between the two keyframes.
	glm::vec3 val = glm::mix(Start, End, Factor);

	return val;


}

glm::vec3 Animation::CalcInterpolatedPosition(float time, const mAnimNode * pNodeAnim)
{

	glm::vec3 Out;
	// we need at least two values to interpolate...
	if (pNodeAnim->positions.size() == 1) {
		Out = pNodeAnim->positions[0].value;
		return glm::vec3(Out.x, Out.y, Out.z);
	}

	unsigned int Index = FindPosition(time, pNodeAnim);
	unsigned int NextIndex = (Index + 1);
	assert(NextIndex < pNodeAnim->positions.size());
	double deltaTime = pNodeAnim->positions[NextIndex].time - pNodeAnim->positions[Index].time;
	float Factor = (time - (float)pNodeAnim->positions[Index].time) / deltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::vec3 start = pNodeAnim->positions[Index].value;
	const glm::vec3 end = pNodeAnim->positions[NextIndex].value;
	glm::vec3 out = glm::mix(start, end, Factor);
	//glm::normalize(out);

	return out;


}

unsigned int Animation::FindScaling(float time, const mAnimNode * animNode)
{

	for (unsigned int i = 0; i < animNode->scales.size() - 1; i++) {
		if (time < (float)animNode->scales[i + 1].time) {
			return i;
		}
	}

	return 0;
}

unsigned int Animation::FindPosition(float time, const mAnimNode * animNode)
{
	for (unsigned int i = 0; i < animNode->positions.size() - 1; i++)
	{
		//If the time passed in is less than the time of the next
		//keyframe, then this is the keyframe we want!
		if (time < (float)animNode->positions[i + 1].time)
			return i;
	}
	return 0;
}

unsigned int Animation::FindRotation(float time, const mAnimNode * animNode)
{
	for (unsigned int i = 0; i < animNode->rotations.size() - 1; i++)
	{
		//Same as with the position.
		if (time < (float)animNode->rotations[i + 1].time)
			return i;
	}
	return 0;
}

void Animation::ReadNodeHeirarchy(float AnimationTime, mNode * pNode, glm::mat4 parent_transforms, std::vector<glm::mat4> * transforms)
{
	std::string NodeName(pNode->name);

	 mAnimation* pAnimation = &animations[currentAnimation.id];
	
	glm::mat4 NodeTransformation(1.0f);
	glm::mat4 GlobalTransformation;
	const mAnimNode* pNodeAnim = NULL;
	for (int i = 0; i < pAnimation->mChannels.size(); i++)
	{
		if(pAnimation->mChannels[i].name == NodeName)
		{
			pNodeAnim = &pAnimation->mChannels[i];
		}
	}

	if (pNodeAnim)
	{
		glm::vec3 pos = CalcInterpolatedPosition(AnimationTime, pNodeAnim);
		glm::quat rot = CalcInterpolatedRotation(AnimationTime, pNodeAnim);// glm::quat(0, 0, 0, 0);//
		glm::vec3 scale = CalcInterpolatedScale(AnimationTime, pNodeAnim);


		glm::mat4 mat(1.0f);
		mat *= glm::translate(pos);
		mat *= glm::toMat4(rot);
		mat *= glm::scale(scale);

		NodeTransformation = mat;
	}
 	GlobalTransformation = parent_transforms * NodeTransformation;
	if (boneMap.find(NodeName) != boneMap.end())
	{
		int boneIndex = boneMap[NodeName];
		GlobalTransformation = parent_transforms * NodeTransformation;
		transforms->at(boneIndex) = (globalInverseTransform * GlobalTransformation * bones[boneIndex].offset_matrix);
	}
	

	for (unsigned int j = 0; j < pNode->mChildren.size(); j++)
	{
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[j], GlobalTransformation, transforms);
	}

}