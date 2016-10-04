#pragma once


#include <vector>
#include <string>
#include "Triangle.h"

class Mesh
{
public:
	std::vector<Vertex> verticies;
	std::vector<uint32_t> indicies;
	Mesh(const std::string & fileName);
};

