#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <array>
#include "shader.h"
#include "camera.h"
#include "model.h" 

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 2.0f, 5.0f)); 
float deltaTime = 0.0f;
float lastFrame = 0.0f; 

bool cameraActive = false; 
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float rotate_speed = 0.0f;
float tiles = 1.0f;
float lod_bias = 0.0f;
bool checkered_texture_active = true;

const std::array<const char *, 6> mipmap_min_list = {"LINEAR_MIPMAP_LINEAR", "NEAREST_MIPMAP_LINEAR", "LINEAR_MIPMAP_NEAREST", "NEAREST_MIPMAP_NEAREST", "LINEAR", "NEAREST"};
int selectedMinConfig = 0;

const std::array<const char *, 2> mipmap_mag_list = {"LINEAR", "NEAREST"};
int selectedMagConfig = 0;

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 4 - Mipmaps (Mac)", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Shader myShader("shaders/basic.vert", "shaders/basic.frag");
    
    Model myModel("assets /tiles/checkered_tile_floor.obj");
    if (myModel.meshes.empty()) {
        std::cout << "!!! MODEL FAILED TO LOAD - Check path: assets /tiles/checkered_tile_floor.obj !!!" << std::endl;
    }

    camera.MovementSpeed = 20.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        ImGuiIO &io = ImGui::GetIO();
        
        static bool cKeyPressed = false;
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            if (!cKeyPressed) { 
                cameraActive = !cameraActive; 
                
                if (cameraActive) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
                    firstMouse = true; 
                } else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);   
                }
                cKeyPressed = true;
            }
        } else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
            cKeyPressed = false; 
        }

        if (!io.WantCaptureKeyboard) {
            processInput(window);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("RTR Assignment 4 - Mipmaps", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Press 'C' to toggle Camera Mouse Control");
        ImGui::Separator();

        ImGui::Text("Min Mapping");
        ImGui::ListBox("Choose mode", &selectedMinConfig, &mipmap_min_list[0], 6);
        ImGui::Separator();
    
        ImGui::Text("Level of Details");
        ImGui::SliderFloat("Change Lod Bias", &lod_bias, -1.0f, 2.0f);
        ImGui::Separator();
        
        ImGui::SliderFloat("Rotation Speed", &rotate_speed, 0.0f, 1.0f);
        ImGui::Separator();
        

        ImGui::Text("Tiles");
        ImGui::SliderFloat("Tile Count", &tiles, 0.0f, 10.0f);
        ImGui::End();

        
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        myShader.use();

        if (!myModel.textures_loaded.empty()) {
            unsigned int textureID = myModel.textures_loaded[0].id;
            glBindTexture(GL_TEXTURE_2D, textureID);


            GLint minFilter = GL_LINEAR_MIPMAP_LINEAR;
            switch (selectedMinConfig) {
                case 0: minFilter = GL_LINEAR_MIPMAP_LINEAR; break;
                case 1: minFilter = GL_NEAREST_MIPMAP_LINEAR; break;
                case 2: minFilter = GL_LINEAR_MIPMAP_NEAREST; break;
                case 3: minFilter = GL_NEAREST_MIPMAP_NEAREST; break;
                case 4: minFilter = GL_LINEAR; break;
                case 5: minFilter = GL_NEAREST; break;
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);


            GLint magFilter = GL_LINEAR;
            switch (selectedMagConfig) {
                case 0: magFilter = GL_LINEAR; break;
                case 1: magFilter = GL_NEAREST; break;
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        }
        

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0f)); 
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        modelMatrix = glm::rotate(modelMatrix, currentFrame * rotate_speed, glm::vec3(0.0f, 1.0f, 0.0f));

        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
        myShader.setMat4("model", modelMatrix);
        

        myShader.setFloat("tiles", tiles);
        myShader.setFloat("lod_bias", lod_bias);

        if (checkered_texture_active) {
            myModel.Draw(myShader); 
        }

        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}



void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    
    if (!cameraActive) return; 

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}