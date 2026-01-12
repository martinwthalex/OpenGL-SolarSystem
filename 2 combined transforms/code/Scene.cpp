
#pragma once

#include "Scene.hpp"

#include <iostream>
#include <cassert>

#include <glm.hpp>                          // vec3, vec4, ivec4, mat4
#include <gtc/matrix_transform.hpp>         // translate, rotate, scale, perspective
#include <gtc/type_ptr.hpp>                 // value_ptr

namespace udit
{
    
    using namespace std;

    glm::mat4 projection_matrix;

    const string Scene::vertex_shader_code =

        "#version 330\n"
        ""
        "uniform mat4 model_view_matrix;\n"
        "uniform mat4 projection_matrix;\n"
        ""
        "layout (location = 0) in vec3 vertex_coordinates;\n"
        "layout (location = 1) in vec2 vertex_uv;\n"
        ""
        "out vec2 uv;\n"
        ""
        "void main()\n"
        "{\n"
        "    gl_Position = projection_matrix * model_view_matrix * vec4(vertex_coordinates, 1.0);\n"
        "    uv = vertex_uv;\n"
        "}\n";

    const string Scene::fragment_shader_code =
        "#version 330\n"
        ""
        "in  vec2 uv;\n"
        "out vec4 fragment_color;\n"
        ""
        "uniform sampler2D color_texture;\n"
        ""
        "void main()\n"
        "{\n"
        "    fragment_color = texture(color_texture, uv);\n"
        "}\n";

    Scene::Scene(int width, int height)
        :
        // Se crea una única esfera base en vez de usar una para cada planeta
        sphere_hi(1.f, 32, 64),

        // texturas
        sun_texture("../../../shared/assets/sun.jpg"),
        mercury_texture("../../../shared/assets/mercury.jpg"),
        venus_texture("../../../shared/assets/venus.jpg"),
        earth_texture("../../../shared/assets/earth.jpg"),        
        mars_texture("../../../shared/assets/mars.jpg"),
        jupiter_texture("../../../shared/assets/jupiter.jpg"),
        saturn_texture("../../../shared/assets/saturn.jpg"),
        uranus_texture("../../../shared/assets/uranus.jpg"),
        neptune_texture("../../../shared/assets/neptune.jpg"),
        moon_texture("../../../shared/assets/moon.jpg"),
        saturn_ring_texture("../../../shared/assets/saturn-ring.png"),
        stars_texture("../../../shared/assets/stars.jpg"),
        fobos_texture("../../../shared/assets/fobos.jpg"),
        deimos_texture("../../../shared/assets/deimos.jpg"),        

        angle(0.f)
    {
        
        //Ajustes para los anillos de jupiter

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glClearColor(.2f, .2f, .2f, 1.f);

        program_id = compile_shaders();
        glUseProgram(program_id);

        model_view_matrix_id = glGetUniformLocation(program_id, "model_view_matrix");
        projection_matrix_id = glGetUniformLocation(program_id, "projection_matrix");

        glUniform1i(glGetUniformLocation(program_id, "color_texture"), 0);

        
        // planetas (datos) --> no son los reales, para que se aprecien todos y sus movimientos

        sun = { &sphere_hi, &sun_texture,      0.f, 0.f, 0.2f, 3.0f };
        mercury = { &sphere_hi, &mercury_texture,  4.f, 4.1f, 2.0f, 0.35f };
        venus = { &sphere_hi, &venus_texture,    6.f, 1.6f, 1.5f, 0.55f };
        earth = { &sphere_hi, &earth_texture,    8.f, 1.0f, 3.0f, 0.60f };
        mars = { &sphere_hi, &mars_texture,    10.f, 1.8f,2.8f, 0.45f };
        jupiter = { &sphere_hi, &jupiter_texture,    11.5f, 0.6f, 1.2f, 1.30f };
        saturn = { &sphere_hi, &saturn_texture,    14.5f, 1.2f, 1.1f, 1.10f };
        uranus = { &sphere_hi, &uranus_texture,    17.5f, 0.8f, 0.9f, 0.90f };
        neptune = { &sphere_hi, &neptune_texture,    20.5f, 1.0f, 0.8f, 0.85f };
        moon = { &sphere_hi, &moon_texture, 2.0f, 4.0f, 2.5f, 0.4f };
        fobos = { &sphere_hi, &fobos_texture, 1.2f, 5.5f, 2.0f, 0.4f };
        deimos = { &sphere_hi, &deimos_texture, 1.8f, 3.2f, 1.5f, 0.4f };
    
        resize(width, height);

    }

    void Scene::update ()
    {
        angle += 0.01f;
    }

    void Scene::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Se dibuja la esfera atmósfera de estrellas

        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);// --> para poder ver el interior de la esfera


        glm::mat4 sky_mv(1.0f);

        sky_mv = glm::rotate(sky_mv, angle * 0.02f, glm::vec3(0.f, 1.f, 0.f));
        sky_mv = glm::scale(sky_mv, glm::vec3(200.0f));

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, &sky_mv[0][0]);
        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, &projection_matrix[0][0]);

        stars_texture.bind(0);
        sphere_hi.render();

        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);


        // base del sistema solar (cámara)

        glm::mat4 solar_base =
            glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -20.f));

        // sol

        draw_planet(sun, solar_base);

        // planetas interiores

        draw_planet(mercury, solar_base);
        draw_planet(venus, solar_base);

        glm::mat4 earth_matrix = compute_matrix(earth, solar_base);

        // La Tierra Y La luna

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(earth_matrix));
        earth_texture.bind(0);
        sphere_hi.render();
        draw_planet(moon, earth_matrix);

        draw_planet(earth, solar_base);

        // Marte y sus 2 lunas interiores Phobos y Deimos

        glm::mat4 mars_matrix = compute_matrix(mars, solar_base);

        // Marte
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(mars_matrix));
        mars_texture.bind(0);
        sphere_hi.render();
        draw_planet(fobos, mars_matrix);
        draw_planet(deimos, mars_matrix);

        // Jupiter

        draw_planet(jupiter, solar_base);

        // Saturno y sus anillos

        glm::mat4 saturn_matrix = compute_matrix(saturn, solar_base);

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(saturn_matrix));
        saturn_texture.bind(0);
        sphere_hi.render();

        glm::mat4 ring_m = saturn_matrix;

        // Aplastamos los anillos en el eje y para formar un disco

        ring_m = glm::scale(ring_m, glm::vec3(2.2f, 0.05f, 2.2f));

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(ring_m));

        saturn_ring_texture.bind(0);

        // evitar que desaparezca por culling

        glDisable(GL_CULL_FACE);
        sphere_hi.render();
        glEnable(GL_CULL_FACE);

        // Urano y Neptuno

        draw_planet(uranus, solar_base);
        draw_planet(neptune, solar_base);
    }


    void Scene::draw_planet(const Planet& p, const glm::mat4& parent_matrix)
    {
        glm::mat4 m(1.f);

        // órbita (si orbit_radius == 0, es el sol)

        if (p.orbit_radius != 0.f)
        {
            m = glm::rotate(m, angle * p.orbit_speed, glm::vec3(0.f, 1.f, 0.f));
            m = glm::translate(m, glm::vec3(p.orbit_radius, 0.f, 0.f));
        }

        // rotación propia

        m = glm::rotate(m, angle * p.spin_speed, glm::vec3(0.f, 1.f, 0.f));

        // escala

        m = glm::scale(m, glm::vec3(p.scale));

        // hereda del padre

        m = parent_matrix * m;

        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(m));
        p.texture->bind(0);
        p.mesh->render();
    }


    void Scene::resize (int width, int height)
    {
        projection_matrix = glm::perspective (20.f, GLfloat(width) / height, 1.f, 5000.f);

        glUniformMatrix4fv (projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        glViewport (0, 0, width, height);
    }

    glm::mat4 Scene::compute_matrix(const Planet& p, const glm::mat4& parent_matrix)
    {
        glm::mat4 m(1.f);

        if (p.orbit_radius != 0.f)
        {
            m = glm::rotate(m, angle * p.orbit_speed, glm::vec3(0.f, 1.f, 0.f));
            m = glm::translate(m, glm::vec3(p.orbit_radius, 0.f, 0.f));
        }

        m = glm::rotate(m, angle * p.spin_speed, glm::vec3(0.f, 1.f, 0.f));
        m = glm::scale(m, glm::vec3(p.scale));

        return parent_matrix * m;
    }


    GLuint Scene::compile_shaders ()
    {
        GLint succeeded = GL_FALSE;

        // Se crean objetos para los shaders:

        GLuint   vertex_shader_id = glCreateShader (GL_VERTEX_SHADER  );
        GLuint fragment_shader_id = glCreateShader (GL_FRAGMENT_SHADER);

        // Se carga el código de los shaders:

        const char *   vertex_shaders_code[] = {          vertex_shader_code.c_str () };
        const char * fragment_shaders_code[] = {        fragment_shader_code.c_str () };
        const GLint    vertex_shaders_size[] = { (GLint)  vertex_shader_code.size  () };
        const GLint  fragment_shaders_size[] = { (GLint)fragment_shader_code.size  () };

        glShaderSource  (  vertex_shader_id, 1,   vertex_shaders_code,   vertex_shaders_size);
        glShaderSource  (fragment_shader_id, 1, fragment_shaders_code, fragment_shaders_size);

        // Se compilan los shaders:

        glCompileShader (  vertex_shader_id);
        glCompileShader (fragment_shader_id);

        // Se comprueba que si la compilación ha tenido éxito:

        glGetShaderiv   (  vertex_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (!succeeded) show_compilation_error (  vertex_shader_id);

        glGetShaderiv   (fragment_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (!succeeded) show_compilation_error (fragment_shader_id);

        // Se crea un objeto para un programa:

        GLuint program_id = glCreateProgram ();

        // Se cargan los shaders compilados en el programa:

        glAttachShader  (program_id,   vertex_shader_id);
        glAttachShader  (program_id, fragment_shader_id);

        // Se linkan los shaders:

        glLinkProgram   (program_id);

        // Se comprueba si el linkage ha tenido éxito:

        glGetProgramiv  (program_id, GL_LINK_STATUS, &succeeded);
        if (!succeeded) show_linkage_error (program_id);

        // Se liberan los shaders compilados una vez se han linkado:

        glDeleteShader (  vertex_shader_id);
        glDeleteShader (fragment_shader_id);

        return (program_id);
    }

    void Scene::show_compilation_error (GLuint shader_id)
    {
        string info_log;
        GLint  info_log_length;

        glGetShaderiv (shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        info_log.resize (info_log_length);

        glGetShaderInfoLog (shader_id, info_log_length, NULL, &info_log.front ());

        cerr << info_log.c_str () << endl;

        #ifdef _MSC_VER
            //OutputDebugStringA (info_log.c_str ());
        #endif

        assert(false);
    }

    void Scene::show_linkage_error (GLuint program_id)
    {
        string info_log;
        GLint  info_log_length;

        glGetProgramiv (program_id, GL_INFO_LOG_LENGTH, &info_log_length);

        info_log.resize (info_log_length);

        glGetProgramInfoLog (program_id, info_log_length, NULL, &info_log.front ());

        cerr << info_log.c_str () << endl;

        #ifdef _MSC_VER
            //OutputDebugStringA (info_log.c_str ());
        #endif

        assert(false);
    }

}
