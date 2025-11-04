#include "Texture.h"

#include <GLAD/glad.h>
#include <stb/stb_image.h>

// ***********
// * GETTERS *
// ***********
unsigned int Texture::GetID() const
{
	return m_ID;
}

unsigned int Texture::GetUnit() const
{
	return m_Unit;
}

// **********
// * PUBLIC *
// **********
void Texture::Load(const std::string &FilePath, const unsigned int Unit)
{
	m_Unit = Unit;

	int Width = 0;
	int Height = 0;
	int ChannelsCount = 0;

	unsigned char *ImageRawData = stbi_load(FilePath.c_str(), &Width, &Height, &ChannelsCount, 0);
	if (!ImageRawData)
	{
		// Handles image load failure error
		return;
	}

	glGenTextures(1, &m_ID);
	Bind();

	// Sets texture wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Determines the format based on the number of channels
	GLenum Format = 0;
	if (ChannelsCount == 1)
		Format = GL_RED;
	else if (ChannelsCount == 3)
		Format = GL_RGB;
	else if (ChannelsCount == 4)
		Format = GL_RGBA;
	else
	{
		// Handles unsupported format error
		stbi_image_free(ImageRawData);
		return;
	}

	// Uploads the image data to the GPU
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(Format), Width, Height, 0, static_cast<GLint>(Format),
	             GL_UNSIGNED_BYTE, ImageRawData);
	// Generates mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Frees the image data from CPU memory
	stbi_image_free(ImageRawData);
	Unbind();
}

void Texture::Destroy() const
{
	if (m_ID == 0)
		return;
	glDeleteTextures(1, &m_ID);
}

void Texture::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + m_Unit);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
