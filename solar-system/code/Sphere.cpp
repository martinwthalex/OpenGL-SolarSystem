#include "Sphere.hpp"
#include <vector>
#include <cmath>
#include <cstdint>

namespace udit
{
    static constexpr float PI = 3.14159265359f;

    Sphere::Sphere(float radius, unsigned stacks, unsigned slices)
    {
        std::vector<float> coords;
        std::vector<float> uvs;
        std::vector<uint32_t> indices;

        for (unsigned i = 0; i <= stacks; ++i)
        {
            float v = float(i) / stacks;
            float phi = v * PI;

            for (unsigned j = 0; j <= slices; ++j)
            {
                float u = float(j) / slices;
                float theta = u * 2.f * PI;

                float x = sin(phi) * cos(theta);
                float y = cos(phi);
                float z = sin(phi) * sin(theta);

                coords.push_back(radius * x);
                coords.push_back(radius * y);
                coords.push_back(radius * z);

                uvs.push_back(u);
                uvs.push_back(1.f - v);
            }
        }

        unsigned stride = slices + 1;
        for (unsigned i = 0; i < stacks; ++i)
        {
            for (unsigned j = 0; j < slices; ++j)
            {
                uint32_t a = i * stride + j;
                uint32_t b = (i + 1) * stride + j;
                uint32_t c = (i + 1) * stride + j + 1;
                uint32_t d = i * stride + j + 1;

                indices.insert(indices.end(), { a,b,d, b,c,d });
            }
        }

        index_count = (GLsizei)indices.size();

        glGenBuffers(VBO_COUNT, vbo_ids);
        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);

        // posiciones
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(float), coords.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // uvs
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[UVS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

        // índices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDICES_EBO]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    Sphere::~Sphere()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
    }

    void Sphere::render() const
    {
        glBindVertexArray(vao_id);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    }
}
