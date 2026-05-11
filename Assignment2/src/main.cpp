#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "headers/camera.h"
#include "headers/shader.h"
#include "headers/model.h" 

#include <iostream>
#include <vector>
#include <string>

// Settings
const unsigned int SCR_WIDTH  = 1500;
const unsigned int SCR_HEIGHT = 1000;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH  / 2.0f;
float lastY = (float)SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadCubemap(const std::vector<std::string>& faces);
unsigned int loadTexture(char const * path);

// GUI State variables
int renderMode = 0; 
float uiIOR = 1.52f; // Using Glass Index of Refraction 
float uiChromaticDispersion = 0.01f;
float uiReflectivity = 0.5f;
bool rotateModels = true;


bool isGuiMode = false; 
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint( GLFW_RESIZABLE, GL_TRUE );
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Reflection, Refraction, Fresnel", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    io.DisplayFramebufferScale = ImVec2(xscale, yscale);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    glEnable(GL_DEPTH_TEST);

    // Shaders
    Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");
    Shader objectShader("shaders/object.vert", "shaders/object.frag");

    // Load Model 
    Model myModel1("assets/teapot/moraccan_teapot.obj");
    Model myModel2("assets/ring/Torus.obj");
    Model myModel3("assets/sphere/sphere.obj");

    // Skybox Geometry
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces {
        "assets/skybox/right.jpg", "assets/skybox/left.jpg",
        "assets/skybox/top.jpg",   "assets/skybox/bottom.jpg",
        "assets/skybox/front.jpg", "assets/skybox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    objectShader.setInt("skybox", 0);
    
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        ImGui::SetNextWindowSize(ImVec2(600 * xscale, 800 * yscale), ImGuiCond_FirstUseEver);       
        ImGui::Begin("Glass Material Properties");
        ImGui::Text("Refelction & Refraction Demo");
        ImGui::Separator();

        ImGui::Text("Render Mode:");
        ImGui::RadioButton("Pure Reflection", &renderMode, 0);
        ImGui::RadioButton("Pure Refraction", &renderMode, 1);
        ImGui::RadioButton("Chromatic Dispersion", &renderMode, 2);
        ImGui::RadioButton("Fresnel Effect", &renderMode, 3);

        ImGui::Separator();
        ImGui::SliderFloat("Index of Refraction", &uiIOR, 1.0f, 2.5f);
        ImGui::SliderFloat("Chromatic Dispersion Slider", &uiChromaticDispersion, 0.0f, 1.0f);
        ImGui::SliderFloat("Reflectivity", &uiReflectivity, 0.0f, 1.0f);
        ImGui::Separator();


        ImGui::Checkbox("Rotate Models", &rotateModels);
        ImGui::Separator();

        
        ImGui::End();

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthFunc(GL_LEQUAL); 

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        

        // --- MODELS ---


        // 1. REFLECTION ONLY (
        objectShader.use();
        objectShader.setFloat("ior", uiIOR);
        objectShader.setFloat("dispersion", uiChromaticDispersion);
        objectShader.setFloat("reflectivity", uiReflectivity);
        objectShader.setInt("effectType", renderMode);


        //Take this out when you uncomment the skybox
        
        //Sphere
        objectShader.setMat4("projection", projection);
        objectShader.setMat4("view", view);
        objectShader.setVec3("cameraPos", camera.Position);

        glm::mat4 modelMatrix1 = glm::mat4(1.0f);
        modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(1.5f, 1.0f, 0.0f));

        if (rotateModels) {
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * 0.4f, glm::vec3(0, 1, 0));
            modelMatrix1 = rotationMatrix * modelMatrix1;  
        }

        modelMatrix1 = glm::scale(modelMatrix1, glm::vec3(0.1f));
        

        glm::mat3 normalMatrix1 = glm::transpose(glm::inverse(glm::mat3(modelMatrix1)));
        objectShader.setMat4("model", modelMatrix1);
        objectShader.setInt("effectType", 0);
        objectShader.setMat3("normalMatrix", normalMatrix1);

        myModel3.Draw(objectShader);


        // 2. REFRACTION ONLY 
        //Ring Donut thing 
        glm::mat4 modelMatrix2 = glm::mat4(1.0f);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0.5f, 1.0f, 0.0f));

        modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.25f)); 


        if (rotateModels) {
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * 0.4f, glm::vec3(0, 1, 0));
            modelMatrix2 = rotationMatrix * modelMatrix2;  
        }

    
        glm::mat3 normalMatrix2 = glm::transpose(glm::inverse(glm::mat3(modelMatrix2)));
        objectShader.setMat3("normalMatrix", normalMatrix2);
        objectShader.setMat4("model", modelMatrix2);
        objectShader.setInt("effectType", 1);
        myModel2.Draw(objectShader);

        // 3. CHROMATIC DIFFUSION
        //Ring Donut thing 
        glm::mat4 modelMatrix3 = glm::mat4(1.0f);
        modelMatrix3 = glm::translate(modelMatrix3, glm::vec3(-1.5f, 1.0f, 0.0f));
        
        if (rotateModels) {
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * 0.4f, glm::vec3(0, 1, 0));
            modelMatrix3 = rotationMatrix * modelMatrix3;  
        }

        modelMatrix3 = glm::scale(modelMatrix3, glm::vec3(0.25f)); 
        

        glm::mat3 normalMatrix3 = glm::transpose(glm::inverse(glm::mat3(modelMatrix3)));
        objectShader.setMat3("normalMatrix", normalMatrix3);
        objectShader.setMat4("model", modelMatrix3);
        objectShader.setInt("effectType", 2);

        myModel2.Draw(objectShader);

        // 3. FRESNEL 
        //Sphere again 
        glm::mat4 modelMatrix4 = glm::mat4(1.0f);
        modelMatrix4 = glm::translate(modelMatrix4, glm::vec3(-3.0f, 1.0f, 0.0f));
        
        if(rotateModels) {
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * 0.4f, glm::vec3(0, 1, 0));
            modelMatrix4 = rotationMatrix * modelMatrix4;  
        } 

        modelMatrix4 = glm::scale(modelMatrix4, glm::vec3(0.1f)); 
        

        glm::mat3 normalMatrix4 = glm::transpose(glm::inverse(glm::mat3(modelMatrix4)));
        objectShader.setMat3("normalMatrix", normalMatrix4);
        objectShader.setMat4("model", modelMatrix4);
        objectShader.setInt("effectType", 3);

        myModel3.Draw(objectShader);

        // --- SKYBOX ---
        skyboxShader.use();

        glm::mat4 skyboxModel = glm::mat4(1.0f);
        skyboxModel = glm::scale(skyboxModel, glm::vec3(50.0f));

        skyboxShader.setMat4("model", skyboxModel);
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        skyboxShader.setMat4("projection", projection);
   
    
        glBindVertexArray(skyboxVAO);
        glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        glDepthFunc(GL_LESS); // set depth function b


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
    // Close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // --- Standard WASD ---
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);


    //This works 
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.Position += camera.Up * camera.MovementSpeed * deltaTime;
    
    
    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        camera.Position -= camera.Up * camera.MovementSpeed * deltaTime;

}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)  
        return;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
    {
        firstMouse = true;  
        return;
    }

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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(const std::vector<std::string>& faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}