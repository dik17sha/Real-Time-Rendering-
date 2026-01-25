//
//  Camera.h
// Might also be too mush for the ass
// cut it down later 

#ifndef CAMERA_H
#define CAMERA_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    YAWRIGHT
};

//Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  05.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

class Camera{
public:
    
    //Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 Down;
    glm::vec3 WorldUp;
    glm::vec3 yawRight;

    //Euler Angles
    float Yaw;
    float Pitch;

    //Camera Options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    
    //Constructor with vectors
    Camera(glm::vec3 postion = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = postion;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    
    //Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }   

    //For calculating te View Matrix
    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
    
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if(direction == FORWARD)
            Position += Front * velocity;
        if(direction == BACKWARD)
            Position -= Front * velocity;
        if(direction == LEFT)
            Position -= Right * velocity;
        if(direction == RIGHT)
            Position += Right * velocity;
        if(direction == UP)
            Position += Up * velocity;
        if(direction == DOWN)
            Position -= Up * velocity;
    
    }   

    void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true)
    {
        xOffset *= MouseSensitivity;
        yOffset *= MouseSensitivity;

        Yaw   += xOffset;
        Pitch += yOffset;

        //Making sure that when pitch is out of bounds, screen doesn't get flipped
        if(constrainPitch)
        {
            if(Pitch > 89.0f)
                Pitch = 89.0f;
            if(Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yOffset)
    {
        Zoom -= (float)yOffset;
        if(Zoom < 1.0f)
            Zoom = 1.0f;
        if(Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    //Calculating the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        //Calculating the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        
        //Also re-calculating the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  
        Up    = glm::normalize(glm::cross(Right, Front));
    }

};

#endif