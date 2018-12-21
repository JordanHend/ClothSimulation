
#ifndef assume
#define assume __assume
#endif
#include "AABB.h"



glm::vec3 AABB::ExtremePoint(glm::vec3 direction)
{

	return glm::vec3((direction.x >= 0.f ? highestCoord.x : lowestCoord.x),
		(direction.y >= 0.f ? highestCoord.y : lowestCoord.y),
		(direction.z >= 0.f ? highestCoord.z : lowestCoord.z));
}

void AABB::SetFromCenterAndSize(glm::vec3 center, glm::vec3 size)
{

	vec3 halfSize = 0.5f * size;
	lowestCoord = center - halfSize;
	highestCoord = center + halfSize;

}

glm::vec3 AABB::FacePoint(int index, float u, float v) const
{
	assume(0 <= index && index <= 5);
	assume(0 <= u && u <= 1.f);
	assume(0 <= v && v <= 1.f);

	vec3 d = highestCoord - lowestCoord;
	switch (index)
	{
	default:
		//
	case 0: return glm::vec3(lowestCoord.x, lowestCoord.y + u * d.y, lowestCoord.z + v * d.z);
	case 1: return glm::vec3(highestCoord.x, lowestCoord.y + u * d.y, lowestCoord.z + v * d.z);
	case 2: return glm::vec3(lowestCoord.x + u * d.x, lowestCoord.y, lowestCoord.z + v * d.z);
	case 3: return glm::vec3(lowestCoord.x + u * d.x, highestCoord.y, lowestCoord.z + v * d.z);
	case 4: return glm::vec3(lowestCoord.x + u * d.x, lowestCoord.y + v * d.y, lowestCoord.z);
	case 5: return glm::vec3(lowestCoord.x + u * d.x, lowestCoord.y + v * d.y, highestCoord.z);


	}
}
void AABB::Triangulate(int numFacesX, int numFacesY, int numFacesZ, std::vector<vec3> *outPos, bool ccwIsFrontFacing) const
{
	assume(numFacesX >= 1);
	assume(numFacesY >= 1);
	assume(numFacesZ >= 1);

	assume(outPos);
	if (!outPos)
		return;

	// Generate both X-Y planes.
	int i = 0;
	for (int face = 0; face < 6; ++face) // Faces run in the order -X, +X, -Y, +Y, -Z, +Z.
	{
		int numFacesU;
		int numFacesV;
		bool flip = (face == 1 || face == 2 || face == 5);
		if (ccwIsFrontFacing)
			flip = !flip;
		if (face == 0 || face == 1)
		{
			numFacesU = numFacesY;
			numFacesV = numFacesZ;
		}
		else if (face == 2 || face == 3)
		{
			numFacesU = numFacesX;
			numFacesV = numFacesZ;
		}
		else// if (face == 4 || face == 5)
		{
			numFacesU = numFacesX;
			numFacesV = numFacesY;
		}
		for (int x = 0; x < numFacesU; ++x)
			for (int y = 0; y < numFacesV; ++y)
			{
				float u = (float)x / (numFacesU);
				float v = (float)y / (numFacesV);
				float u2 = (float)(x + 1) / (numFacesU);
				float v2 = (float)(y + 1) / (numFacesV);

				outPos->at(i) = FacePoint(face, u, v);
				outPos->at(i + 1) = FacePoint(face, u, v2);
				outPos->at(i + 2) = FacePoint(face, u2, v);
				if (flip)
				{
					vec3 temp = outPos->at(i + 1);
					outPos->at(i + 1) = outPos->at(i + 2);
					outPos->at(i + 2) = temp;

				}
				//Swap(outPos[i + 1], outPos[i + 2]);

				outPos->at(i + 3) = outPos->at(i + 2);
				outPos->at(i + 4) = outPos->at(i + 1);
				outPos->at(i + 5) = FacePoint(face, u2, v2);


				i += 6;
			}
	}
	// assert(i == NumVerticesInTriangulation(numFacesX, numFacesY, numFacesZ));
}

void AABB::move(glm::vec3 displacement)
{
	SetFromCenterAndSize(CenterPoint() + displacement, Size());
}

void AABB::setPos(glm::vec3 center)
{
	glm::vec3 halfSize = (highestCoord - lowestCoord) * 0.5f;

	lowestCoord = center - halfSize;
	highestCoord = center + halfSize;
}

glm::mat4 AABB::getModel()
{
	return glm::mat4();
}

glm::vec3 AABB::CenterPoint() const
{
	return (lowestCoord + highestCoord) * 0.5f;
}

glm::vec3 AABB::Size() const
{
	return highestCoord - lowestCoord;
}

glm::vec3 AABB::HalfSize() const
{
	return Size() * 0.5f;
}


bool AABB::Intersects(glm::vec3 point)
{
	glm::vec3 min = lowestCoord;
	glm::vec3 max = highestCoord;

	if (point.x < min.x || point.y < min.y || point.z < min.z) {
		return false;
	}
	if (point.x > max.x || point.y > max.y || point.z > max.z) {
		return false;
	}

	return true;
}

void AABB::Serialize(std::ofstream * stream)
{

	if (*stream)
	{

		stream->write((char*)&lowestCoord, sizeof(glm::vec3));
		stream->write((char*)&highestCoord, sizeof(glm::vec3));
	}
	else
	{
		std::cout << "Error saving AABB!" << std::endl;
	}
}

void AABB::FromSerialize(std::ifstream * stream)
{
	if (*stream)
	{
		stream->read(reinterpret_cast<char*>(&this->lowestCoord), sizeof(glm::vec3));
		stream->read(reinterpret_cast<char*>(&this->highestCoord), sizeof(glm::vec3));
		//origin = lowestCoord + HalfSize();

	}
	else
	{
		std::cout << "Error loading AABB!" << std::endl;
	}
}
