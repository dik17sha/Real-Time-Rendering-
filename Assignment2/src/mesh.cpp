#include "mesh.h"

// global or member
Mesh* glassCube = nullptr;

Mesh* CreateGlassCube()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // For simplicity, define 24 unique vertices (4 per face) with proper normals
    auto addFace = [&](glm::vec3 n,
                    glm::vec3 v0, glm::vec3 v1,
                    glm::vec3 v2, glm::vec3 v3)
    {
        unsigned int startIndex = (unsigned int)vertices.size();
        Vertex vert{};

        vert.Normal = n;
        vert.TexCoords = glm::vec2(0.0f); // unused
        vert.Tangent   = glm::vec3(0.0f);
        vert.Bitangent = glm::vec3(0.0f);

        vert.Position = v0; vertices.push_back(vert);
        vert.Position = v1; vertices.push_back(vert);
        vert.Position = v2; vertices.push_back(vert);
        vert.Position = v3; vertices.push_back(vert);

        // two triangles: (0,1,2) and (0,2,3)
        indices.push_back(startIndex + 0);
        indices.push_back(startIndex + 1);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 0);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 3);
    };

    float s = 0.5f;
    // back (−Z)
    addFace(glm::vec3(0,0,-1),
            glm::vec3(-s,-s,-s), glm::vec3(s,-s,-s),
            glm::vec3(s, s,-s),  glm::vec3(-s, s,-s));
    // front (+Z)
    addFace(glm::vec3(0,0,1),
            glm::vec3(-s,-s, s), glm::vec3(s,-s, s),
            glm::vec3(s, s, s),  glm::vec3(-s, s, s));
    // left (−X)
    addFace(glm::vec3(-1,0,0),
            glm::vec3(-s,-s,-s), glm::vec3(-s,-s, s),
            glm::vec3(-s, s, s),  glm::vec3(-s, s,-s));
    // right (+X)
    addFace(glm::vec3(1,0,0),
            glm::vec3( s,-s,-s), glm::vec3( s,-s, s),
            glm::vec3( s, s, s),  glm::vec3( s, s,-s));
    // bottom (−Y)
    addFace(glm::vec3(0,-1,0),
            glm::vec3(-s,-s,-s), glm::vec3( s,-s,-s),
            glm::vec3( s,-s, s),  glm::vec3(-s,-s, s));
    // top (+Y)
    addFace(glm::vec3(0,1,0),
            glm::vec3(-s, s,-s), glm::vec3( s, s,-s),
            glm::vec3( s, s, s),  glm::vec3(-s, s, s));

    std::vector<Texture> textures; // empty; we’re not using 2D textures

    return new Mesh(vertices, indices, textures);
}
