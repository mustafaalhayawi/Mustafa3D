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
			std::vector<int> faceTypes;

			std::string vertexData;
			while (iss >> vertexData) {
				std::stringstream vss(vertexData);
				std::string vIdxStr, vnIdxStr;

				if (vertexData.find('/') != std::string::npos) {
					// vertex data has at least one '/'
					faceTypes.push_back(0);

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
				else {
					// vertex data has no '/'
					faceTypes.push_back(1);

					std::getline(vss, vIdxStr, ' ');

					int vIdx = std::stoi(vIdxStr) - 1;

					if (vIdx >= 0 && vIdx < (int)positions.size()) {
						Vertex v;
						v.position = positions[vIdx];
						faceVertices.emplace_back(v);
					}
				}
			}

			for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
				size_t startIdx = outMesh.vertices.size();

				if (faceTypes[i] == 0) {
					// vertices have position and normal vectors

				}
				else if (faceTypes[i] == 1) {
					// vertices have only position vectors
					Vector3 edgeA = faceVertices[0].position - faceVertices[1].position;
					Vector3 edgeB = faceVertices[1].position - faceVertices[2].position;
					Vector3 normal = Math::crossProduct(edgeA, edgeB);
					normal = Math::normalise(normal);
					
					faceVertices[0].normal = normal;
					faceVertices[i].normal = normal;
					faceVertices[i + 1].normal = normal;
				}

				outMesh.vertices.emplace_back(faceVertices[0]);
				outMesh.vertices.emplace_back(faceVertices[i]);
				outMesh.vertices.emplace_back(faceVertices[i + 1]);

				Triangle t = { (unsigned int)(startIdx + 2), (unsigned int)(startIdx + 1), (unsigned int)startIdx };
				outMesh.triangles.emplace_back(t);
			}
		}
	}

	meshFile.close();
}
