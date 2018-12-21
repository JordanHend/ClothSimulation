#include "Mesh.h"





Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	//this->indices.resize(indices.size() / 4);

	setupMesh();
}

void Mesh::setupMesh()
{




	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);


	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Setting vertex attribute pointer.


	// Positions

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// Normals

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	// Texture coords


	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);
	// Tangent

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	glEnableVertexAttribArray(3);
	// Bitangent


	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	glEnableVertexAttribArray(4);
	// Bone ID


	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)(offsetof(Vertex, boneID)));
	glEnableVertexAttribArray(5);

	// Bone Weight

	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, weight)));
	glEnableVertexAttribArray(6);




	glBindVertexArray(0);

}

void Mesh::Draw(Shader shader)
{

	

	//Loop through textures, and bind them where appropriate.
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = "Texture_diffuse";
									
		glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
		// Then bind the texture.
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	//Bind VAO and draw
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "GL ERROR! " << err << std::endl;
	}
	// Set back to default
	glActiveTexture(GL_TEXTURE0);

}

void Mesh::DrawShadow(Shader shader)
{
	//Bind VAO and draw
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::Serialize(std::ofstream * stream)
{
	stream->write((char*)vertices.size(), sizeof(unsigned int));
	stream->write((char*)indices.size(), sizeof(unsigned int));
	stream->write((char*)&vertices[0], sizeof(Vertex) * vertices.size());
	stream->write((char*)&indices[0], sizeof(Vertex) * indices.size());
}

void Mesh::FromSerialize(std::ifstream * stream)
{
	unsigned int vsize, isize;
	stream->read((char*)&vsize, sizeof(unsigned int));
	stream->read((char*)&isize, sizeof(unsigned int));

	vertices.resize(vsize);
	indices.resize(isize);
	stream->read((char*)&vertices[0], sizeof(Vertex) * vertices.size());
	stream->read((char*)&indices[0], sizeof(Vertex) * indices.size());
}

void Cloth::verticesShared(Face a, Face b)
{
	// Container for any indices the two faces have in common. 
	std::vector<glm::vec2> traversed;

	// Loop through both face's indices, to see if they match eachother. 
	for (int i = 0; i < a.vertexIDs.size(); i++)
	{
		for (int k = 0; k < b.vertexIDs.size(); k++)
		{

			// If we do get a match, we push a vector into the container containing the two indices of the faces so we know which ones are equal.
			if (a.vertexIDs[i] == b.vertexIDs[k])
			{
				traversed.push_back(glm::vec2(i, k));
			}
		}
		// If we're here, if means we have an edge in common, aka that we have two vertices shared between the two faces.
		if (traversed.size() == 2)
		{
			// Get the adjacent vertices.
			unsigned int face_a_adj_ind = abs(((traversed[0].x) + (traversed[1].x)) - 3) ;
			unsigned int face_b_adj_ind =  abs(((traversed[0].y) + (traversed[1].y)) - 3);
			// Turn the stored ones from earlier and just get the ACTUAL indices from the face. Indices of indices, eh. 
			unsigned int adj_1 = a.vertexIDs[face_a_adj_ind];
			unsigned int adj_2 = b.vertexIDs[face_b_adj_ind];
			// And finally, make a bending spring between the two adjacent particles.
			makeConstraint(adj_1, adj_2);
		}
	}
}



Cloth::Cloth(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->textures = textures;
	this->indices = indices;

	cParticles.resize(vertices.size());


	// Turn vertices into particles.
	for (int j = 0; j < vertices.size(); j++)
	{
		ComputeParticle p;
		p.pos = glm::vec4(vertices[j].Position, 1.0);
		p.old_pos = glm::vec4(vertices[j].Position, 1.0);
		p.accumulated_normal = glm::vec4(vertices[j].Normal, 1.0);

		cParticles[j] = p;
	

	}





	/*
	Indices aren't created properly from assimp (or maybe just this version??)... They will point point to 2 different versices that have the same position, but are different
	in the vertex array. This makes finding which one is shared in a triangle a bit annoying, so i just correct it here.
	
	*/


	// Container to temporarily hold faces while we process springs... need to find out triangles.
	std::vector<Face> faces;
	// Go through indices and take the ones making a triangle.
	for (int i = 0; i < this->indices.size(); i+=3)
	{
			std::vector<unsigned int> faceIds = { this->indices.at(i), this->indices.at(i + 1), this->indices.at(i + 2)};
			Face face;
			face.vertexIDs = faceIds;
			faces.push_back(face);
	}
	
	// Iterate through faces and add constraints when needed.
	for (int l = 0; l < faces.size(); l++)
	{

		// Adding edge springs.
		Face temp = faces[l];
		makeConstraint(temp.vertexIDs[0], temp.vertexIDs[1]);
		makeConstraint(temp.vertexIDs[0], temp.vertexIDs[2]);
		makeConstraint(temp.vertexIDs[1], temp.vertexIDs[2]);

		// We need to get the bending springs as well, and i've just written a function to do that.
		for (int x = 0; x < faces.size(); x++)
		{
			Face temp2 = faces[x];
			if (l != x)
			{

				verticesShared(temp, temp2);

			}

		}
	}


	//Just basically setting something to be unmovable so it doesn't fly off.
	for (unsigned int i = 0; i < cParticles.size(); i++)
	{
		if (cParticles[i].pos.x > 1.2)
			cParticles[i].pos.w = 0;
	}

	// Set up buffers 
	setupCloth();
}
//Error checking, just to make sure nothing goes wrong.
GLenum err;
void Cloth::Draw(float runningTime, glm::vec3 model, Shader shader)
{

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "GL ERROR! " << err << std::endl;

	}
	compute.use();

    glDisable(GL_CULL_FACE);
	compute.setInt("numParticles", cParticles.size());
	compute.setInt("numTriangles", indices.size() / 3);
	compute.setInt("numConstraints", cConstraints.size());

	compute.setVec3("force", force);

	//Bind buffers for compute shader.
	/*
	VBO  -> Particles
	SSBO2 -> Constraints
	SSBO3 -> indices (used to form triangles to update normals.. i don't think I have to use these but i did just to make sure)
	*/
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, VBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo2);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo3);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo3);

	//Compute shader!!
	glDispatchCompute(1,1,1);

	glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

	//Theres no texture used in the mesh, and the material data is hardcoded in the shader, so no need for parsing texture data....


	clothShader.use();
	//Just Bind VAO and draw
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);


	// Set back to default
	glActiveTexture(GL_TEXTURE0);

}

void Cloth::setupCloth()
{

	setUpSSBO();
	clothShader.use();
	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);


	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, cParticles.size() * sizeof(ComputeParticle), &cParticles[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Setting vertex attribute pointer.

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ComputeParticle), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ComputeParticle), (void*)offsetof(ComputeParticle, old_pos));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ComputeParticle), (void*)offsetof(ComputeParticle, acceleration));
	glEnableVertexAttribArray(2);
	
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ComputeParticle), (void*)offsetof(ComputeParticle, accumulated_normal));
	glEnableVertexAttribArray(3);
	/*
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(ComputeParticle), (void*)offsetof(ComputeParticle, texCoord));
	glEnableVertexAttribArray(4);
	*/


	glBindVertexArray(0);


	



}

void Cloth::setUpSSBO()
{
	compute.use();
	//Init ssbo

	//Add data to buffer
	glGenBuffers(1, &ssbo2);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo2);
	glBufferData(GL_SHADER_STORAGE_BUFFER, cConstraints.size() * sizeof(ComputeConstraint), &cConstraints[0], GL_STATIC_DRAW);

	//Add data to buffer
	std::vector<glm::uvec4> cIndices;
	cIndices.resize(indices.size() / 3);
	assert(indices.size() % 3 == 0);
	int j = 0;
	for (unsigned int i = 0; i < cIndices.size(); i++)
	{
		cIndices[i].x = indices[j];
		cIndices[i].y = indices[j+1];
		cIndices[i].z = indices[j+2];
		j += 3;
	}
	glGenBuffers(1, &ssbo3);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo3);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo3);
	glBufferData(GL_SHADER_STORAGE_BUFFER, cIndices.size() * sizeof(glm::uvec4), &cIndices[0], GL_STATIC_DRAW);
}
