#pragma once
#include <vector>
#include <iostream>
#include <glm\glm.hpp>
#include <string>
#include <fstream>

class Mesh
{
public:
	std::vector<glm::vec3> vertices_;
	std::vector<glm::ivec3> indices_;
	float max_x_, min_x_, max_y_, min_y_, max_z_, min_z_;

	Mesh(std::string filename);
	void normalize(float, float, float);
	void transform(glm::mat4 model_matrix);
};
