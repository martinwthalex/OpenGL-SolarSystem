
#include "Sphere.hpp"
#include "Texture.hpp"
#include <string>
#include <glm.hpp>

namespace udit
{
    class Scene
    {
    private:
        static const std::string   vertex_shader_code;
        static const std::string fragment_shader_code;

        GLint model_view_matrix_id;
        GLint projection_matrix_id;

        GLuint program_id;

        struct Planet
        {
            Sphere* mesh;
            Texture* texture;
            float    orbit_radius;
            float    orbit_speed;
            float    spin_speed;
            float    scale;
        };

        Sphere sphere_hi;// --> una sola esfera para todo

        // texturas

        Texture sun_texture, mercury_texture, venus_texture, earth_texture, mars_texture, jupiter_texture, saturn_texture, uranus_texture, neptune_texture, moon_texture, fobos_texture, deimos_texture, saturn_ring_texture, stars_texture;

        // planetas

        Planet sun;
        Planet mercury;
        Planet venus;
        Planet earth;
        Planet mars;
        Planet jupiter;
        Planet saturn;
        Planet uranus;
        Planet neptune;
        Planet moon;
        Planet fobos; //--> luna de marte
        Planet deimos; //--> luna de marte        

        float angle;

    public:
        
        Scene(int width, int height);

        void update();
        void render();
        void resize(int width, int height);

    private:
        
        GLuint compile_shaders();
        void show_compilation_error(GLuint shader_id);
        void show_linkage_error(GLuint program_id);
        void draw_planet(const Planet& p, const glm::mat4& parent_matrix);
        glm::mat4 compute_matrix(const Planet& p, const glm::mat4& parent_matrix);

    };
}
