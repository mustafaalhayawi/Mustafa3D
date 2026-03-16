#pragma once
#include <vector>
#include <string>
#include "Mesh.h"
#include "MathUtils.h"
#include "Material.h"
#include <iostream> // temp

void loadMesh(Mesh& outMesh, std::string objFileName, std::string rasterFileName);