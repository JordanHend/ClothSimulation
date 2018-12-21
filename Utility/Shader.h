#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <vector>
class Shader
{
public:

	//Program ID.
	unsigned int ID;
	void init(GLchar* computepath);
	// constructor reads and builds the shader
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);
	// use/activate the shader
	void init(const GLchar* vertexPath, const GLchar* fragmentPath);
	virtual void init(const GLchar* vertexPath,const GLchar* geometryPath, const GLchar* fragmentPath);
	void use();
	// utility uniform functions

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setUInt(const std::string &name, unsigned int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setVec2(const std::string &name, float x, float y) const;
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;
	void setVec4(const std::string &name, const glm::vec4 &value) const;
	void setVec4(const std::string &name, float x, float y, float z, float w);
	void setMat2(const std::string &name, const glm::mat2 &mat) const;
	void setMat3(const std::string &name, const glm::mat3 &mat) const;
	void setMat4(const std::string &name, const glm::mat4 &mat, unsigned int offset = 1) const;
	void setBones(const std::string &name, std::vector<glm::mat4> mat, int size) const;
};



#endif	