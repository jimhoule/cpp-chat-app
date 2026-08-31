#include "Texture.h"

#include <GLAD/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// ***********
// * GETTERS *
// ***********
unsigned int Texture::GetId() const
{
	return m_id;
}

unsigned int Texture::GetUnit() const
{
	return m_unit;
}

// **********
// * PUBLIC *
// **********
void Texture::Load(const std::string &filePath, const unsigned int unit)
{
	m_unit = unit;

	int width = 0;
	int height = 0;
	int channelsCount = 0;

	unsigned char *imageRawData = stbi_load(filePath.c_str(), &width, &height, &channelsCount, 0);
	if (!imageRawData)
	{
		// Handles image load failure error
		return;
	}

	glGenTextures(1, &m_id);
	Bind();

	// Sets texture wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Determines the format based on the number of channels
	GLenum Format = 0;
	if (channelsCount == 1)
		Format = GL_RED;
	else if (channelsCount == 3)
		Format = GL_RGB;
	else if (channelsCount == 4)
		Format = GL_RGBA;
	else
	{
		// Handles unsupported format error
		stbi_image_free(imageRawData);
		return;
	}

	// Uploads the image data to the GPU
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(Format), width, height, 0, static_cast<GLint>(Format),
	             GL_UNSIGNED_BYTE, imageRawData);
	// Generates mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Frees the image data from CPU memory
	stbi_image_free(imageRawData);
	Unbind();
}

void Texture::Destroy() const
{
	if (m_id == 0)
		return;
	glDeleteTextures(1, &m_id);
}

void Texture::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + m_unit);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
