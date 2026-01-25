//
//  VBO.h

#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

class VBO
{
public:
    //Refrence ID for VBO
    GLuint ID;
    
    //Constructor
    VBO(GLfloat* vertices, GLsizeiptr size);
    
    
    //helper functions
    void Bind();
    void Unbind();
    void Delete();
};

#endif
