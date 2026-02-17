#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <model.h>
#include<camera.h>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

bool useNormalMap = false; 
float rotationAngle = 0.0f;
float rotationSpeed = 30.0f;


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
glm::vec3 diffuseColor = glm::vec3(0.8f, 0.8f, 0.8f);
glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
float shininess = 32.0f; 
float bumpiness = 0;

void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


int main()
{
    // 1. Initialize GLFW and Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS
    #endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Normal Mapping Demo", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetScrollCallback(window, scroll_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glEnable(GL_DEPTH_TEST);

    //2 Loading shader and models 
    Shader myShader("shaders/normal.vert", "shaders/normal.frag");
    Model myModel("assets /thinker/Rodin_Thinker.obj");
    Model otherModel("assets /box/normal_map_test.obj");
    Model anotherModel("assets /Abox/cube_-_normal_map_test.obj");
    Model manModel("assets /manhole /normal_map_test_-_manhole.obj");


    static bool samplersSet = false;
    if(!samplersSet)
    {
        myShader.use();
        myShader.setInt("texture_diffuse1", 0);
        myShader.setInt("texture_normal1", 1);
        samplersSet = true;
    }

    myShader.setBool("useNormalMap", useNormalMap);

    //3 render 
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame; 
        lastFrame = currentFrame;
        processInput(window);

        rotationAngle += rotationSpeed * deltaTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Mapping Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Separator();
        ImGui::Checkbox("Normal Mapping", &useNormalMap);
        ImGui::Separator();

        //Lighting Controls
        ImGui::SliderFloat("Bump Strength", &bumpiness, 0.0f, 2.0f);
        ImGui::Separator();
        ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.0f, 200.0f);
        ImGui::Text("Current speed: %.1f deg/s", rotationSpeed);
        ImGui::Separator();
        ImGui::End();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myShader.use();
        myShader.setInt("texture_diffuse1", 0);
        myShader.setInt("texture_normal1", 1);
        myShader.setBool("useNormalMap", useNormalMap);
        myShader.setFloat("bumpiness", bumpiness);

        // 4 - lights 
        //glm::vec3 lightPos(sin(currentFrame) * 3.0f, 1.0f, cos(currentFrame) * 3.0f);
        glm::vec3 lightPos(1.5f, 1.0f, 2.0f);
        myShader.setVec3("lightPos", lightPos);
        myShader.setVec3("viewPos", camera.Position);


        // 5 matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
/*
        //Model 1
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f, -1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        myShader.setMat4("model", model);

        // draw calls heree
        myModel.Draw(myShader);
*/
        //Moddel 2
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(4.0f,-1.0f,0.0f));
        model2 = glm::rotate(model2, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model2 = glm::scale(model2, glm::vec3(5.0f));

        myShader.setMat4("model", model2);

        otherModel.Draw(myShader);

        //Model 3
        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(0.0f, -1.0f, 0.0f));
        model3 = glm::rotate(model3, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model3 = glm::scale(model3, glm::vec3(0.01f));
        myShader.setMat4("model", model3);

        anotherModel.Draw(myShader);

        //Model 4
        glm::mat4 model4 = glm::mat4(1.0f);
        model4 = glm::translate(model4, glm::vec3(7.0f,-1.0f, 0.0f));
        model4 = glm::rotate(model4, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model4 = glm::rotate(model4, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        model4 = glm::scale(model4, glm::vec3(2.0f));
        myShader.setMat4("model", model4);

        manModel.Draw(myShader);
    

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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(UP,deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN,deltaTime);
    
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}