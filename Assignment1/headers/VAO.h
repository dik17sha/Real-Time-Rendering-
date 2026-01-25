//
//  VAO.h

#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include"VBO.h"

class VAO
{
public:
    //Refrence ID for VAO
    GLuint ID;
    
    //Constructor
    VAO();
    
    //Linker for VBO and VAO
    void LinkVBO(VBO& VBO, GLuint layout);
    
    //helper functions
    void Bind();
    void Unbind();
    void Delete();

};

#endif

