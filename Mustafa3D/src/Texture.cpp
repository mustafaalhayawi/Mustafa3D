#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <iostream>

Texture::Texture(std::string file_name)
	: m_file_name(file_name)
{
	m_pixels = stbi_load(m_file_name.c_str(), &m_width, &m_height, &m_channels, 3);

	if (!m_pixels) {
		std::cerr << "error: couldn't load texture " << m_file_name << std::endl;
		m_width = 0;
		m_height = 0;
	}
	else {
		m_channels = 3;
	}
}

Texture::~Texture() {
	if (m_pixels) {
		stbi_image_free(m_pixels);
	}
}

Vector3 Texture::getColor(float u, float v) const {
	if (!m_pixels) return Vector3(1, 0, 1); // if raster image not found return pink

	u = std::max(0.0f, std::min(1.0f, u));
	v = std::max(0.0f, std::min(1.0f, v));

	int x = (int)(u * (m_width - 1));
	int y = (int)(v * (m_height - 1)); // flip v because most images are top-down;

	int index = (y * m_width + x) * m_channels;

	return Vector3(
		m_pixels[index] / 255.0f,
		m_pixels[index + 1] / 255.0f,
		m_pixels[index + 2] / 255.0f
	);
}