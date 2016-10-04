#include "stdafx.h"
#include "Mesh.h"

#include <map>

Mesh::Mesh(const std::string & fileName)
{
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;

	FILE * file = fopen(fileName.c_str(), "r");

	if (file == 0) {
		throw std::exception("Couldn't open file");
	}

	setvbuf(file, 0, _IOFBF, 1048576);

	char str[256];
	float tempFloat1, tempFloat2, tempFloat3;

	fscanf(file, "%s", str);

	do {
		if (strcmp(str, "v") != 0) {
			if (strcmp(str, "vn") != 0 &&
				strcmp(str, "vt") != 0 &&
				strcmp(str, "f") != 0)
			{
				fgets(str, 256, file);
				continue;
			}
			else break;
		}

		fscanf(file, "%f %f %f", &tempFloat1, &tempFloat2, &tempFloat3);
		positions.push_back(tempFloat1);
		positions.push_back(tempFloat2);
		positions.push_back(tempFloat3);
		fgets(str, 256, file);

	} while (fscanf(file, "%s", str) > 0);

	do {
		if (strcmp(str, "vn") != 0) {
			if (strcmp(str, "vt") != 0 &&
				strcmp(str, "f") != 0)
			{
				fgets(str, 256, file);
				continue;
			}
			else break;
		}

		fscanf(file, "%f %f %f", &tempFloat1, &tempFloat2, &tempFloat3);
		normals.push_back(tempFloat1);
		normals.push_back(tempFloat2);
		normals.push_back(tempFloat3);
		fgets(str, 256, file);

	} while (fscanf(file, "%s", str) > 0);

	do {
		if (strcmp(str, "vt") != 0) {
			if (strcmp(str, "f") != 0)
			{
				fgets(str, 256, file);
				continue;
			}
			else break;
		}

		fscanf(file, "%f %f", &tempFloat1, &tempFloat2);
		uvs.push_back(tempFloat1);
		uvs.push_back(tempFloat2);
		fgets(str, 256, file);

	} while (fscanf(file, "%s", str) > 0);

	{
		std::map<std::tuple<uint32_t, uint32_t, uint32_t>, uint32_t> indexMap;
		std::pair<std::tuple<uint32_t, uint32_t, uint32_t>, uint32_t> tempIndicies;
		uint32_t & posIndex = std::get<0>(tempIndicies.first);
		uint32_t & uvIndex = std::get<1>(tempIndicies.first);
		uint32_t & normalIndex = std::get<2>(tempIndicies.first);
		uint32_t & currentIndex = tempIndicies.second;
		currentIndex = 0;
		do {
			if (strcmp(str, "f") != 0) {
				fgets(str, 256, file);
				continue;
			}
			for (uint32_t i = 0; i < 3; ++i) {
				fscanf(file, "%i/%i/%i", &posIndex, &uvIndex, &normalIndex);
				--posIndex;
				--uvIndex;
				--normalIndex;

				auto result = indexMap.insert(tempIndicies);

				if (result.second) {
					indicies.push_back(currentIndex++);

					Vertex v;
					memcpy(&v.p, &positions[posIndex * 3], 3 * 4);
					v.p.w() = 1.0f;
					memcpy(&v.uv, &uvs[uvIndex * 2], 2 * 4);
					memcpy(&v.n, &normals[normalIndex * 3], 3 * 4);

					verticies.push_back(v);
				}
				else indicies.push_back(result.first->second);
			}
		} while (fscanf(file, "%s", str) > 0);
	}

	fclose(file);
}
