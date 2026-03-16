#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "ObjLoader.h"

void loadMesh(Mesh& outMesh, std::string objFileName, std::string rasterFileName = "")
{
	std::string line;
	std::ifstream meshFile(objFileName);

	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> textures;

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
		else if (prefix == "vt") {
			Vector2 uv;
			if (iss >> uv.x >> uv.y) textures.push_back(uv);
		}
		else if (prefix == "f") {
			std::vector<Vertex> faceVertices;
			std::vector<int> faceTypes;

			std::string vertexData;
			while (iss >> vertexData) {
				std::stringstream vss(vertexData);
				std::string vIdxStr, vtIdxStr, vnIdxStr;

				if (vertexData.find('/') != std::string::npos) {
					// vertex data has at least one '/'

					std::getline(vss, vIdxStr, '/');
					std::getline(vss, vtIdxStr, '/');
					std::getline(vss, vnIdxStr);

					int vIdx = -1, vtIdx = -1, vnIdx = -1;

					if (!vIdxStr.empty()) vIdx = std::stoi(vIdxStr) - 1;
					if (!vtIdxStr.empty())vtIdx = std::stoi(vtIdxStr) - 1;
					if (!vnIdxStr.empty())vnIdx = std::stoi(vnIdxStr) - 1;

					if (vIdx >= 0 && vIdx < (int)positions.size()) {
						bool nFlag = false, tFlag = false; // flags to indicate if a face has a normal or UV mapping

						Vertex v;
						v.position = positions[vIdx];
						if (vnIdx >= 0 && vnIdx < (int)normals.size())
						{
							v.normal = normals[vnIdx];
							nFlag = true;
						}
						if (vtIdx >= 0 && vtIdx < (int)textures.size()) {
							v.uv = { textures[vtIdx].x, 1.0f - textures[vtIdx].y}; // flip image vertically
							tFlag = true;
						}

						if (nFlag && !tFlag) faceTypes.push_back(1); // has a normal but no uv mapping
						else if (tFlag && !nFlag) faceTypes.push_back(2); // has a uv mapping but no normal
						else if (tFlag && nFlag) faceTypes.push_back(3); // has both uv mapping and normal
						else faceTypes.push_back(0); // has no normal or uv mapping

						faceVertices.emplace_back(v);
					}
				}
				else {
					// vertex data has no '/'
					faceTypes.push_back(0); // has no normal or uv mapping

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

				if (faceTypes[i] == 0 || faceTypes[i] == 2) {
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

	Material* material = new Material();

	material->ambient = Vector3(0.0, 0.0, 0.1);
	material->diffuse = Vector3(0.0, 0.0, 0.7);
	material->specular = Vector3(1.0, 1.0, 1.0);
	material->shininess = 32.0f;

	if (rasterFileName != "") {
		material->diffuseMap = new Texture(rasterFileName);
	}

	outMesh.material = material;

	meshFile.close();
}
