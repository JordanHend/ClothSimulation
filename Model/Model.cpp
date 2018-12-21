#include "Model.h"





void Model::loadModel(std::string path)
{

	this->filename = path;
	Assimp::Importer importer;

	const aiScene * pScene = importer.ReadFile(path, aiProcess_Triangulate |
		aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
);


	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));


	
	{
		int numAnimations = pScene->mNumAnimations;
		
		//ModelTracker::models.insert(std::pair<std::string, Model*>(filename, this));
		// process ASSIMP's root node recursively
		processNode(pScene->mRootNode, pScene);
		// Free the data from the scene that we've processed into our own data structures.
		// I use the scene structure itself to help with managing bone & animation data, so make sure to keep those. 
		if (pScene->mNumMeshes != 0)
			;//delete[] pScene->mMeshes;
		if (pScene->mNumMaterials != 0)
			;//delete[] pScene->mMaterials;
		if (pScene->mNumTextures != 0)
			;//delete[] pScene->mTextures;
	}


	
	




}

void Model::processNode(aiNode * node, const aiScene * scene)
{

	// Process meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh * m = processMesh(mesh, scene);
		if (m->meshname.find("cloth") != std::string::npos)
		{
			Cloth c(m->vertices, m->indices, m->textures);
			cloth.push_back(c);
			free(m);
		}
		else
		meshes.push_back(m);

	}
	// Process the rest of the nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh * Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	// data to fill


	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	AABB meshCollider;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
						  // positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		
		if (vector.x < meshCollider.lowestCoord.x)
		{
			meshCollider.lowestCoord.x = vector.x;
		}
		if (vector.y < meshCollider.lowestCoord.y)
		{
			meshCollider.lowestCoord.y = vector.y;
		}
		if (vector.z < meshCollider.lowestCoord.z)
		{
			meshCollider.lowestCoord.z = vector.z;
		}


		if (vector.x > meshCollider.highestCoord.x)
		{
			meshCollider.highestCoord.x = vector.x;
		}
		if (vector.y > meshCollider.highestCoord.y)
		{
			meshCollider.highestCoord.y = vector.y;
		}
		if (vector.z > meshCollider.highestCoord.z)
		{
			meshCollider.highestCoord.z = vector.z;
		}


		if (vector.x < collider.lowestCoord.x)
		{
			collider.lowestCoord.x = vector.x;
		}
		if (vector.y < collider.lowestCoord.y)
		{
			collider.lowestCoord.y = vector.y;
		}
		if (vector.z < collider.lowestCoord.z)
		{
			collider.lowestCoord.z = vector.z;
		}


		if (vector.x > collider.highestCoord.x)
		{
			collider.highestCoord.x = vector.x;
		}
		if (vector.y > collider.highestCoord.y)
		{
			collider.highestCoord.y = vector.y;
		}
		if (vector.z > collider.highestCoord.z)
		{
			collider.highestCoord.z = vector.z;
		}

		vertex.Position = vector;
		// normals
		if (mesh->mNormals != NULL)
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
		}
		vertex.Normal = vector;
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		// tangent
		if (mesh->mTangents != NULL)
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
		}
		vertex.Tangent = vector;
		// bitangent
		if (mesh->mTangents != NULL)
		{
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
		}
		vertex.Bitangent = vector;



		vertices.push_back(vertex);
	}





	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());



	// If we have a static mesh, aka one that does not move, we have to get its weights to 1 if we're using the same shader we use to calculate rigged models.
	// If we dont, the calculations in the shader will not display the mesh correctly. 



	Mesh * amesh = new Mesh(vertices, indices, textures);
	amesh->collider = meshCollider;
	//amesh.collisionBox = collisionBox;
	/*collisionBox.maxX = 0;
	/collisionBox.maxY = 0;
	collisionBox.maxZ = 0;
	collisionBox.minX = 0;
	collisionBox.minY = 0;
	collisionBox.minZ = 0;
	*/

	amesh->meshname = std::string(mesh->mName.C_Str());
	return amesh;

}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->filename);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void Model::processInverseModel()
{
	//To get the inverse model needed to calculate normals in the shader, we need the 3x3 matrix of the current model matrix.

	// So, as we do in the draw function, we calculate the current model matrix...
	glm::mat4 model;
	model = glm::translate(model, position);
	model = glm::scale(model, scale_vector);
	model *= glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);


	inverseModelMatrix = glm::transpose(glm::inverse(model));

}






Model::Model()
{
}

void Model::Init(const char * path)
{
	loadModel(path);
}

void Model::movePosition(glm::vec3 displacement)
{
	position.x += displacement.x;
	position.y += displacement.y;
	position.z += displacement.z;
}

void Model::changeScale(glm::vec3 newScale)
{
	scale_vector.x = newScale.x;
	scale_vector.y = newScale.y;
	scale_vector.z = newScale.z;

	processInverseModel();
}


void Model::Draw(Shader shader)
{
	shader.use();
	collider.SetFromCenterAndSize(position, collider.Size());
	// Move the model's position to whever it's set to. 
	model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::scale(model, scale_vector);
	model *= glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);
	//processInverseModel();
	shader.setBool("hasAnimations", 0);
	shader.setMat4("model", model);
	shader.setMat3("inverseModel", inverseModelMatrix);
	for (unsigned int i = 0; i < cloth.size(); i++)
		cloth[i].Draw(0, glm::vec3(0), shader);
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->Draw(shader);
	}
}


void Model::DrawShadow(Shader shader)
{

	// Move the model's position to whever it's set to. 
	model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::scale(model, scale_vector);
	model *= glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);


	shader.setMat4("model", model);


	//	shader.setBool("hasAnimations", false);
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->DrawShadow(shader);
	}

	
	
	
}

AABB Model::getCollider()
{
	AABB temp;
	temp.highestCoord = (collider.highestCoord + position);
	temp.lowestCoord = (collider.lowestCoord + position);
	return temp;
}

glm::mat4 Model::getModelMatrix()
{
	glm::mat4 model;

	model = glm::translate(model, position);
	model = glm::scale(model, scale_vector);
	model *= glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);
	return model;
}

std::string SplitFilename(const std::string& str)
{
	std::cout << "Splitting: " << str << '\n';
	std::size_t found = str.find_last_of("/\\");
	return(str.substr(found + 1));
}

std::string CutFilename(const std::string& str)
{

	
	std::size_t found = str.find_last_of("/\\");
	std::string s = str;
	for (unsigned int i = 0; i < str.size() - found; i++)
	{
		s.pop_back();
	}
	s.push_back('/');
	return s;
}


unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = SplitFilename(filename);
	const std::string t = directory;
	std::string p;

	p = CutFilename(t);
	filename = p + filename;
	

	std::string extension = filename.substr(filename.size() - 3, 3);
	if (extension == "dds")
	{
		filename.at(filename.size() - 3) = 'p';
		filename.at(filename.size() - 2) = 'n';
		filename.at(filename.size() - 1) = 'g';
	}


	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void AnimatedModel::resetAnimation()
{
	this->anim.resetAnimation();
}

void AnimatedModel::loadAnimationSet(std::string dir)
{
}

void AnimatedModel::Serialize(std::ofstream * stream)
{
	stream->write((char*)meshes.size(), sizeof(unsigned int));
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->Serialize(stream);
	}
}

void AnimatedModel::FromSerialize(std::ifstream * stream)
{
	unsigned int numMeshes = 0;
	stream->read((char*)numMeshes, sizeof(unsigned int));
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->FromSerialize(stream);
	}

}

void AnimatedModel::setAnimation(AnimInfo anim)
{
	this->anim.setFromInfo(anim);
}

void AnimatedModel::Draw(Shader shader)
{
	shader.setBool("hasAnimations", 1);
	collider.SetFromCenterAndSize(position, collider.Size());
	anim.getBoneTransforms(&transforms);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "aBones"), transforms.size(), GL_FALSE, glm::value_ptr(transforms[0]));
	for (unsigned int i = 0; i < meshes.size(); i++)
	{



		meshes[i]->Draw(shader);
	}
}

Mesh * AnimatedModel::processMesh(aiMesh * mesh, const aiScene * scene)
{
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		AABB meshCollider;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
							  // positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;

			if (vector.x < meshCollider.lowestCoord.x)
			{
				meshCollider.lowestCoord.x = vector.x;
			}
			if (vector.y < meshCollider.lowestCoord.y)
			{
				meshCollider.lowestCoord.y = vector.y;
			}
			if (vector.z < meshCollider.lowestCoord.z)
			{
				meshCollider.lowestCoord.z = vector.z;
			}


			if (vector.x > meshCollider.highestCoord.x)
			{
				meshCollider.highestCoord.x = vector.x;
			}
			if (vector.y > meshCollider.highestCoord.y)
			{
				meshCollider.highestCoord.y = vector.y;
			}
			if (vector.z > meshCollider.highestCoord.z)
			{
				meshCollider.highestCoord.z = vector.z;
			}


			if (vector.x < collider.lowestCoord.x)
			{
				collider.lowestCoord.x = vector.x;
			}
			if (vector.y < collider.lowestCoord.y)
			{
				collider.lowestCoord.y = vector.y;
			}
			if (vector.z < collider.lowestCoord.z)
			{
				collider.lowestCoord.z = vector.z;
			}


			if (vector.x > collider.highestCoord.x)
			{
				collider.highestCoord.x = vector.x;
			}
			if (vector.y > collider.highestCoord.y)
			{
				collider.highestCoord.y = vector.y;
			}
			if (vector.z > collider.highestCoord.z)
			{
				collider.highestCoord.z = vector.z;
			}

			vertex.Position = vector;
			// normals
			if (mesh->mNormals != NULL)
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
			}
			vertex.Normal = vector;
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// tangent
			if (mesh->mTangents != NULL)
			{
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
			}
			vertex.Tangent = vector;
			// bitangent
			if (mesh->mTangents != NULL)
			{
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
			}
			vertex.Bitangent = vector;



			vertices.push_back(vertex);
		}


		anim.loadBones(scene, mesh, &vertices);


		// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());



		// If we have a static mesh, aka one that does not move, we have to get its weights to 1 if we're using the same shader we use to calculate rigged models.
		// If we dont, the calculations in the shader will not display the mesh correctly. 



		Mesh * amesh = new Mesh(vertices, indices, textures);
		amesh->collider = meshCollider;
	

		amesh->meshname = std::string(mesh->mName.C_Str());
		return amesh;

	

}

void AnimatedModel::loadModel(std::string path)
{
	animTimer.start();
	this->filename = path;
	Assimp::Importer importer;

	const aiScene * pScene = importer.ReadFile(path, aiProcess_Triangulate |
		aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
	);


	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));



	{
		int numAnimations = pScene->mNumAnimations;

		//ModelTracker::models.insert(std::pair<std::string, Model*>(filename, this));
		// process ASSIMP's root node recursively
		processNode(pScene->mRootNode, pScene);
		// Free the data from the scene that we've processed into our own data structures.
		// I use the scene structure itself to help with managing bone & animation data, so make sure to keep those. 
		if (pScene->mNumMeshes != 0)
			;//delete[] pScene->mMeshes;
		if (pScene->mNumMaterials != 0)
			;//delete[] pScene->mMaterials;
		if (pScene->mNumTextures != 0)
			;//delete[] pScene->mTextures;
	}

	this->transforms.resize(100);
	this->anim.Init(pScene, &meshes);



}


void AnimatedModel::Init(const char * path)
{
	loadModel(path);
}