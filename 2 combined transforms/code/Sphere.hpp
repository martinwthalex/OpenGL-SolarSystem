
#include <glad/gl.h>

namespace udit
{
    class Sphere
    {
    private:
        enum
        {
            COORDS_VBO,
            UVS_VBO,
            INDICES_EBO,
            VBO_COUNT
        };

        GLuint vbo_ids[VBO_COUNT]{};
        GLuint vao_id = 0;
        GLsizei index_count = 0;

    public:
        Sphere(float radius, unsigned stacks, unsigned slices);
        ~Sphere();

        void render() const;
    };
}
