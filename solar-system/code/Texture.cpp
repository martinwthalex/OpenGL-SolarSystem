#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

namespace udit
{
    Texture::Texture(const std::string& path)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(
            path.c_str(),
            &width,
            &height,
            &channels,
            0
        );

        if (!data)
        {
            std::cerr << "ERROR: Cannot load texture: " << path << std::endl;
            return;
        }

        GLenum format = GL_RGB;
        if (channels == 4) format = GL_RGBA;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        // Parámetros básicos (planetas)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }

    Texture::Texture(){}

    Texture::~Texture()
    {
        glDeleteTextures(1, &id);
    }

    void Texture::bind(GLuint unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }
}
