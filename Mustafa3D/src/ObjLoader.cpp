#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "ObjLoader.h"

void loadMesh(Mesh& outMesh, std::string fileName)
{
	std::string line;
	std::ifstream meshFile(fileName);

	std::vector<Vector3> positions;
	std::vector<Vector3> normals;

	while (getline(meshFile, line)) {
		std::istringstream iss(line);
		std::string prefix;
		
		iss >> prefix;

		if (prefix == "v") {
			Vector3 v;
			if (iss >> v.x >> v.y >> v.z) positions.push_back(v);
		}
		else if (prefix == "vn") {
			Vector3 n;
			if (iss >> n.x >> n.y >> n.z) normals.push_back(n);
		}
		else if (prefix == "f") {
			std::vector<Vertex> faceVertices;

			std::string vertexData;
			while (iss >> vertexData) {
				std::stringstream vss(vertexData);
				std::string vIdxStr, vnIdxStr;

				std::getline(vss, vIdxStr, '/');

				if (vss.peek() == '/') vss.ignore();

				std::getline(vss, vnIdxStr);

				int vIdx = std::stoi(vIdxStr) - 1;
				int vnIdx = std::stoi(vnIdxStr) - 1;
				
				if (vIdx >= 0 && vIdx < (int)positions.size() &&
					vnIdx >= 0 && vnIdx < (int)normals.size()) {
					Vertex v = { positions[vIdx], normals[vnIdx] };
					faceVertices.emplace_back(v);
				}
			}

			for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
				int startIdx = outMesh.vertices.size();

				outMesh.vertices.emplace_back(faceVertices[0]);
				outMesh.vertices.emplace_back(faceVertices[i]);
				outMesh.vertices.emplace_back(faceVertices[i + 1]);

				Triangle t = { startIdx + 2, startIdx + 1, startIdx };
				outMesh.triangles.emplace_back(t);
			}
		}
	}

	meshFile.close();
}
