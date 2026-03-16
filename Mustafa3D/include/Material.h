#pragma once
#include "Geometry.h"
#include "Texture.h"

struct Material {
	Vector3 ambient, diffuse, specular;
	float shininess;
	Texture* diffuseMap = nullptr;
};