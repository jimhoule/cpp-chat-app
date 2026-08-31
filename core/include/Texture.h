#pragma once

#include <string>

class Texture
{
  public:
	Texture() = default;

	[[nodiscard]] unsigned int GetId() const;
	[[nodiscard]] unsigned int GetUnit() const;

	void Load(const std::string &filePath, const unsigned int unit);
	void Destroy() const;
	void Bind() const;
	void Unbind() const;

  private:
	unsigned int m_id = 0;
	unsigned int m_unit = 0;
};
