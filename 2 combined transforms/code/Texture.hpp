#include <glad/gl.h>
#include <string>

namespace udit
{
    class Texture
    {
        GLuint id = 0;

    public:
        Texture();
        Texture(const std::string& path);
        ~Texture();

        void bind(GLuint unit = 0) const;
    };
}
