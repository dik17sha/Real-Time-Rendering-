//  main.cpp

#include<iostream>
#include <fstream>
#include <vector>
#include<algorithm>

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

#include"Mesh.h"
#include "Camera.h"
#include "Model.h"
#include"shader.h"

//Window Settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 1000;

//Calling Camera object
Camera camera(glm::vec3(15.0f, 15.0f, 70.0f));
float lastX = SCR_WIDTH/2.0f;
float lastY = SCR_HEIGHT/2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Function Prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

int main()
{

    // 1. Initializing GLFW
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std ::endl;
        return -1;
    }
    
    //Setting up openGL verison -- only becaue of MAC
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // 2. Creating the Window and Glad loading
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 1", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    //Setting callback functions for resizing and mouse input
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    //Locking cursor to the center of the screen
    glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    
    //Loading GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    std::cout <<"OpenGl version: " << glGetString(GL_VERSION) << std::endl;
    
    // 3. Global OpenGL state
    glEnable(GL_DEPTH_TEST);
    
    // 4. Setup for the Cube Geometry using the classes
    
    //Setting up the shader
    Shader defaultShader("/Users/dchottani/Desktop/Real-Time-Rendering-/Assignment1/shaders/basic.vert", "/Users/dchottani/Desktop/Real-Time-Rendering-/Assignment1/shaders/basic.frag");

    //Loading the model HEREEEEE

    Model myModel("/Users/dchottani/Desktop/Real-Time-Rendering-/Assignment1/Assets/model/snok.obj");
    
    std::cout << "Model loaded successfully. Ready to enter." << std::endl;

    // 5. Main Render Loop
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //render heree
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //Other shaders to load othe models 
        defaultShader.use();

        defaultShader.setVec3("lightPos" , glm::vec3(20.0f, 40.0f, 50.0f));
        defaultShader.setVec3("viewPos", camera.Position);
        defaultShader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);


        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 1000.0f);
        glm::mat4 view = camera.getViewMatrix();

        defaultShader.setMat4("projection", projection);
        defaultShader.setMat4("view", view);

        //Loading the model once 
        glm::mat4 model1Matrix = glm::mat4(1.0f);
        model1Matrix = glm::translate(model1Matrix, glm::vec3(-20.0f, 10.0f, 0.0f));
        model1Matrix = glm::rotate(model1Matrix, (float)glfwGetTime() * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        model1Matrix = glm::scale(model1Matrix, glm::vec3(5.0f));

        defaultShader.setMat4("model", model1Matrix);
        defaultShader.setInt("modelType", 0);
        myModel.Draw(defaultShader);


        // Second
        glm::mat4 model2Matrix = glm::mat4(1.0f);
        model2Matrix = glm::translate(model2Matrix, glm::vec3(15.0f, 10.0f, 0.0f));
        model2Matrix = glm::rotate(model2Matrix, (float)glfwGetTime() * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        model2Matrix = glm::scale(model2Matrix, glm::vec3(5.0f));
        defaultShader.setMat4("model", model2Matrix);
        defaultShader.setInt("modelType", 1);
        myModel.Draw(defaultShader);


        //Third 
        glm::mat4 model3Matrix = glm::mat4(1.0f);
        model3Matrix = glm::translate(model3Matrix, glm::vec3(45.0f, 10.0f, 0.0f));
        model3Matrix = glm::rotate(model3Matrix, (float)glfwGetTime() * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        model3Matrix = glm::scale(model3Matrix, glm::vec3(5.0f));
        defaultShader.setMat4("model", model3Matrix);
        defaultShader.setInt("modelType", 2);
        myModel.Draw(defaultShader);

        // 5.4 Swapping the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // 6. Cleanup

    glfwTerminate();
    return 0;

}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}