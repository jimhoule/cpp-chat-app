#pragma once

#include <string>

class Texture
{
public:
    Texture() = default;
    Texture(const std::string& FilePath, const unsigned int Unit);
    ~Texture();

    [[nodiscard]] unsigned int GetID() const;
    [[nodiscard]] unsigned int GetUnit() const;

    void Load(const std::string& FilePath, const unsigned int Unit);
    void Bind() const;
    void Unbind() const;

private:
    unsigned int m_ID = 0;
    unsigned int m_Unit = 0;
};
