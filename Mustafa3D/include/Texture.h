#pragma once
#include "Geometry.h"
#include "stb_image.h"
#include <string>

class Texture {
public:
	Texture(std::string file_name);
	Texture(const Texture&) = delete;
	~Texture();

	Vector3 getColor(float u, float v) const;

private:
	std::string m_file_name;
	int m_width;
	int m_height;
	int m_channels;
	unsigned char* m_pixels = nullptr;
};