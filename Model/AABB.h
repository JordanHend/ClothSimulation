#ifndef AABB_H
#define AABB_H
#include <string>
#include <vector>
#include <glm\glm.hpp>
#include <utility>
#include <iostream>
#include <fstream>
typedef glm::vec3 vec3;

class OBB;
class Ray;
static int NumVerticesInTriangulation(int numFacesX, int numFacesY, int numFacesZ)
{
	return (numFacesX*numFacesY + numFacesX*numFacesZ + numFacesY*numFacesZ) * 2 * 6;
}


class AABB
{
public:
	glm::vec3 lowestCoord = { 5000, 5000, 5000 };
	glm::vec3 highestCoord = { -5000, -5000, -5000 };

	AABB()
	{

	}
	;
	AABB(glm::vec3 center, glm::vec3 size)
	{
		SetFromCenterAndSize(center, size);
	}
	glm::vec3 ExtremePoint(glm::vec3 direction);
	void SetFromCenterAndSize(glm::vec3 center, glm::vec3 size);

	void SetFrom(const OBB &obb);

	glm::vec3 FacePoint(int index, float u, float v) const;

	void Triangulate(int numFacesX, int numFacesY, int numFacesZ, std::vector<vec3> *outPos, bool ccwIsFrontFacing) const;
	
	glm::vec3 CenterPoint() const;
	glm::vec3 Size() const;
	glm::vec3 HalfSize() const;
	void move(glm::vec3 displacement);
	void setPos(glm::vec3 pos);
	glm::mat4 getModel();
	bool Intersects(OBB obb);
	bool Intersects(AABB aabb); 
	bool Intersects(Ray ray);
	bool Intersects(glm::vec3 point);
	void Serialize(std::ofstream * stream);
	void FromSerialize(std::ifstream * stream);
private:
	glm::mat4 model;
};
#endif