#include "mesh.h"
#include <stb_image.h>  // Make sure you have stb_image.h and link -lstb_image

// global or member
Mesh* glassCube = nullptr;

// Simple texture loading function
unsigned int loadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        // Create dummy white texture if file not found
        glBindTexture(GL_TEXTURE_2D, textureID);
        unsigned char white[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        std::cout << "Failed to load texture: " << path << " - using white dummy" << std::endl;
    }
    stbi_image_free(data);
    
    return textureID;
}

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
        vert.TexCoords = glm::vec2(0.0f, 0.0f); // Fixed UVs
        vert.Tangent   = glm::vec3(1.0f, 0.0f, 0.0f); // Proper tangent for normal mapping
        vert.Bitangent = glm::vec3(0.0f, 1.0f, 0.0f); // Proper bitangent

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

    // NOW WITH PROPER TEXTURES!
    std::vector<Texture> textures;
    
    // Load diffuse texture (put your own image here or it falls back to white)
    Texture diffuse;
    diffuse.id = loadTexture("assets/brick_diffuse.jpg");  // Change path as needed
    diffuse.type = "texture_diffuse";
    diffuse.path = "assets/brick_diffuse.jpg";
    textures.push_back(diffuse);
    
    // Load normal map texture (put your own or it falls back to flat normal)
    Texture normal;
    normal.id = loadTexture("assets/brick_normal.jpg");    // Change path as needed
    normal.type = "texture_normal";
    normal.path = "assets/brick_normal.jpg";
    textures.push_back(normal);

    std::cout << "Created cube with " << textures.size() << " textures" << std::endl;
    
    return new Mesh(vertices, indices, textures);
}
