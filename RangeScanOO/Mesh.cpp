#include "Mesh.h"

Mesh::Mesh(std::string filename)
{
	std::ifstream mesh_file(filename);
	std::cout << "read obj file..." << filename << std::endl;

	char flag;
	float x, y, z;
	float max_x, max_y, max_z;
	float min_x, min_y, min_z;
	max_x = max_y = max_z = -1000;
	min_x = min_y = min_z = 1000;

	float avgx, avgy, avgz;
	avgx = avgy = avgz = 0;

	std::string line;
	while (std::getline(mesh_file, line))
	{
		sscanf(line.c_str(), "%c %f %f %f", &flag, &x, &y, &z);
		if (flag == 'v')
		{
			z = z*-1;
			vertices_.push_back(glm::vec3(x, y, z));
			if (x > max_x)
				max_x = x;
			if (x < min_x)
				min_x = x;

			if (y > max_y)
				max_y = y;
			if (y < min_y)
				min_y = y;

			if (z > max_z)
				max_z = z;
			if (z < min_z)
				min_z = z;

			avgx += x;
			avgy += y;
			avgz += z;

		}
		else if (flag == '#')
			std::cout << line << std::endl;
		else if (flag == 'f')
		{
			indices_.push_back(glm::ivec3(x - 1, y - 1, z - 1));
		}
		else
			std::cout << line << std::endl;
	}

	mesh_file.close();
	std::cout << "vertices number: " << vertices_.size() << std::endl;
	std::cout << "faces number: " << indices_.size() << std::endl;

	max_x_ = max_x;
	max_y_ = max_y;
	max_z_ = max_z;

	min_x_ = min_x;
	min_y_ = min_y;
	min_z_ = min_z;

	avgx = avgx / vertices_.size();
	avgy = avgy / vertices_.size();
	avgz = avgz / vertices_.size();

	normalize(avgx, avgy, avgz);
}

void Mesh::normalize(float avg_x, float avg_y, float avg_z)
{
	float length = std::fmaxf(max_x_ - min_x_, max_y_ - min_y_);
	float scale = 290.0 / length;
	// scale
	float transx = 200 - avg_x * scale;
	float transy = 200 - avg_y * scale;

	for (int i = 0; i < vertices_.size(); i++)
	{
		vertices_[i] = vertices_[i] * scale + glm::vec3(transx, transy, 0);

	}
}

void Mesh::transform(glm::mat4 model_matrix)
{
	
	// 此时以200,200为中心
	for (int i = 0; i < vertices_.size(); i++)
	{
		glm::vec4 temp = glm::vec4(vertices_[i].x, vertices_[i].y, vertices_[i].z, 1);
			
		temp = model_matrix * (temp - glm::vec4(200, 200, 0, 0)) + glm::vec4(200, 200, 0, 0);
		vertices_[i] = glm::vec3(temp.x, temp.y, temp.z);
	}


}